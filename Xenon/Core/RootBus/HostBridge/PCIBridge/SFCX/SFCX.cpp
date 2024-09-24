#include <iostream>

#include "SFCX.h"

SFCX::SFCX()
{

}

bool SFCX::LoadNANDDump(char* nandPath)
{
	return false;
}

void SFCX::Read(u64 readAddress, u64* data, u8 byteCount)
{
	u16 reg = readAddress & 0xFF;

	switch (reg)
	{
	case SFCX_CONFIG_REG:
		*data = sfcxState.configReg;
		break;
	case SFCX_STATUS_REG:
		*data = sfcxState.statusReg;
		break;
	case SFCX_COMMAND_REG:
		*data = sfcxState.commandReg;
		break;
	case SFCX_ADDRESS_REG:
		*data = sfcxState.addressReg;
		break;
	case SFCX_DATA_REG:
		*data = sfcxState.dataReg;
		break;
	case SFCX_LOGICAL_REG:
		*data = sfcxState.logicalReg;
		break;
	case SFCX_PHYSICAL_REG:
		*data = sfcxState.physicalReg;
		break;
	case SFCX_DATAPHYADDR_REG:
		*data = sfcxState.dataPhysAddrReg;
		break;
	case SFCX_SPAREPHYADDR_REG:
		*data = sfcxState.sparePhysAddrReg;
		break;
	case SFCX_MMC_ID_REG:
		*data = sfcxState.mmcIDReg;
		break;
	default:
		std::cout << "SFCX: Read from unknown register 0x" << reg << std::endl;
		break;
	}
}

void SFCX::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	u8 offset = readAddress & 0xFF;
	memcpy(data ,&pciConfigSpace.data[offset], byteCount);
}

void SFCX::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	u16 reg = writeAddress & 0xFF;

	switch (reg)
	{
	case SFCX_CONFIG_REG:
		sfcxState.configReg = (u32)data;
		break;
	case SFCX_STATUS_REG:
		sfcxState.statusReg = (u32)data;
		break;
	case SFCX_COMMAND_REG:
		sfcxState.commandReg = (u32)data;
		break;
	case SFCX_ADDRESS_REG:
		sfcxState.addressReg = (u32)data;
		break;
	case SFCX_DATA_REG:
		sfcxState.dataReg = (u32)data;
		break;
	case SFCX_LOGICAL_REG:
		sfcxState.logicalReg = (u32)data;
		break;
	case SFCX_PHYSICAL_REG:
		sfcxState.physicalReg = (u32)data;
		break;
	case SFCX_DATAPHYADDR_REG:
		sfcxState.dataPhysAddrReg = (u32)data;
		break;
	case SFCX_SPAREPHYADDR_REG:
		sfcxState.sparePhysAddrReg = (u32)data;
		break;
	case SFCX_MMC_ID_REG:
		sfcxState.mmcIDReg = (u32)data;
		break;
	default:
		std::cout << "SFCX: Write to unknown register 0x" << reg << std::endl;
		break;
	}
}

void SFCX::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	u8 offset = writeAddress & 0xFF;
	memcpy(&pciConfigSpace.data[offset], &data, byteCount);
}
