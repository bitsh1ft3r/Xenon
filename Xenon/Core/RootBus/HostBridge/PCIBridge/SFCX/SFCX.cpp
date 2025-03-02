// Copyright 2025 Xenon Emulator Project

#include "SFCX.h"

#include "Base/Logging/Log.h"
                                                                          
SFCX::SFCX(const char* deviceName, const std::string nandLoadPath, u64 size,
  PCIBridge *parentPCIBridge) : PCIDevice(deviceName, size) {
  // Asign parent PCI Bridge pointer.
  parentBus = parentPCIBridge;

  // Set PCI Properties.
  pciConfigSpace.configSpaceHeader.reg0.hexData = 0x580B1414;
  pciConfigSpace.configSpaceHeader.reg1.hexData = 0x02000006;
  pciConfigSpace.configSpaceHeader.reg2.hexData = 0x05010001;

  // Set our PCI Dev Sizes.
  pciDevSizes[0] = 0x400; // BAR0
  pciDevSizes[1] = 0x1000000; // BAR1

  LOG_INFO(SFCX, "Xenon Secure Flash Controller for Xbox.");

  // Set the registers as a dump from my Corona 16MB. These were dumped at POR
  // via Xell before SFCX Init. These are also readable via JRunner and
  // simple360 flasher.

  // Xenon Dev Kit ES DD2 64 MB
  // 0x01198030

  // Corona 16 Megs Retail
  // 0x000043000

  sfcxState.configReg = 0x00043000; // Config Reg is VERY Important. Tells info
                                    // about Meta/NAND Type.
  sfcxState.statusReg = 0x00000600;
  sfcxState.statusReg = 0x00000600;
  sfcxState.addressReg = 0x00f70030;
  sfcxState.logicalReg = 0x00000100;
  sfcxState.physicalReg = 0x0000100;
  sfcxState.commandReg = NO_CMD;

  // Load the NAND dump.
  LOG_INFO(SFCX, "Loading NAND from path: {}", nandLoadPath);

  fopen_s(&nandFile, nandLoadPath.c_str(), "rb");

  if (!nandFile) {
    LOG_CRITICAL(SFCX, "Fatal error, check your nand dump path!");
    SYSTEM_PAUSE();
  }

  // Check file magic.
  if (!checkMagic()) {
    LOG_CRITICAL(SFCX, "Fatal error, loaded faile magic does'nt correspond to Xbox 360 NAND.");
    SYSTEM_PAUSE();
  }

  // Load NAND header and display info about it.
  fseek(nandFile, 0, SEEK_SET);
  fread_s(&sfcxState.nandHeader, sizeof(NAND_HEADER), sizeof(NAND_HEADER), 1,
          nandFile);
  // Fix Endiannes
  sfcxState.nandHeader.nandMagic =
      std::byteswap<u16>(sfcxState.nandHeader.nandMagic);
  LOG_INFO(SFCX, " * NAND Magic: ", sfcxState.nandHeader.nandMagic);

  sfcxState.nandHeader.build = std::byteswap<u16>(sfcxState.nandHeader.build);
  LOG_INFO(SFCX, " * Build: ", sfcxState.nandHeader.build);

  sfcxState.nandHeader.qfe = std::byteswap<u16>(sfcxState.nandHeader.qfe);
  sfcxState.nandHeader.flags = std::byteswap<u16>(sfcxState.nandHeader.flags);

  sfcxState.nandHeader.entry = std::byteswap<u32>(sfcxState.nandHeader.entry);
  LOG_INFO(SFCX, " * Entry: ", sfcxState.nandHeader.entry);

  sfcxState.nandHeader.size = std::byteswap<u32>(sfcxState.nandHeader.size);
  LOG_INFO(SFCX, " * Size: ", sfcxState.nandHeader.size);

  sfcxState.nandHeader.keyvaultSize =
      std::byteswap<u32>(sfcxState.nandHeader.keyvaultSize);
  LOG_INFO(SFCX, " * Keyvault Size: ", sfcxState.nandHeader.keyvaultSize);

  sfcxState.nandHeader.sysUpdateAddr =
      std::byteswap<u32>(sfcxState.nandHeader.sysUpdateAddr);
  LOG_INFO(SFCX, " * System Update Addr: ", sfcxState.nandHeader.sysUpdateAddr);

  sfcxState.nandHeader.sysUpdateCount =
      std::byteswap<u16>(sfcxState.nandHeader.sysUpdateCount);
  LOG_INFO(SFCX, " * System Update Count: ", sfcxState.nandHeader.sysUpdateCount);

  sfcxState.nandHeader.keyvaultVer =
      std::byteswap<u16>(sfcxState.nandHeader.keyvaultVer);
  LOG_INFO(SFCX, " * Keyvault Ver: ", sfcxState.nandHeader.keyvaultVer);

  sfcxState.nandHeader.keyvaultAddr =
      std::byteswap<u32>(sfcxState.nandHeader.keyvaultAddr);
  LOG_INFO(SFCX, " * Keyvault Addr: ", sfcxState.nandHeader.keyvaultAddr);

  sfcxState.nandHeader.sysUpdateSize =
      std::byteswap<u32>(sfcxState.nandHeader.sysUpdateSize);
  LOG_INFO(SFCX, " * System Update Size: ", sfcxState.nandHeader.sysUpdateSize);

  sfcxState.nandHeader.smcConfigAddr =
      std::byteswap<u32>(sfcxState.nandHeader.smcConfigAddr);  
  LOG_INFO(SFCX, " * SMC Config Addr: ", sfcxState.nandHeader.smcConfigAddr);

  sfcxState.nandHeader.smcBootSize =
      std::byteswap<u32>(sfcxState.nandHeader.smcBootSize);
  LOG_INFO(SFCX, " * SMC Boot Size: ", sfcxState.nandHeader.smcBootSize);

  sfcxState.nandHeader.smcBootAddr =
      std::byteswap<u32>(sfcxState.nandHeader.smcBootAddr);
  LOG_INFO(SFCX, " * SMC Boot Addr: ", sfcxState.nandHeader.smcBootAddr);

  // Check Image size and Meta type.
  size_t imageSize = 0;
  fseek(nandFile, 0, SEEK_END);
  imageSize = ftell(nandFile);
  fseek(nandFile, 0, SEEK_SET);

  // There are two SFCX Versions, original (Pre Jasper) and Jasper+.

  // Enter SFCX Thread.
  sfcxThread = std::thread(&SFCX::sfcxMainLoop, this);
  sfcxThread.detach();
}

void SFCX::Read(u64 readAddress, u64 *data, u8 byteCount) {
  const u16 reg = readAddress & 0xFF;

  switch (reg) {
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
    LOG_ERROR(SFCX, "Read from unknown register {:#x}", reg);
    break;
  }
}

void SFCX::ConfigRead(u64 readAddress, u64 *data, u8 byteCount) {
  const u8 offset = readAddress & 0xFF;
  memcpy(data, &pciConfigSpace.data[offset], byteCount);
}

void SFCX::Write(u64 writeAddress, u64 data, u8 byteCount) {
  const u16 reg = writeAddress & 0xFF;

  switch (reg) {
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
    LOG_ERROR(SFCX, "Write from unknown register {:#x}", reg);
    break;
  }
}

void SFCX::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {
  const u8 offset = writeAddress & 0xFF;

  // Check if we're being scanned.
  if (static_cast<u8>(writeAddress) >= 0x10 && static_cast<u8>(writeAddress) < 0x34) {
    const u32 regOffset = (static_cast<u8>(writeAddress) - 0x10) >> 2;
    if (pciDevSizes[regOffset] != 0) {
      if (data == 0xFFFFFFFF) { // PCI BAR Size discovery.
        u64 x = 2;
        for (int idx = 2; idx < 31; idx++) {
          data &= ~x;
          x <<= 1;
          if (x >= pciDevSizes[regOffset]) {
            break;
          }
        }
        data &= ~0x3;
      }
    }
    if (static_cast<u8>(writeAddress) == 0x30) { // Expansion ROM Base Address.
      data = 0; // Register not implemented.
    }
  }

  memcpy(&pciConfigSpace.data[offset], &data, byteCount);
}

void SFCX::sfcxMainLoop() {
  // Config register should be initialized by now.
  while (true) {
    // Did we got a command?
    if (sfcxState.commandReg != NO_CMD) {
      // Set status to busy.
      sfcxState.statusReg |= STATUS_BUSY;

      // Check the command reg to see what command was issued.
      switch (sfcxState.commandReg) {
      case PAGE_BUF_TO_REG:
        // If we're reading from data buffer to data reg the Address reg becomes
        // our buffer pointer.
        memcpy(&sfcxState.dataReg, &sfcxState.pageBuffer[sfcxState.addressReg],
               4);
        sfcxState.addressReg += 4;
        break;
      // case REG_TO_PAGE_BUF:
      //	break;
      // case LOG_PAGE_TO_BUF:
      //	break;
      case PHY_PAGE_TO_BUF:
        // Read Phyisical page into page buffer.
        // Physical pages are 0x210 bytes long, logical page (0x200) + meta data
        // (0x10).
        fseek(nandFile, sfcxState.addressReg, SEEK_SET);
        fread_s(&sfcxState.pageBuffer, 0x210, 1, 0x210, nandFile);
        // Issue Interrupt.
        if (sfcxState.configReg & CONFIG_INT_EN) {
          // Set a delay for our interrupt?
          std::this_thread::sleep_for(std::chrono::milliseconds(150));
          parentBus->RouteInterrupt(PRIO_SFCX);
          sfcxState.statusReg |= STATUS_INT_CP;
        }
        break;
      // case WRITE_PAGE_TO_PHY:
      //	break;
      // case BLOCK_ERASE:
      //	break;
      // case DMA_LOG_TO_RAM:
      //	break;
      // case DMA_PHY_TO_RAM:
      //	break;
      // case DMA_RAM_TO_PHY:
      //	break;
      // case UNLOCK_CMD_0:
      //	break;
      // case UNLOCK_CMD_1:
      //	break;
      default:
        LOG_ERROR(SFCX, "Unrecognized command was issued. {:#x}", sfcxState.commandReg);
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

bool SFCX::checkMagic() {
  char magic[2];

  fread(&magic, 1, 2, nandFile);

  // Retail Nand Magic is 0xFF4F.
  // Devkit Nand Magic is 0x0F4F.
  // Older Devkit Nand's magic is 0x0F3F.

  if (magic[0] == (char)0xff && magic[1] == (char)0x4f) {
    LOG_INFO(SFCX, "Retail NAND Magic found.");
    return true;
  }
  if (magic[0] == (char)0x0f && magic[1] == (char)0x4f) {
    LOG_INFO(SFCX, "Devkit NAND Magic found.");
    return true;
  }
  if (magic[0] == (char)0x0f && magic[1] == (char)0x3f) {
    LOG_INFO(SFCX, "Old Devkit NAND Magic found.");
    return true;
  }
  return false;
}
