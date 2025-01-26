// Copyright 2025 Xenon Emulator Project

#include <fstream>
#include <iostream>

#include "SFCX.h"

SFCX::SFCX(const std::string nandLoadPath, PCIBridge* parentPCIBridge)
{
	// Asign parent PCI Bridge pointer.
	parentBus = parentPCIBridge;

	// Dev & Vendor ID
	pciConfigSpace.configSpaceHeader.reg0.vendorID = 0x1414;
	pciConfigSpace.configSpaceHeader.reg0.deviceID = 0x580B;

	std::cout << "Xenon Secure Flash Controller for Xbox" << std::endl;
	
	// Set the registers as a dump from my Corona 16MB. These were dumped at POR via Xell before SFCX Init.
	// These are also readable via JRunner and simple360 flasher. 
	
	// Xenon Dev Kit ES DD2 64 MB
	// 0x01198030 
	
	// Corona 16 Megs Retail
	// 0x000043000

	sfcxState.configReg = 0x00043000; // Config Reg is VERY Important. Tells info about Meta/NAND Type.
	sfcxState.statusReg = 0x00000600;
	sfcxState.statusReg = 0x00000600;
	sfcxState.addressReg = 0x00f70030;
	sfcxState.logicalReg = 0x00000100;
	sfcxState.physicalReg = 0x0000100;
	sfcxState.commandReg = NO_CMD;

	// Load the NAND dump.
	std::cout << "SFCX: Loading NAND from path: " << nandLoadPath << std::endl;
	
	fopen_s(&nandFile, nandLoadPath.c_str(), "rb");

	if (!nandFile)
	{
		std::cout << "SFCX: Fatal error, check your nand dump path." << std::endl;
		system("PAUSE");
	}

	// Check file magic.
	if (!checkMagic())
	{
		std::cout << "SFCX: Fatal error, loaded faile magic does'nt correspond to Xbox 360 NAND." << std::endl;
		system("PAUSE");
	}

	// Load NAND header and display info about it.
	fseek(nandFile, 0, SEEK_SET);
	fread_s(&sfcxState.nandHeader, sizeof(NAND_HEADER), sizeof(NAND_HEADER), 1, nandFile);
	// Fix Endiannes
	sfcxState.nandHeader.nandMagic = _byteswap_ushort(sfcxState.nandHeader.nandMagic);
	std::cout << " * NAND Magic: 0x" << std::hex << sfcxState.nandHeader.nandMagic << std::endl;

	sfcxState.nandHeader.build = _byteswap_ushort(sfcxState.nandHeader.build);
	std::cout << " * Build: 0x" << sfcxState.nandHeader.build << std::endl;

	sfcxState.nandHeader.qfe = _byteswap_ushort(sfcxState.nandHeader.qfe);
	sfcxState.nandHeader.flags = _byteswap_ushort(sfcxState.nandHeader.flags);

	sfcxState.nandHeader.entry = _byteswap_ulong(sfcxState.nandHeader.entry);
	std::cout << " * Entry: 0x" << sfcxState.nandHeader.entry << std::endl;

	sfcxState.nandHeader.size = _byteswap_ulong(sfcxState.nandHeader.size);
	std::cout << " * Size: 0x" << sfcxState.nandHeader.size << std::endl;

	sfcxState.nandHeader.keyvaultSize = _byteswap_ulong(sfcxState.nandHeader.keyvaultSize);
	std::cout << " * Keyvault Size: 0x" << sfcxState.nandHeader.keyvaultSize << std::endl;

	sfcxState.nandHeader.sysUpdateAddr = _byteswap_ulong(sfcxState.nandHeader.sysUpdateAddr);
	std::cout << " * System Update Addr: 0x" << sfcxState.nandHeader.sysUpdateAddr << std::endl;

	sfcxState.nandHeader.sysUpdateCount = _byteswap_ushort(sfcxState.nandHeader.sysUpdateCount);
	std::cout << " * System Update Count: 0x" << sfcxState.nandHeader.sysUpdateCount << std::endl;

	sfcxState.nandHeader.keyvaultVer = _byteswap_ushort(sfcxState.nandHeader.keyvaultVer);
	std::cout << " * Keyvault Ver: 0x" << sfcxState.nandHeader.keyvaultVer << std::endl;

	sfcxState.nandHeader.keyvaultAddr = _byteswap_ulong(sfcxState.nandHeader.keyvaultAddr);
	std::cout << " * Keyvault Addr: 0x" << sfcxState.nandHeader.keyvaultAddr << std::endl;

	sfcxState.nandHeader.sysUpdateSize = _byteswap_ulong(sfcxState.nandHeader.sysUpdateSize);
	std::cout << " * System Update Size: 0x" << sfcxState.nandHeader.sysUpdateSize << std::endl;

	sfcxState.nandHeader.smcConfigAddr = _byteswap_ulong(sfcxState.nandHeader.smcConfigAddr);
	std::cout << " * SMC Config Addr: 0x" << sfcxState.nandHeader.smcConfigAddr << std::endl;

	sfcxState.nandHeader.smcBootSize = _byteswap_ulong(sfcxState.nandHeader.smcBootSize);
	std::cout << " * SMC Boot Size: 0x" << sfcxState.nandHeader.smcBootSize << std::endl;

	sfcxState.nandHeader.smcBootAddr = _byteswap_ulong(sfcxState.nandHeader.smcBootAddr);
	std::cout << " * SMC Boot Addr: 0x" << sfcxState.nandHeader.smcBootAddr << std::endl;

	// Check Image size and Meta type.
	size_t imageSize = 0;
	fseek(nandFile, 0, SEEK_END);
	imageSize = ftell(nandFile);
	fseek(nandFile, 0, SEEK_SET);

	// There are two SFCX Versions, original (Pre Jasper) and Jasper+.


	// Enter SFCX Thread.
	sfcxThread = std::thread(&SFCX::sfcxMainLoop, this);
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
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

void SFCX::sfcxMainLoop()
{
	// Config register should be initialized by now.
	while (true)
	{
		// Did we got a command?
		if (sfcxState.commandReg != NO_CMD)
		{
			// Set status to busy.
			sfcxState.statusReg |= STATUS_BUSY;

			// Check the command reg to see what command was issued.
			switch (sfcxState.commandReg)
			{
			case PAGE_BUF_TO_REG:
				// If we're reading from data buffer to data reg the Address reg becomes our buffer pointer.
				memcpy(&sfcxState.dataReg, &sfcxState.pageBuffer[sfcxState.addressReg], 4);
				sfcxState.addressReg += 4;
				break;
			//case REG_TO_PAGE_BUF:
			//	break;
			//case LOG_PAGE_TO_BUF:
			//	break;
			case PHY_PAGE_TO_BUF:
				// Read Phyisical page into page buffer.
				// Physical pages are 0x210 bytes long, logical page (0x200) + meta data (0x10).
				fseek(nandFile, sfcxState.addressReg, SEEK_SET);
				fread_s(&sfcxState.pageBuffer, 0x210, 1, 0x210, nandFile);
				// Issue Interrupt.
				if (sfcxState.configReg & CONFIG_INT_EN)
				{
					// Set a delay for our interrupt?
					std::this_thread::sleep_for(std::chrono::milliseconds(150));
					parentBus->RouteInterrupt(PRIO_SFCX);
					sfcxState.statusReg |= STATUS_INT_CP;
				}
				break;
			//case WRITE_PAGE_TO_PHY:
			//	break;
			//case BLOCK_ERASE:
			//	break;
			//case DMA_LOG_TO_RAM:
			//	break;
			//case DMA_PHY_TO_RAM:
			//	break;
			//case DMA_RAM_TO_PHY:
			//	break;
			//case UNLOCK_CMD_0:
			//	break;
			//case UNLOCK_CMD_1:
			//	break;
			default:
				std::cout << "SFCX: Unrecognized command was issued. 0x" << sfcxState.commandReg << std::endl;
				break;
			}

			// Clear Command Register.
			sfcxState.commandReg = NO_CMD;

			// Set Status to Ready again.
			sfcxState.statusReg &= ~STATUS_BUSY;
		}
		// Sleep for some time.
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool SFCX::checkMagic()
{
	char magic[2];

	fread(&magic, 1, 2, nandFile);

	// Retail Nand Magic is 0xFF4F.
	// Devkit Nand Magic is 0x0F4F.
	// Older Devkit Nand's magic is 0x0F3F.

	if (magic[0] == (char)0xff && magic[1] == (char)0x4f)
	{
		std::cout << "SFCX: Retail NAND Magic found." << std::endl;
		return true;
	}
	if (magic[0] == (char)0x0f && magic[1] == (char)0x4f)
	{
		std::cout << "SFCX: Devkit NAND Magic found." << std::endl;
		return true;
	}
	if (magic[0] == (char)0x0f && magic[1] == (char)0x3f)
	{
		std::cout << "SFCX: Old Devkit NAND Magic found." << std::endl;
		return true;
	}
	return false;
}
