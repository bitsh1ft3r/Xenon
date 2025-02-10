// Copyright 2025 Xenon Emulator Project

#include "XMA.h"

XMA::XMA() {
  // Set PCI Properties.
  pciConfigSpace.configSpaceHeader.reg0.hexData = 0x58011414;
  pciConfigSpace.configSpaceHeader.reg1.hexData = 0x02000002;

  // Set our PCI Dev Sizes.
  pciDevSizes[0] = 0x400; // BAR0
}

void XMA::Read(u64 readAddress, u64 *data, u8 byteCount) {}

void XMA::Write(u64 writeAddress, u64 data, u8 byteCount) {}

void XMA::ConfigRead(u64 readAddress, u64 *data, u8 byteCount) {
  memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void XMA::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {
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
