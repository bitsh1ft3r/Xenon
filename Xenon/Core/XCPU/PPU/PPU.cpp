#include <thread>
#include <chrono>
#include <assert.h>

#include "PPU.h"
#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

PPU::PPU()
{
	//
	// Set evrything as in POR. See CELL-BE Programming Handbook.
	// 

	// Allocate memory for our PPU state.
	ppuState = new PPU_STATE;

	// Zero out PPU state.
	memset(ppuState, 0, sizeof(PPU_STATE));

	// Initialize Both threads as in a Reset.
	for (u8 thrdNum = 0; thrdNum < 2; thrdNum++)
	{
		// Set Reset vector for both threads
		ppuState->ppuThread[thrdNum].NIA = XE_RESET_VECTOR;
		// Set MSR for both Threads
		ppuState->ppuThread[thrdNum].SPR.MSR.MSR_Hex = 0x9000000000000000;
	}

	// Set Thread Timeout Register.
	ppuState->SPR.TTR = 0x1000; // Execute 4096 instructions.
}

void PPU::Initialize(XENON_CONTEXT* inXenonContext, RootBus* mainBus, u32 PVR, u32 PIR,
	const char* ppuName)
{
	// Asign global Xenon context.
	xenonContext = inXenonContext;

	// Asign Interpreter global variables.
	PPCInterpreter::intXCPUContext = xenonContext;
	PPCInterpreter::sysBus = mainBus;

	for (u8 thrdID = 0; thrdID < 2; thrdID++)
	{
		ppuState->ppuThread[thrdID].ppuRes = new PPU_RES;
		memset(ppuState->ppuThread[thrdID].ppuRes, 0, sizeof(PPU_RES));
		xenonContext->xenonRes.Register(ppuState->ppuThread[thrdID].ppuRes);
	}

	// Set PPU Name
	ppuState->ppuName = ppuName;

	// Set PVR and PIR
	ppuState->SPR.PVR.PVR_Hex = PVR;
	ppuState->ppuThread[PPU_THREAD_0].SPR.PIR = PIR;
	ppuState->ppuThread[PPU_THREAD_1].SPR.PIR = PIR + 1;

	// PPU is running!
	ppuRunning = true;

	// TLB Software reload Mode?
	ppuState->SPR.LPCR = 0x0000000000000402;

	// HID6?
	ppuState->SPR.HID6 = 0x0001803800000000;

	// TSCR[WEXT] = 1??
	ppuState->SPR.TSCR = 0x100000;

	// If we're PPU0,thread0 then enable THRD 0 and set Reset Vector.
	if (strcmp(ppuState->ppuName,"PPU0") == false)
	{
		ppuState->SPR.CTRL = 0x800000; // CTRL[TE0] = 1;
		ppuState->SPR.HRMOR = 0x0000020000000000;
		ppuState->ppuThread[PPU_THREAD_0].NIA = 0x20000000100;
	}
}

// PPU Entry Point.
void PPU::StartExecution()
{
	// While the CPU is running
	while (ppuRunning)
	{
		// See if we have any threads active.
		while (getCurrentRunningThreads() != PPU_THREAD_NONE)
		{
			// We have some threads active!
			
			// Check if the 1st thread is active and process instructions on it.
			if (getCurrentRunningThreads() == PPU_THREAD_0 || getCurrentRunningThreads() == PPU_THREAD_BOTH)
			{
				// Thread 0 is running, process instructions until we reach TTR timeout.
				ppuState->currentThread = PPU_THREAD_0;

				// Loop on this thread for the amount of Instructions that TTR tells us.
				for (size_t instrCount = 0; instrCount < ppuState->SPR.TTR; instrCount++)
				{
					// Main processing loop.

					// Read next intruction from Memory.
					if (ppuReadNextInstruction())
					{
						// Execute next intrucrtion.
						PPCInterpreter::ppcExecuteSingleInstruction(ppuState);

						// Increase Time Base Counter
						if (xenonContext->timeBaseActive)
						{
							ppuState->SPR.TB++;
							// Decrease the Decrementer.
							ppuState->ppuThread[ppuState->currentThread].SPR.DEC -= 1;
						}
					}

					// Check if the IIC has an external interrupt pending if External interrupts are enabled.
					if (ppuState->ppuThread[ppuState->currentThread].SPR.MSR.EE)
					{
						if (xenonContext->xenonIIC.checkExtInterrupt(ppuState->ppuThread[ppuState->currentThread].SPR.PIR))
						{
							ppuState->ppuThread[ppuState->currentThread].exceptReg |= PPU_EX_EXT;
						}
					}

					// Check Exceptions pending.
					ppuCheckExceptions();
				}
			}
			// Check again for the 2nd thread.
			if (getCurrentRunningThreads() == PPU_THREAD_1 || getCurrentRunningThreads() == PPU_THREAD_BOTH)
			{
				// Thread 0 is running, process instructions until we reach TTR timeout.
				ppuState->currentThread = PPU_THREAD_1;

				// Loop on this thread for the amount of Instructions that TTR tells us.
				for (size_t instrCount = 0; instrCount < ppuState->SPR.TTR; instrCount++)
				{
					// Main processing loop.
					// 
					// Read next intruction from Memory.
					if (ppuReadNextInstruction())
					{
						// Execute next intrucrtion.
						PPCInterpreter::ppcExecuteSingleInstruction(ppuState);

						// Increase Time Base Counter
						if (xenonContext->timeBaseActive)
						{
							ppuState->SPR.TB++;
							// Decrease the Decrementer.
							ppuState->ppuThread[ppuState->currentThread].SPR.DEC -= 1;
						}
					}	

					// Check if the IIC has an external interrupt pending if External interrupts are enabled.
					if (ppuState->ppuThread[ppuState->currentThread].SPR.MSR.EE)
					{
						if (xenonContext->xenonIIC.checkExtInterrupt(ppuState->ppuThread[ppuState->currentThread].SPR.PIR))
						{
							ppuState->ppuThread[ppuState->currentThread].exceptReg |= PPU_EX_EXT;
						}
					}

					// Check Exceptions pending.
					ppuCheckExceptions();
				}
			}
		}

		//
		// Check for external interrupts that enable us if we're allowed to.
		//
		
		// If TSCR[WEXT] = ‘1’, wake up at System Reset and set SRR1[42:44] = ‘100’.
		bool WEXT = (ppuState->SPR.TSCR & 0x100000) >> 20;
		if (xenonContext->xenonIIC.checkExtInterrupt(ppuState->ppuThread[ppuState->currentThread].SPR.PIR)
			&& WEXT)
		{
			// Great, someone started us! Let's enable THRD0.
			ppuState->SPR.CTRL = 0x800000;
			// Issue reset!
			ppuState->ppuThread[PPU_THREAD_0].exceptReg |= PPU_EX_RESET;	
			ppuState->ppuThread[PPU_THREAD_1].exceptReg |= PPU_EX_RESET;			// Set CIA to 0x100 as per docs.
			ppuState->ppuThread[ppuState->currentThread].SPR.SRR1 = 0x200000;		// Set SRR1 42-44 = 100
		}
	}
}

PPU_THREAD_REGISTERS* PPU::GetPPUThread(u8 thrdID)
{
	return &this->ppuState->ppuThread[thrdID];
}

bool PPU::ppuReadNextInstruction()
{
	// Update CIA.
	ppuState->ppuThread[ppuState->currentThread].CIA =
		ppuState->ppuThread[ppuState->currentThread].NIA;
	// Increase Next Instruction Address.
	ppuState->ppuThread[ppuState->currentThread].NIA += 4;
	ppuState->ppuThread[ppuState->currentThread].iFetch = true;
	// Fetch the instruction from memory.
	ppuState->ppuThread[ppuState->currentThread].CI =
		PPCInterpreter::MMURead32(ppuState, ppuState->ppuThread[ppuState->currentThread].CIA);
	if (ppuState->ppuThread[ppuState->currentThread].exceptReg & PPU_EX_INSSTOR ||
		ppuState->ppuThread[ppuState->currentThread].exceptReg & PPU_EX_INSTSEGM)
	{
		return false;
	}
	ppuState->ppuThread[ppuState->currentThread].iFetch = false;
	return true;
}

void PPU::ppuCheckExceptions()
{
	// Check Exceptions pending and process them in order.
	u16 exceptions = ppuState->ppuThread[ppuState->currentThread].exceptReg;
	if (exceptions != PPU_EX_NONE)
	{
		// Non Maskable:

		//
		// 1. System Reset
		//
		if (exceptions & PPU_EX_RESET)
		{
			PPCInterpreter::ppcResetException(ppuState);
			exceptions &= ~PPU_EX_RESET;
			goto end;
		}

		//
		// 2. Machine Check
		//
		if (exceptions & PPU_EX_MC)
		{
			if (ppuState->ppuThread[ppuState->currentThread].SPR.MSR.ME)
			{
				PPCInterpreter::ppcResetException(ppuState);
				exceptions &= ~PPU_EX_MC;
				goto end;
			}
			else
			{
				// Checkstop Mode. Hard Fault.
				std::cout << "[" << ppuState->ppuName << "] CHECKSTOP!" << std::endl;
				// TODO: Properly end execution.
				// A checkstop is a full - stop of the processor that requires a System Reset to
				// recover.
				system("PAUSE");
			}
		}

		// Maskable
		
		//
		// 3. Instruction-Dependent
		//

		// A. Program - Illegal Instruction
		if (exceptions & PPU_EX_PROG && ppuState->ppuThread[ppuState->currentThread].exceptTrapType == 44)
		{
			std::cout << "[" << ppuState->ppuName << "](THRD" << ppuState->currentThread << "): Unhandled Exception: "
				<< "Illegal Instruction" << std::endl;
			exceptions &= ~PPU_EX_PROG;
			goto end;
		}			
		// B. Floating-Point Unavailable
		if (exceptions & PPU_EX_FPU)
		{
			std::cout << "[" << ppuState->ppuName << "](THRD" << ppuState->currentThread << "): Unhandled Exception: "
				<< "Floating-Point Unavailable" << std::endl;
			exceptions &= ~PPU_EX_FPU;
			goto end;
		}
		// C. Data Storage, Data Segment, or Alignment
		// Data Storage
		if (exceptions & PPU_EX_DATASTOR)
		{
			PPCInterpreter::ppcDataStorageException(ppuState, DMASK(1,1));
			exceptions &= ~PPU_EX_DATASTOR;
			goto end;
		}
		// Data Segment
		if (exceptions & PPU_EX_DATASEGM)
		{
			PPCInterpreter::ppcDataSegmentException(ppuState);
			exceptions &= ~PPU_EX_DATASEGM;
			goto end;
		}
		// Alignment
		if (exceptions & PPU_EX_ALIGNM)
		{
			std::cout << "[" << ppuState->ppuName << "](THRD" << ppuState->currentThread << "): Unhandled Exception: "
				<< "Alignment" << std::endl;
			exceptions &= ~PPU_EX_ALIGNM;
			goto end;
		}
		// D. Trace
		if (exceptions & PPU_EX_TRACE)
		{
			std::cout << "[" << ppuState->ppuName << "](THRD" << ppuState->currentThread << "): Unhandled Exception: "
				<< "Trace" << std::endl;
			exceptions &= ~PPU_EX_TRACE;
			goto end;
		}
		// E. Program Trap, System Call, Program Priv Inst, Program Illegal Inst
		// Program Trap
		if (exceptions & PPU_EX_PROG && ppuState->ppuThread[ppuState->currentThread].exceptTrapType == 46)
		{
			PPCInterpreter::ppcProgramException(ppuState);
			exceptions &= ~PPU_EX_PROG;
			goto end;
		}
		// System Call
		if (exceptions & PPU_EX_SC)
		{
			PPCInterpreter::ppcSystemCallException(ppuState);
			exceptions &= ~PPU_EX_SC;
			goto end;
		}
		// Program - Privileged Instruction
		if (exceptions & PPU_EX_PROG && ppuState->ppuThread[ppuState->currentThread].exceptTrapType == 45)
		{
			std::cout << "[" << ppuState->ppuName << "](THRD" << ppuState->currentThread << "): Unhandled Exception: "
				<< "Privileged Instruction" << std::endl;
			exceptions &= ~PPU_EX_PROG;
			goto end;
			}
		// F. Instruction Storage and Instruction Segment
		// Instruction Storage
		if (exceptions & PPU_EX_INSSTOR)
		{
			PPCInterpreter::ppcInstStorageException(ppuState);
			exceptions &= ~PPU_EX_INSSTOR;
			goto end;
		}
		// Instruction Segment
		if (exceptions & PPU_EX_INSTSEGM)
		{
			PPCInterpreter::ppcInstSegmentException(ppuState);
			exceptions &= ~PPU_EX_INSTSEGM;
			goto end;
		}

		//
		// 4. Program - Imprecise Mode Floating-Point Enabled Exception
		//

		if (exceptions & PPU_EX_PROG)
		{
			std::cout << "[" << ppuState->ppuName << "](THRD" << ppuState->currentThread << "): Unhandled Exception: "
				<< "Imprecise Mode Floating-Point Enabled Exception" << std::endl;
			exceptions &= ~PPU_EX_PROG;
			goto end;
		}

		//
		// 5. External, Decrementer, and Hypervisor Decrementer
		//

		// External
		if (exceptions & PPU_EX_EXT && ppuState->ppuThread[ppuState->currentThread].SPR.MSR.EE)
		{
			PPCInterpreter::ppcExternalException(ppuState);
			exceptions &= ~PPU_EX_EXT;
			goto end;
		}
		// Decrementer
		if (exceptions & PPU_EX_DEC)
		{
			PPCInterpreter::ppcDecrementerException(ppuState);
			exceptions &= ~PPU_EX_DEC;
			goto end;
		}
		// Hypervisor Decrementer
		if (exceptions & PPU_EX_HDEC)
		{
			std::cout << "[" << ppuState->ppuName << "](THRD" << ppuState->currentThread << "): Unhandled Exception: "
				<< "Hypervisor Decrementer" << std::endl;
			exceptions &= ~PPU_EX_HDEC;
			goto end;
		}

		// Set the new value for our exception register.
			end:
		ppuState->ppuThread[ppuState->currentThread].exceptReg = exceptions;
	}
}

// Returns current executing thread by reading CTRL register.
PPU_THREAD PPU::getCurrentRunningThreads()
{
	// Check CTRL Register CTRL>TE[0,1];
	u8 ctrlTE = (ppuState->SPR.CTRL & 0xC00000) >> 22;
	switch (ctrlTE)
	{
	case 0b10:
		return PPU_THREAD_0;
	case 0b01:
		return PPU_THREAD_1;
	case 0b11:
		return PPU_THREAD_BOTH;
	default:
		return PPU_THREAD_NONE;
	}
}