// Copyright 2025 Xenon Emulator Project

#pragma once

#include "Base/SystemDevice.h"
#include <vector>

#define RAM_START_ADDR 0
#define RAM_SIZE 0x20000000

class RAM : public SystemDevice {
public:
  RAM(const char* deviceName, u64 startAddress, u64 endAddress,
    bool isSOCDevice);
  ~RAM();
  void Read(u64 readAddress, u64 *data, u8 byteCount) override;
  void Write(u64 writeAddress, u64 data, u8 byteCount) override;

  u8 *getPointerToAddress(u32 address);

private:
  std::vector<u8> RAMData;
};
