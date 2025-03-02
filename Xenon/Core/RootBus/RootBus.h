// Copyright 2025 Xenon Emulator Project

#pragma once

#include "Base/SystemDevice.h"
#include "Core/RootBus/HostBridge/HostBridge.h"

// PCI Configuration region
#define PCI_CONFIG_REGION_ADDRESS 0xD0000000
#define PCI_CONFIG_REGION_SIZE 0x1000000

class RootBus {
public:
  RootBus();
  ~RootBus();

  void AddHostBridge(HostBridge *newHostBridge);

  void AddDevice(SystemDevice *device);

  void Read(u64 readAddress, u64 *data, u8 byteCount);
  void Write(u64 writeAddress, u64 data, u8 byteCount);

  // Configuration Space R/W
  void ConfigRead(u64 readAddress, u64 *data, u8 byteCount);
  void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount);

private:
  HostBridge *hostBridge{};
  u32 deviceCount;
  std::vector<SystemDevice*> conectedDevices;

  std::unique_ptr<u8> biuData{ std::make_unique<STRIP_UNIQUE(biuData)>(0x10000) };
};
