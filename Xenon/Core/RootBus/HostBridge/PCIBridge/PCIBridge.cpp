// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"

#include "PCIBridge.h"
#include "PCIBridgeConfig.h"

// Device tree, taken from an actual system, Corona V6.
/*
        BUS0
                -Dev 0, Func 0: Microsoft Corp. XMA Decoder
                -Dev 1, Func 0: Microsoft Corp. SATA Controller - CDROM
                -Dev 2, Func 0: Microsoft Corp. SATA Controller - HDD
                -Dev 4, Func 0: Microsoft Corp. OHCI Controller 0
                -Dev 4, Func 1: Microsoft Corp. EHCI Controller 0
                -Dev 5, Func 0: Microsoft Corp. OHCI Controller 1
                -Dev 5, Func 1: Microsoft Corp. EHCI Controller 1
                -Dev 7, Func 0: Microsoft Corp. Fast Ethernet Adapter
                -Dev 8, Func 0: Microsoft Corp. Secure Flash Controller
                -Dev 9, Func 0: Microsoft Corp. 580C
                -Dev A, Func 0: Microsoft Corp. System Management Controller
                -Dev F, Func 0: Microsoft Corp. 5841
*/

#define XMA_DEV_NUM 0x0
#define CDROM_DEV_NUM 0x1
#define HDD_DEV_NUM 0x2
#define OHCI0_DEV_NUM 0x4
#define EHCI0_DEV_NUM 0x4
#define OHCI1_DEV_NUM 0x5
#define EHCI1_DEV_NUM 0x5
#define FAST_ETH_DEV_NUM 0x7
#define SFC_DEV_NUM 0x8
#define AUDIO_CTRLR_DEV_NUM 0x9
#define SMC_DEV_NUM 0xA
#define _5841_DEV_NUM 0xF

PCIBridge::PCIBridge() {
  // Set config registers.
  u8 i = 0;
  for (u16 idx = 0; idx < 256; idx += 4) {
    memcpy(&pciBridgeConfigSpace[idx], &pciBridgeConfigMap[i], 4);
    i++;
  }

  pciBridgeConfig.configSpaceHeader.reg0.hexData = pciBridgeConfigMap[0];
  pciBridgeConfig.configSpaceHeader.reg1.hexData = pciBridgeConfigMap[1];
  pciBridgeConfig.configSpaceHeader.reg2.hexData = pciBridgeConfigMap[2];
  pciBridgeConfig.configSpaceHeader.reg3.hexData = pciBridgeConfigMap[3];
  pciBridgeConfig.configSpaceHeader.BAR0 = pciBridgeConfigMap[4];
  pciBridgeConfig.configSpaceHeader.BAR1 = pciBridgeConfigMap[5];
  pciBridgeConfig.configSpaceHeader.reg6.hexData = pciBridgeConfigMap[6];
  pciBridgeConfig.configSpaceHeader.reg7.hexData = pciBridgeConfigMap[7];
  pciBridgeConfig.configSpaceHeader.reg8.hexData = pciBridgeConfigMap[8];
  pciBridgeConfig.configSpaceHeader.reg9.hexData = pciBridgeConfigMap[9];
  pciBridgeConfig.configSpaceHeader.prefetchableBaseUpper32Bits =
      pciBridgeConfigMap[0xA];
  pciBridgeConfig.configSpaceHeader.prefetchableLimitUpper32Bits =
      pciBridgeConfigMap[0xB];
  pciBridgeConfig.configSpaceHeader.regC.hexData = pciBridgeConfigMap[0xC];
  pciBridgeConfig.configSpaceHeader.regD.hexData = pciBridgeConfigMap[0xD];
  pciBridgeConfig.configSpaceHeader.expansionROMBaseAddress =
      pciBridgeConfigMap[0xE];
  pciBridgeConfig.configSpaceHeader.regF.hexData = pciBridgeConfigMap[0xF];

  // PCI Bridge Config regs
  pciBridgeState.REG_EA00000C =
      0x7cff; // Software writes here to enable interrupts (Bus IRQL).
}

void PCIBridge::RegisterIIC(Xe::XCPU::IIC::XenonIIC *xenonIICPtr) {
  xenonIIC = xenonIICPtr;
}

bool PCIBridge::RouteInterrupt(u8 prio) {
  u64 intPacket = 0;
  u32 address = 0;

  switch (prio) {
  case PRIO_CLOCK:
    if (pciBridgeState.PRIO_REG_CLCK.intEnabled) {
      xenonIIC->genInterrupt(PRIO_CLOCK,
                             pciBridgeState.PRIO_REG_CLCK.targetCPU);
    }
    break;
  case PRIO_SMM:
    if (pciBridgeState.PRIO_REG_SMM.intEnabled) {
      xenonIIC->genInterrupt(PRIO_SMM, pciBridgeState.PRIO_REG_SMM.targetCPU);
    }
    break;
  case PRIO_SFCX:
    if (pciBridgeState.PRIO_REG_SFCX.intEnabled) {
      xenonIIC->genInterrupt(PRIO_SFCX, pciBridgeState.PRIO_REG_SFCX.targetCPU);
    }
    break;
  default:
      LOG_ERROR(PCIBridge, "Unknown interrupt being routed: {:#x}", prio);
    break;
  }
  return false;
}

bool PCIBridge::isAddressMappedinBAR(u32 address) {
  u32 bar0 = pciBridgeConfig.configSpaceHeader.BAR0;
  u32 bar1 = pciBridgeConfig.configSpaceHeader.BAR1;

  if (address >= bar0 && address <= bar0 + PCI_BRIDGE_SIZE ||
      address >= bar1 && address <= bar1 + PCI_BRIDGE_SIZE) {
    return true;
  }

  return false;
}

void PCIBridge::addPCIDevice(PCIDevice *device) {
  LOG_INFO(PCIBridge, "Attatched: {}", device->GetDeviceName());

  connectedPCIDevices.push_back(device);
}

bool PCIBridge::Read(u64 readAddress, u64 *data, u8 byteCount) {
  // Reading to our own space?
  if (readAddress >= PCI_BRIDGE_BASE_ADDRESS &&
      readAddress <= PCI_BRIDGE_BASE_END_ADDRESS) {
    switch (readAddress) {
    case 0xEA000000:
      *data = pciBridgeState.REG_EA000000;
      break;
    case 0xEA000004:
      *data = pciBridgeState.REG_EA000004;
      break;
    case 0xEA00000C:
      *data = pciBridgeState.REG_EA00000C;
      break;
    case 0xEA000010:
      *data = pciBridgeState.PRIO_REG_CLCK.hexData;
      break;
    case 0xEA00001C:
      *data = pciBridgeState.PRIO_REG_SMM.hexData;
      break;
    case 0xEA000044:
      *data = pciBridgeState.PRIO_REG_SFCX.hexData;
      break;
    default:
      LOG_ERROR(PCIBridge, "Unknown reg being read: {:#x}", readAddress);
      break;
    }
    return true;
  }

  // Try writing to one of the attatched devices.
  for (auto &device : connectedPCIDevices) {
    if (device->isAddressMappedInBAR(static_cast<u32>(readAddress))) {
      // Hit
      device->Read(readAddress, data, byteCount);
      return true;
    }
  }
  *data = 0xFFFFFFFFFFFFFFFF;
  return false;
}

bool PCIBridge::Write(u64 writeAddress, u64 data, u8 byteCount) {
  bool enabled = (data & 0x00800000) >> 20;
  bool latched = (data & 0x00200000) >> 20;
  u8 targetCPU = (data & 0x00003F00) >> 8;
  u8 cpuIRQ = (data & 0x0000003F) << 2;

  // Writing to our own space?
  if (writeAddress >= PCI_BRIDGE_BASE_ADDRESS &&
      writeAddress <= PCI_BRIDGE_BASE_END_ADDRESS) {
    switch (writeAddress) {
    case 0xEA000000:
      pciBridgeState.REG_EA000000 = static_cast<u32>(data);
      break;
    case 0xEA000004:
      pciBridgeState.REG_EA000004 = static_cast<u32>(data);
      break;
    case 0xEA00000C:
      pciBridgeState.REG_EA00000C = static_cast<u32>(data);
      break;
    case 0xEA000010: // PRIO_CLOCK
      pciBridgeState.PRIO_REG_CLCK.hexData = static_cast<u32>(data);
      pciBridgeState.PRIO_REG_CLCK.intEnabled = enabled;
      pciBridgeState.PRIO_REG_CLCK.latched = latched;
      pciBridgeState.PRIO_REG_CLCK.targetCPU = targetCPU;
      pciBridgeState.PRIO_REG_CLCK.cpuIRQ = cpuIRQ;
      break;
    case 0xEA00001C: // PRIO_SMM System Management Mode Int. Used by SMC.
      pciBridgeState.PRIO_REG_SMM.hexData = static_cast<u32>(data);
      pciBridgeState.PRIO_REG_SMM.intEnabled = enabled;
      pciBridgeState.PRIO_REG_SMM.latched = latched;
      pciBridgeState.PRIO_REG_SMM.targetCPU = targetCPU;
      pciBridgeState.PRIO_REG_SMM.cpuIRQ = cpuIRQ;
      break;
    case 0xEA000044: // PRIO_SFCX Secure Flash Controller for Xbox Int.
      pciBridgeState.PRIO_REG_SFCX.hexData = static_cast<u32>(data);
      pciBridgeState.PRIO_REG_SFCX.intEnabled = enabled;
      pciBridgeState.PRIO_REG_SFCX.latched = latched;
      pciBridgeState.PRIO_REG_SFCX.targetCPU = targetCPU;
      pciBridgeState.PRIO_REG_SFCX.cpuIRQ = cpuIRQ;
      break;
    default:
      LOG_ERROR(PCIBridge, "Unknown reg being written: {:#x}, {:#x}", writeAddress, data);
      break;
    }
    return true;
  }

  // Try writing to one of the attatched devices.
  for (auto &device : connectedPCIDevices) {
    if (device->isAddressMappedInBAR(static_cast<u32>(writeAddress))) {
      // Hit
      device->Write(writeAddress, data, byteCount);
      return true;
    }
  }
  return false;
}

void PCIBridge::ConfigRead(u64 readAddress, u64 *data, u8 byteCount) {
  PCIE_CONFIG_ADDR configAddr = {};
  configAddr.hexData = static_cast<u32>(readAddress);

  if (configAddr.busNum == 0 && configAddr.devNum == 0) {
    // Reading from our own config space!
    memcpy(data, &pciBridgeConfig.data[configAddr.regOffset], byteCount);
    return;
  }

  // Current device Name
  std::string currentDevName = "";

  switch (configAddr.devNum) {
  case XMA_DEV_NUM:
    currentDevName = "XMA";
    break;
  case CDROM_DEV_NUM:
    currentDevName = "CDROM";
    break;
  case HDD_DEV_NUM:
    currentDevName = "HDD";
    break;
  case OHCI0_DEV_NUM:
    if (configAddr.functNum == 0) {
      currentDevName = "OHCI0";
    } else if (configAddr.functNum == 1) {
      currentDevName = "EHCI0";
    }
    break;
  case OHCI1_DEV_NUM:
    if (configAddr.functNum == 0) {
      currentDevName = "OHCI1";
    } else if (configAddr.functNum == 1) {
      currentDevName = "EHCI1";
    }
    break;
  case FAST_ETH_DEV_NUM:
    currentDevName = "ETHERNET";
    break;
  case SFC_DEV_NUM:
    currentDevName = "SFCX";
    break;
  case AUDIO_CTRLR_DEV_NUM:
    currentDevName = "AUDIOCTRLR";
    break;
  case SMC_DEV_NUM:
    currentDevName = "SMC";
    break;
  case _5841_DEV_NUM:
    currentDevName = "5841";
    break;
  default:
    LOG_ERROR(PCIBridge, "Config Space Read: Unknown device accessed: Dev {:#x}, Reg{:#x}",
        configAddr.devNum, configAddr.regOffset);
    return;
    break;
  }

  for (auto &device : connectedPCIDevices) {
    if (device->GetDeviceName() == currentDevName) {
      // Hit!
      LOG_TRACE(PCIBridge, "Config read, device: {} offset = {:#x}", currentDevName, configAddr.regOffset);
      device->ConfigRead(readAddress, data, byteCount);
      return;
    }
  }
  LOG_ERROR(PCIBridge, "Read to unimplemented device: {}", currentDevName.c_str());
  *data = 0xFFFFFFFFFFFFFFFF;
}

void PCIBridge::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {
  PCIE_CONFIG_ADDR configAddr = {};
  configAddr.hexData = static_cast<u32>(writeAddress);

  if (configAddr.busNum == 0 && configAddr.devNum == 0) {
    // Writing to our own config space!
    memcpy(&pciBridgeConfig.data[configAddr.regOffset], &data, byteCount);
    return;
  }

  // Current device Name
  std::string currentDevName = "";

  switch (configAddr.devNum) {
  case XMA_DEV_NUM:
    currentDevName = "XMA";
    break;
  case CDROM_DEV_NUM:
    currentDevName = "CDROM";
    break;
  case HDD_DEV_NUM:
    currentDevName = "HDD";
    break;
  case OHCI0_DEV_NUM:
    if (configAddr.functNum == 0) {
      currentDevName = "OHCI0";
    } else if (configAddr.functNum == 1) {
      currentDevName = "EHCI0";
    }
    break;
  case OHCI1_DEV_NUM:
    if (configAddr.functNum == 0) {
      currentDevName = "OHCI1";
    } else if (configAddr.functNum == 1) {
      currentDevName = "EHCI1";
    }
    break;
  case FAST_ETH_DEV_NUM:
    currentDevName = "ETHERNET";
    break;
  case SFC_DEV_NUM:
    currentDevName = "SFCX";
    break;
  case AUDIO_CTRLR_DEV_NUM:
    currentDevName = "AUDIOCTRLR";
    break;
  case SMC_DEV_NUM:
    currentDevName = "SMC";
    break;
  case _5841_DEV_NUM:
    currentDevName = "5841";
    break;
  default:
    LOG_ERROR(PCIBridge, "Config Space Write: Unknown device accessed: Dev {:#x} Func {:#x}"
        "Reg {:#x} data = {:#x}", configAddr.devNum, configAddr.functNum, configAddr.regOffset, data);
    return;
    break;
  }

  for (auto &device : connectedPCIDevices) {
    if (device->GetDeviceName() == currentDevName) {
      // Hit!
      LOG_TRACE(PCIBridge, "Config write, device: {}, offset = {:#x} data = {:#x}", currentDevName.c_str(), configAddr.regOffset, data);
      device->ConfigWrite(writeAddress, data, byteCount);
      return;
    }
  }
  LOG_ERROR(PCIBridge, "Write to unimplemented device: {} data = {:#x}", currentDevName.c_str(), data);
}
