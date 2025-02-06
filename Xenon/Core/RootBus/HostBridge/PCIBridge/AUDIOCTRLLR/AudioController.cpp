// Copyright 2025 Xenon Emulator Project

#include "AudioController.h"

Xe::PCIDev::AUDIOCTRLR::AUDIOCTRLR::AUDIOCTRLR(const char *deviceName, u64 size) : PCIDevice(deviceName, size) {
  // Set PCI Ven & Dev ID.
  u32 devID = 0x580C1414;
  memcpy(&pciConfigSpace.data[0], &devID, 4);
}

void Xe::PCIDev::AUDIOCTRLR::AUDIOCTRLR::Read(u64 readAddress, u64 *data,
                                              u8 byteCount) {}

void Xe::PCIDev::AUDIOCTRLR::AUDIOCTRLR::ConfigRead(u64 readAddress, u64 *data,
                                                    u8 byteCount) {
  memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void Xe::PCIDev::AUDIOCTRLR::AUDIOCTRLR::Write(u64 writeAddress, u64 data,
                                               u8 byteCount) {}

void Xe::PCIDev::AUDIOCTRLR::AUDIOCTRLR::ConfigWrite(u64 writeAddress, u64 data,
                                                     u8 byteCount) {
  memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
