// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"

#include "HDD.h"

HDD::HDD(PCIBridge *parentPCIBridge) {
  // TODO(bitsh1ft3r): Implement PCIe Capabilities.
  // Set PCI Properties.
  pciConfigSpace.configSpaceHeader.reg0.hexData = 0x58031414;
  pciConfigSpace.configSpaceHeader.reg1.hexData = 0x02300006;
  pciConfigSpace.configSpaceHeader.reg2.hexData = 0x01060000;
  // Set our PCI Dev Sizes.
  pciDevSizes[0] = 0x20; // BAR0
  pciDevSizes[1] = 0x10; // BAR1

  // Assign our PCI Bridge pointer
  parentBus = parentPCIBridge;

  // Device ready to receive commands.
  ataDeviceState.ataReadState.status = ATA_STATUS_DRDY;
}

void HDD::Read(u64 readAddress, u64 *data, u8 byteCount) {
  u32 regOffset = (readAddress & 0xFF) * 4;

  if (regOffset < sizeof(ATA_REG_STATE)) {
    switch (regOffset) {
    case ATA_REG_DATA:
      if (!ataDeviceState.readBuffer.empty()) {
        // First clear the DATA Request.
        if (ataDeviceState.ataReadState.status & ATA_STATUS_DRQ) {
          ataDeviceState.ataReadState.status &= ~ATA_STATUS_DRQ;
        }
        // We have some data to send out.
        // Make sure we send the right amount of data.
        byteCount = (byteCount < ataDeviceState.readBuffer.capacity()
                         ? byteCount
                         : ataDeviceState.readBuffer.capacity());
        // Do the actual copy of data.
        memcpy(data, ataDeviceState.readBuffer.data(), byteCount);
        // Remove the readed bytes of the conatiner.
        ataDeviceState.readBuffer.erase(ataDeviceState.readBuffer.begin(),
                                        ataDeviceState.readBuffer.begin() +
                                            byteCount);
        return;
      }
      break;
    default:
      break;
    }

    memcpy(data, (u8 *)&ataDeviceState.ataReadState + regOffset, byteCount);
  } else {
    LOG_ERROR(HDD, "Unknown register being accesed: (Read) {:#x}", regOffset);
    memset(data, 0, byteCount);
  }
}

void HDD::Write(u64 writeAddress, u64 data, u8 byteCount) {
  u32 regOffset = (writeAddress & 0xFF) * 4;

  u32 value = 0;

  if (regOffset < sizeof(ATA_REG_STATE)) {
    memcpy(&value, &data, byteCount);

    switch (regOffset) {
    case ATA_REG_CMD_STATUS:
      switch (data) {
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
        // Copy the device indetification data to our read buffer.
        ataCopyIdentifyDeviceData();
        // Set data ready flag.
        ataDeviceState.ataReadState.status |= ATA_STATUS_DRQ;
        // Raise an interrupt.
        parentBus->RouteInterrupt(PRIO_SATA_HDD);
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

    memcpy((u8 *)&ataDeviceState.ataWriteState + regOffset, &data, byteCount);
  } else {
    LOG_ERROR(HDD, "Unknown register being accesed: (Write) {:#x}", regOffset);
  }
}

void HDD::ConfigRead(u64 readAddress, u64 *data, u8 byteCount) {
  memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void HDD::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {
    // Check if we're being scanned.
    if (static_cast<u8>(writeAddress) >= 0x10 && static_cast<u8>(writeAddress) < 0x34) {
        u32 regOffset = (static_cast<u8>(writeAddress) - 0x10) >> 2;
        if (pciDevSizes[regOffset] != 0) {
            if (data == 0xFFFFFFFF) { // PCI BAR Size discovery.
                u32 x = 2;
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
  memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}

void HDD::ataCopyIdentifyDeviceData() {
  if (!ataDeviceState.readBuffer.empty())
    LOG_ERROR(HDD, "Read Buffer not empty!");

  ataDeviceState.readBuffer.resize(sizeof(ATA_IDENTIFY_DATA));

  for (size_t buffPos = 0; buffPos <= sizeof(ATA_IDENTIFY_DATA); buffPos++) {
    memcpy(ataDeviceState.readBuffer.data() + buffPos,
           (u8 *)&ataDeviceState.ataIdentifyData + buffPos, 1);
  }
}
