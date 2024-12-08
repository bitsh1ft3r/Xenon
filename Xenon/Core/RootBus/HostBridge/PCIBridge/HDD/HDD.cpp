#include "HDD.h"

HDD::HDD()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x58031414;
	memcpy(&pciConfigSpace.data[0], &devID, 4);

	// Device ready to receive commands.
	ataDeviceState.ataRegState.status = ATA_STATUS_DRDY;

	// Set the ATA Indentify data.

}

void HDD::Read(u64 readAddress, u64* data, u8 byteCount)
{
	u32 regOffset = (readAddress & 0xFF) * 4;

	memcpy(&data, &ataDeviceState.ataRegState + regOffset, byteCount);

	if (regOffset < sizeof(ATA_REG_STATE))
	{
		switch (regOffset)
		{
		case ATA_REG_DATA:
			break;
		case ATA_REG_ERROR:
			break;
		case ATA_REG_SECTORCOUNT:
			break;
		case ATA_REG_LBA_LOW:
			break;
		case ATA_REG_LBA_MED:
			break;
		case ATA_REG_LBA_HI:
			break;
		case ATA_REG_DEV_SEL:
			break;
		case ATA_REG_CMD_STATUS:
			break;
		case ATA_REG_DEV_CTRL:
			break;
			break;
		default:
			break;
		}
	}
	else
	{
		std::cout << "ATA: Unknown register being accesed: (Read)0x" << regOffset << std::endl;
	}
}

void HDD::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	u32 regOffset = (writeAddress & 0xFF) * 4;

	memcpy(&ataDeviceState.ataRegState + regOffset, &data, byteCount);
	if (regOffset < sizeof(ATA_REG_STATE))
	{
		switch (regOffset)
		{
		case ATA_REG_CMD_STATUS:
			switch (data)
			{
			case ATA_COMMAND_DEVICE_RESET:
				break;
			case ATA_COMMAND_READ_SECTORS:
				break;
			case ATA_COMMAND_READ_DMA_EXT:
				break;
			case ATA_COMMAND_WRITE_SECTORS:
				break;
			case ATA_COMMAND_WRITE_DMA_EXT:
				break;
			case ATA_COMMAND_VERIFY:
				break;
			case ATA_COMMAND_VERIFY_EXT:
				break;
			case ATA_COMMAND_SET_DEVICE_PARAMETERS:
				break;
			case ATA_COMMAND_PACKET:
				break;
			case ATA_COMMAND_IDENTIFY_PACKET_DEVICE:
				break;
			case ATA_COMMAND_READ_MULTIPLE:
				break;
			case ATA_COMMAND_WRITE_MULTIPLE:
				break;
			case ATA_COMMAND_SET_MULTIPLE_MODE:
				break;
			case ATA_COMMAND_READ_DMA:
				break;
			case ATA_COMMAND_WRITE_DMA:
				break;
			case ATA_COMMAND_STANDBY_IMMEDIATE:
				break;
			case ATA_COMMAND_FLUSH_CACHE:
				break;
			case ATA_COMMAND_IDENTIFY_DEVICE:
				break;
			case ATA_COMMAND_SET_FEATURES:
				break;
			case ATA_COMMAND_SECURITY_SET_PASSWORD:
				break;
			case ATA_COMMAND_SECURITY_UNLOCK:
				break;
			case ATA_COMMAND_SECURITY_DISABLE_PASSWORD:
				break;
			default:
				break;
			}
		default:
			break;
		}
	}
	else
	{
		std::cout << "ATA: Unknown register being accesed: (Write)0x" << regOffset << std::endl;
	}
}

void HDD::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void HDD::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
