// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"

#include "Ethernet.h"

#define XE_NET_STATUS_INT 0x0000004C

Xe::PCIDev::ETHERNET::ETHERNET::ETHERNET() {
  // Set PCI Ven & Dev ID.
  u32 devID = 0x580A1414;
  memcpy(&pciConfigSpace.data[0], &devID, 4);
}

void Xe::PCIDev::ETHERNET::ETHERNET::Read(u64 readAddress, u64 *data,
                                          u8 byteCount) {
  u8 offset = readAddress & 0xFF;

  return; // For now.

  switch (offset) {
  case Xe::PCIDev::ETHERNET::TX_CONFIG:
    memcpy(data, &ethPciState.txConfigReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::TX_DESCRIPTOR_BASE:
    memcpy(data, &ethPciState.txDescriptorBaseReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::TX_DESCRIPTOR_STATUS:
    memcpy(data, &ethPciState.txDescriptorStatusReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::RX_CONFIG:
    memcpy(data, &ethPciState.rxConfigReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::RX_DESCRIPTOR_BASE:
    memcpy(data, &ethPciState.rxDescriptorBaseReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::INTERRUPT_STATUS:
    memcpy(data, &ethPciState.interruptStatusReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::INTERRUPT_MASK:
    memcpy(data, &ethPciState.interruptMaskReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::CONFIG_0:
    memcpy(data, &ethPciState.config0Reg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::POWER:
    memcpy(data, &ethPciState.powerReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::PHY_CONFIG:
    memcpy(data, &ethPciState.phyConfigReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::PHY_CONTROL:
    memcpy(data, &ethPciState.phyControlReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::CONFIG_1:
    memcpy(data, &ethPciState.config1Reg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::RETRY_COUNT:
    memcpy(data, &ethPciState.retryCountReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::MULTICAST_FILTER_CONTROL:
    memcpy(data, &ethPciState.multicastFilterControlReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::ADDRESS_0:
    memcpy(data, &ethPciState.address0Reg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::MULTICAST_HASH:
    memcpy(data, &ethPciState.multicastHashReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::MAX_PACKET_SIZE:
    memcpy(data, &ethPciState.maxPacketSizeReg, byteCount);
    break;
  case Xe::PCIDev::ETHERNET::ADDRESS_1:
    memcpy(data, &ethPciState.address1Reg, byteCount);
    break;
  default:
    LOG_ERROR(ETH, "Unknown PCI Reg being read 0x{}", (u16)offset);
    memset(data, 0xFF, byteCount);
    break;
  }
}

void Xe::PCIDev::ETHERNET::ETHERNET::ConfigRead(u64 readAddress, u64 *data,
                                                u8 byteCount) {
  memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void Xe::PCIDev::ETHERNET::ETHERNET::Write(u64 writeAddress, u64 data,
                                           u8 byteCount) {
  u8 offset = writeAddress & 0xFF;

  switch (offset) {
  case Xe::PCIDev::ETHERNET::TX_CONFIG:
    ethPciState.txConfigReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::TX_DESCRIPTOR_BASE:
    ethPciState.txDescriptorBaseReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::TX_DESCRIPTOR_STATUS:
    ethPciState.txDescriptorStatusReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::RX_CONFIG:
    ethPciState.rxConfigReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::RX_DESCRIPTOR_BASE:
    ethPciState.rxDescriptorBaseReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::INTERRUPT_STATUS:
    ethPciState.interruptStatusReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::INTERRUPT_MASK:
    ethPciState.interruptMaskReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::CONFIG_0:
    ethPciState.config0Reg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::POWER:
    ethPciState.powerReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::PHY_CONFIG:
    ethPciState.phyConfigReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::PHY_CONTROL:
    ethPciState.phyControlReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::CONFIG_1:
    ethPciState.config1Reg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::RETRY_COUNT:
    ethPciState.retryCountReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::MULTICAST_FILTER_CONTROL:
    ethPciState.multicastFilterControlReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::ADDRESS_0:
    ethPciState.address0Reg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::MULTICAST_HASH:
    ethPciState.multicastHashReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::MAX_PACKET_SIZE:
    ethPciState.maxPacketSizeReg = static_cast<u32>(data);
    break;
  case Xe::PCIDev::ETHERNET::ADDRESS_1:
    ethPciState.address1Reg = static_cast<u32>(data);
    break;
  default:
    LOG_ERROR(ETH, "Unknown PCI Reg being written 0x{} data = 0x{}", (u16)offset, data);
    break;
  }
}

void Xe::PCIDev::ETHERNET::ETHERNET::ConfigWrite(u64 writeAddress, u64 data,
                                                 u8 byteCount) {
  memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
