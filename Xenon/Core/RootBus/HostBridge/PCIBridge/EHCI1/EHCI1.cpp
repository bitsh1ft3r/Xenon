// Copyright 2025 Xenon Emulator Project

#include "EHCI1.h"

Xe::PCIDev::EHCI1::EHCI1::EHCI1(const char *deviceName, u64 size) : PCIDevice(deviceName, size) {
  // Set PCI Properties.
  pciConfigSpace.configSpaceHeader.reg0.hexData = 0x58071414;
  pciConfigSpace.configSpaceHeader.reg1.hexData = 0x02900106;
  pciConfigSpace.configSpaceHeader.reg2.hexData = 0x0c032001;
  // Set our PCI Dev Sizes.
  pciDevSizes[0] = 0x1000; // BAR0
}

void Xe::PCIDev::EHCI1::EHCI1::Read(u64 readAddress, u64 *data, u8 byteCount) {}

void Xe::PCIDev::EHCI1::EHCI1::ConfigRead(u64 readAddress, u64 *data,
                                          u8 byteCount) {
  memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void Xe::PCIDev::EHCI1::EHCI1::Write(u64 writeAddress, u64 data, u8 byteCount) {
}

void Xe::PCIDev::EHCI1::EHCI1::ConfigWrite(u64 writeAddress, u64 data,
                                           u8 byteCount) {
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

  memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
