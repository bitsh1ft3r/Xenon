#include <iostream>
#include <thread>

#include "IIC.h"

Xe::XCPU::IIC::XenonIIC::XenonIIC()
{
	for (s8 idx = 0; idx < 6; idx++)
	{
		// Set pending interrupts to 0.
		iicState.ppeIntCtrlBlck[idx].REG_ACK = PRIO_NONE;
	}	
}

void Xe::XCPU::IIC::XenonIIC::writeInterrupt(u64 intAddress, u64 intData)
{
	u32 mask = 0xF000;
	u8 ppeIntCtrlBlckID = static_cast<u8>((intAddress & mask) >> 12);
	u8 ppeIntCtrlBlckReg = intAddress & 0xFF;
	u8 intType = (intData >> 56) & 0xFF;
	u8 cpusToInterrupt = (intData >> 40) & 0xFF;
	size_t intIndex = 0;

	switch (ppeIntCtrlBlckReg)
	{
	case Xe::XCPU::IIC::CPU_WHOAMI:
		iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_WHOAMI = static_cast<u32>(_byteswap_uint64(intData));
		break;
	case Xe::XCPU::IIC::CPU_CURRENT_TSK_PRI:
		iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_CURRENT_TSK_PRI = static_cast<u32>(_byteswap_uint64(intData));
		break;
	case Xe::XCPU::IIC::CPU_IPI_DISPATCH_0:
		iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_IPI_DISPATCH_0 = static_cast<u32>(_byteswap_uint64(intData));
		genInterrupt(intType, cpusToInterrupt);
		break;
	case Xe::XCPU::IIC::INT_0x30:
		// Dont know what this does, lets cause an interrupt?
		genInterrupt(intType, cpusToInterrupt);
		break;
	case Xe::XCPU::IIC::EOI:
		// Remove the last interrupt in the queue that matches our current priority.
		for (auto& interrupt : iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt)
		{
			if (interrupt.ack) // Remove the first interrupt that was ACK'd.
			{
				iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.erase
				(iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.begin() + intIndex);
				continue;
			}
			intIndex++;
		}
		break;
	case Xe::XCPU::IIC::EOI_SET_CPU_CURRENT_TSK_PRI:
		// Remove the last interrupt in the queue that matches our current priority.
		for (auto& interrupt : iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt)
		{
			if (interrupt.ack) // Remove the first interrupt that was ACK'd.
			{
				iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.erase
				(iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.begin() + intIndex);
				continue;
			}
			intIndex++;
		}

		// Set new Interrupt priority.
		iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_CURRENT_TSK_PRI = static_cast<u32>(_byteswap_uint64(intData));
		break;
	case Xe::XCPU::IIC::INT_MCACK:
		iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_INT_MCACK = static_cast<u32>(_byteswap_uint64(intData));
		break;
	default:
		std::cout << "Xenon IIC: Unknown CPU Interrupt Ctrl Blck Reg being written: " << ppeIntCtrlBlckReg 
			<< std::endl;
		break;
	}
}

void Xe::XCPU::IIC::XenonIIC::readInterrupt(u64 intAddress, u64* intData)
{
	u32 mask = 0xF000;
	u8 ppeIntCtrlBlckID = static_cast<u8>((intAddress & mask) >> 12);
	u8 ppeIntCtrlBlckReg = intAddress & 0xFF;
	size_t intIndex = 0;
	switch (ppeIntCtrlBlckReg)
	{
	case Xe::XCPU::IIC::CPU_CURRENT_TSK_PRI:
		*intData = _byteswap_uint64((u64)iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_CURRENT_TSK_PRI);
		break;
	case Xe::XCPU::IIC::ACK:
		if (!iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.empty())
		{
			for (auto& interrupt : iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt)
			{
				if (interrupt.pendingInt >= iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_CURRENT_TSK_PRI)
				{
					*intData = _byteswap_uint64((u64)iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt[intIndex].pendingInt);
					iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt[intIndex].ack = true;
					continue;
				}
				intIndex++;
			}
			return;
		}
		*intData = _byteswap_uint64(PRIO_NONE);
		break;
	default:
		std::cout << "XenonIIC: Unknown interupt being read 0x" << ppeIntCtrlBlckReg << std::endl;
		break;
	}
}

bool Xe::XCPU::IIC::XenonIIC::checkExtInterrupt(u8 ppuID)
{
	if (!iicState.ppeIntCtrlBlck[ppuID].pendingInt.empty())
	{
		for (auto& interrupt : iicState.ppeIntCtrlBlck[ppuID].pendingInt)
		{
			// Check to se if we have a new interrupt.
			// Conditions for signaling:
			// * Interrupt was not ack'd.
			// * Interrupt priority higher than current task priority.
			if (interrupt.pendingInt >= iicState.ppeIntCtrlBlck[ppuID].REG_CPU_CURRENT_TSK_PRI
				&& interrupt.ack != true)
			{
				// Signal the interrupt.
				return true;
			}
		}
	}
	return false;
}

void Xe::XCPU::IIC::XenonIIC::genInterrupt(u8 interruptType, u8 cpusToInterrupt)
{
	XE_INT newInt;
	newInt.ack = false;
	newInt.pendingInt = interruptType;
	for (u8 ppuID = 0; ppuID < 6; ppuID++) 
	{
		if ((cpusToInterrupt & 0x1) == 1)
		{
			for (auto& interrupt : iicState.ppeIntCtrlBlck[ppuID].pendingInt)
			{
				if (interrupt.pendingInt == newInt.pendingInt)
				{
					// An interrupt with the same priority and type exists.
					return;
				}
			}
			// Store the interrupt.
			iicState.ppeIntCtrlBlck[ppuID].pendingInt.push_back(newInt);
		}
		cpusToInterrupt = cpusToInterrupt >> 1;
	}
}
