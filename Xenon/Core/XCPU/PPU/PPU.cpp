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

	// Zero out PPU state.
	memset(&ppuState, 0, sizeof(PPU_STATE));

	// Initialize Both threads as in a Reset.
	for (u8 thrdNum = 0; thrdNum < 2; thrdNum++)
	{
		// Set Reset vector for both threads
		ppuState.ppuThread[thrdNum].NIA = XE_RESET_VECTOR;
		// Set MSR for both Threads
		ppuState.ppuThread[thrdNum].SPR.MSR.MSR_Hex = 0x9000000000000000;
	}

	// Set Thread Timeout Register.
	ppuState.SPR.TTR = 0x1000; // Execute 4096 instructions.
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
		ppuState.ppuThread[thrdID].ppuRes = new PPU_RES;
		memset(ppuState.ppuThread[thrdID].ppuRes, 0, sizeof(PPU_RES));
		xenonContext->xenonRes.Register(ppuState.ppuThread[thrdID].ppuRes);
	}

	// Set PPU Name
	ppuState.ppuName = ppuName;

	// Set PVR and PIR
	ppuState.SPR.PVR.PVR_Hex = PVR;
	ppuState.ppuThread[PPU_THREAD_0].SPR.PIR = PIR;
	ppuState.ppuThread[PPU_THREAD_1].SPR.PIR = PIR + 1;

	// PPU is running!
	ppuRunning = true;

	// TLB Software reload Mode?
	ppuState.SPR.LPCR = 0x0000000000000402;

	// HID6?
	ppuState.SPR.HID6 = 0x0001803800000000;

	// TSCR[WEXT] = 1??
	ppuState.SPR.TSCR = 0x100000;

	// If we're PPU0,thread0 then enable THRD 0 and set Reset Vector.
	if (ppuState.ppuName == "PPU0")
	{
		ppuState.SPR.CTRL = 0x800000; // CTRL[TE0] = 1;
		ppuState.SPR.HRMOR = 0x0000020000000000;
		ppuState.ppuThread[PPU_THREAD_0].NIA = 0x20000000100;
	}
}

// PPU Entry Point.
void PPU::StartExecution()
{
	// While the CPU is running
	while (ppuRunning)
	{
		// See if we have any threads active.
		if (getCurrentRunningThreads() != PPU_THREAD_NONE)
		{
			// We have some threads active!
			
			// Check if the 1st thread is active and process instructions on it.
			if (getCurrentRunningThreads() == PPU_THREAD_0 || getCurrentRunningThreads() == PPU_THREAD_BOTH)
			{
				// Thread 0 is running, process instructions until we reach TTR timeout.
				ppuState.currentThread = PPU_THREAD_0;

				// Start a timer
				const auto timerStart{ std::chrono::steady_clock::now() };

				// Loop on this thread for the amount of Instructions that TTR tells us.
				for (size_t instrCount = 0; instrCount < ppuState.SPR.TTR; instrCount++)
				{
					// Main processing loop.

					// Check External Interrupts
					if (xenonContext->xenonIIC.checkExtInterrupt(ppuState.ppuThread[ppuState.currentThread].SPR.PIR))
					{
						PPCInterpreter::ppcExternalException(&ppuState);
						xenonContext->xenonIIC.clearExtInterrupt(ppuState.ppuThread[ppuState.currentThread].SPR.PIR);
					}

					// Read Next Intruction from Memory.
					ppuReadNextInstruction();

					// If interrupts ocurred, process them, else execute next instruction.
					if (!ppuState.ppuThread[ppuState.currentThread].exceptionOcurred)
					{
						PPCInterpreter::ppcExecuteSingleInstruction(&ppuState);
					}

					// Exception raised, we can disbale the flag now.
					ppuState.ppuThread[ppuState.currentThread].exceptionOcurred = false;

					// Increase Time Base Counter
					if (xenonContext->timeBaseActive)
					{
						ppuState.SPR.TB++;
					}
				}

				const auto timerStop{ std::chrono::steady_clock::now() };

				const std::chrono::duration<double> elapsed_seconds{ timerStop - timerStart };
				//std::cout << "PPU(" << ppuState.ppuName << ") Thread " << ppuState.currentThread
				//	<< " Executed " << ppuState.SPR.TTR << " instructions in " << elapsed_seconds << " seconds" << std::endl;
			}
			// Check again for the 2nd thread.
			if (getCurrentRunningThreads() == PPU_THREAD_1 || getCurrentRunningThreads() == PPU_THREAD_BOTH)
			{
				// Thread 0 is running, process instructions until we reach TTR timeout.
				ppuState.currentThread = PPU_THREAD_1;

				// Start a timer
				const auto timerStart{ std::chrono::steady_clock::now() };

				// Loop on this thread for the amount of Instructions that TTR tells us.
				for (size_t instrCount = 0; instrCount < ppuState.SPR.TTR; instrCount++)
				{
					// Main processing loop.

					// Check External Interrupts
					if (xenonContext->xenonIIC.checkExtInterrupt(ppuState.ppuThread[ppuState.currentThread].SPR.PIR))
					{
						PPCInterpreter::ppcExternalException(&ppuState);
						xenonContext->xenonIIC.clearExtInterrupt(ppuState.ppuThread[ppuState.currentThread].SPR.PIR);
					}

					// Read Next Intruction from Memory.
					ppuReadNextInstruction();

					// If interrupts ocurred, process them, else execute next instruction.
					if (!ppuState.ppuThread[ppuState.currentThread].exceptionOcurred)
					{
						PPCInterpreter::ppcExecuteSingleInstruction(&ppuState);
					}

					// Exception raised, we can disbale the flag now.
					ppuState.ppuThread[ppuState.currentThread].exceptionOcurred = false;

					// Increase Time Base Counter
					if (xenonContext->timeBaseActive)
					{
						ppuState.SPR.TB++;
					}
				}

				const auto timerStop{ std::chrono::steady_clock::now() };

				const std::chrono::duration<double> elapsed_seconds{ timerStop - timerStart };
				//std::cout << "PPU(" << ppuState.ppuName << ") Thread " << ppuState.currentThread
				//	<< " Executed " << ppuState.SPR.TTR << " instructions in " << elapsed_seconds << " seconds" << std::endl;
			}

		}

		//
		// Check for external interrupts that enable us if we're allowed to.
		//
		
		// If TSCR[WEXT] = ‘1’, wake up at System Reset and set SRR1[42:44] = ‘100’.
		bool WEXT = ppuState.SPR.TSCR & 0x100000;
		if (xenonContext->xenonIIC.checkExtInterrupt(ppuState.ppuThread[ppuState.currentThread].SPR.PIR)
			&& WEXT)
		{
			xenonContext->xenonIIC.clearExtInterrupt(ppuState.ppuThread[ppuState.currentThread].SPR.PIR);
			// Great, someone started us! Let's enable THRD0.
			ppuState.SPR.CTRL = 0x800000;
			// Issue reset!
			ppuState.ppuThread[ppuState.currentThread].CIA = 0x100;				// Set CIA to 0x100 as per docs.
			ppuState.ppuThread[ppuState.currentThread].SPR.SRR1 = 0x200000;		// Set SRR1 42-44 = 100
			// EOI + INT_PRIO = 0
			xenonContext->xenonIIC.writeInterrupt(0x50000 + ppuState.ppuThread[ppuState.currentThread].SPR.PIR * 0x1000 + 0x68, 0);
		}
	}
}

PPU_THREAD_REGISTERS* PPU::GetPPUThread(u8 thrdID)
{
	return &this->ppuState.ppuThread[thrdID];
}

void PPU::ppuReadNextInstruction()
{
	// Update CIA.
	ppuState.ppuThread[ppuState.currentThread].CIA =
		ppuState.ppuThread[ppuState.currentThread].NIA;
	// Increase Next Instruction Address.
	ppuState.ppuThread[ppuState.currentThread].NIA += 4;
	ppuState.ppuThread[ppuState.currentThread].iFetch = true;
	// Fetch the instruction from memory.
	ppuState.ppuThread[ppuState.currentThread].CI =
		PPCInterpreter::MMURead32(&ppuState, ppuState.ppuThread[ppuState.currentThread].CIA);
	ppuState.ppuThread[ppuState.currentThread].iFetch = false;
}

// Returns current executing thread by reading CTRL register.
PPU_THREAD PPU::getCurrentRunningThreads()
{
	// Check CTRL Register CTRL>TE[0,1];
	u8 ctrlTE = (ppuState.SPR.CTRL & 0xC00000) >> 22;
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