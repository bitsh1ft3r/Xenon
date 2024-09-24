#include <iostream>
#include <thread>

#include "IIC.h"

Xe::XCPU::IIC::XenonIIC::XenonIIC()
{
	for (s8 idx = 0; idx < 6; idx++)
	{
		iicState.ppeIntCtrlBlck[idx].extInt = false;
		// Set pending interrupts to 0.
		iicState.ppeIntCtrlBlck[idx].REG_ACK = PRIO_NONE;
	}	
}

bool Xe::XCPU::IIC::XenonIIC::IICActive(u8 ppuID)
{
	if (iicState.ppeIntCtrlBlck[ppuID].REG_INT_MCACK == 0x7C)
	{
		return true; // We're active baby!
	}
	return false;
}

void Xe::XCPU::IIC::XenonIIC::writeInterrupt(u64 intAddress, u64 intData)
{
	u32 mask = 0xF000;
	u8 ppeIntCtrlBlckID = static_cast<u8>((intAddress & mask) >> 12);
	u8 ppeIntCtrlBlckReg = intAddress & 0xFF;
	u8 intType = (intData >> 56) & 0xFF;
	u8 cpusToInterrupt = (intData >> 40) & 0xFF;

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
		// Remove last interrupt in the queue.
		iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.pop_back();

		// Interrupts pending?
		if (!iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.empty() &&
			iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_CURRENT_TSK_PRI <
			iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.back())
		{
			iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].extInt = true;
		}
		break;
	case Xe::XCPU::IIC::EOI_SET_CPU_CURRENT_TSK_PRI:
		// Remove last interrupt in the queue.
		iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.pop_back();
		// Set new Interrupt priority.
		iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_CURRENT_TSK_PRI = static_cast<u32>(_byteswap_uint64(intData));

		// Interrupts pending?
		if (!iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.empty() &&
			iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_CURRENT_TSK_PRI < 
			iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.back())
		{
			iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].extInt = true;
		}
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

	switch (ppeIntCtrlBlckReg)
	{
	case Xe::XCPU::IIC::CPU_CURRENT_TSK_PRI:
		*intData = _byteswap_uint64((u64)iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].REG_CPU_CURRENT_TSK_PRI);
		break;
	case Xe::XCPU::IIC::ACK:
		if (!iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.empty())
		{
			*intData = _byteswap_uint64((u64)iicState.ppeIntCtrlBlck[ppeIntCtrlBlckID].pendingInt.back());
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
	return  iicState.ppeIntCtrlBlck[ppuID].extInt;
}

void Xe::XCPU::IIC::XenonIIC::clearExtInterrupt(u8 ppuID)
{
	iicState.ppeIntCtrlBlck[ppuID].extInt = false;
}

void Xe::XCPU::IIC::XenonIIC::genInterrupt(u8 interruptType, u8 cpusToInterrupt)
{
	for (u8 ppuID = 0; ppuID < 6; ppuID++) 
	{
		if ((cpusToInterrupt & 0x1) == 1)
		{
			// Store the interrupt.
			iicState.ppeIntCtrlBlck[ppuID].pendingInt.push_back(interruptType);
			if (iicState.ppeIntCtrlBlck[ppuID].REG_CPU_CURRENT_TSK_PRI < interruptType)
			{
				// Signal the interrupt.
				iicState.ppeIntCtrlBlck[ppuID].extInt = true;
			}
			else if (iicState.ppeIntCtrlBlck[ppuID].REG_CPU_CURRENT_TSK_PRI == interruptType)
			{
				// If interrupt priority = current int prio merge them as per docs.
				iicState.ppeIntCtrlBlck[ppuID].pendingInt.pop_back();
			}
		}
		cpusToInterrupt = cpusToInterrupt >> 1;
	}
}
