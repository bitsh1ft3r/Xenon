// Copyright 2025 Xenon Emulator Project

#include "XGPU.h"

#include "XGPUConfig.h"
#include "XenosRegisters.h"

#include "Core/Xe_Main.h"

#include "Base/Config.h"
#include "Base/Path_util.h"
#include "Base/Version.h"
#include "Base/Logging/Log.h"

#define XE_DEBUG

Xe::Xenos::XGPU::XGPU(RAM *ram) {
  // Assign RAM Pointer
  ramPtr = ram;

  memset(&xgpuConfigSpace.data, 0xf, sizeof(GENRAL_PCI_DEVICE_CONFIG_SPACE));
  // Setup config space as per dump taken from a Jasper console.
  // Located at config address 0xD0010000.
  u8 i = 0;
  for (u16 idx = 0; idx < 256; idx += 4) {
    memcpy(&xgpuConfigSpace.data[idx], &xgpuConfigMap[i], 4);
    i++;
  }
  // Set our PCI Dev Sizes.
  pciDevSizes[0] = 0x20000; // BAR0

  xenosState.Regs = new u8[0xFFFFF];
  memset(xenosState.Regs, 0, 0xFFFFF);

  // Set Clocks speeds.
  u32 reg = 0x09000000;
  memcpy(&xenosState.Regs[REG_GPU_CLK], &reg, 4);
  reg = 0x11000c00;
  memcpy(&xenosState.Regs[REG_EDRAM_CLK], &reg, 4);
  reg = 0x1a000001;
  memcpy(&xenosState.Regs[REG_FSB_CLK], &reg, 4);
  reg = 0x19100000;
  memcpy(&xenosState.Regs[REG_MEM_CLK], &reg, 4);
}

bool Xe::Xenos::XGPU::Read(u64 readAddress, u64 *data, u8 byteCount) {
  std::lock_guard lck(mutex);
  if (isAddressMappedInBAR(static_cast<u32>(readAddress))) {
    const u32 regIndex = (readAddress & 0xFFFFF) / 4;

#ifdef XE_DEBUG
    LOG_DEBUG(Xenos, "Read to {}, index {:#x}", GetRegisterNameById(regIndex), regIndex);
#endif

    LOG_TRACE(Xenos, "Read Addr = {:#x}, reg: {:#x}.", readAddress, regIndex);

    XeRegister reg = static_cast<XeRegister>(regIndex);

    u32 regData = 0;
    memcpy(&regData, &xenosState.Regs[regIndex * 4], 4);

    // Switch for properly return the requested amount of data.
    switch (byteCount) {
      case 2:
        regData = regData >> 16;
        break;
      case 1:
        regData = regData >> 24;
        break;
      default:
        break;
    }

    *data = regData;

    if (regIndex == 0x00000a07)
      *data = 0x2000000;

    if (regIndex == 0x00001928)
      *data = 0x2000000;

    if (regIndex == 0x00001e54)
      *data = 0;

    return true;
  }

  return false;
}

bool Xe::Xenos::XGPU::Write(u64 writeAddress, u64 data, u8 byteCount) {
  std::lock_guard lck(mutex);
  if (isAddressMappedInBAR(static_cast<u32>(writeAddress))) {
    const u32 regIndex = (writeAddress & 0xFFFFF) / 4;

#ifdef XE_DEBUG
    LOG_DEBUG(Xenos, "Write to {}, index {:#x}, data = {:#x}", GetRegisterNameById(regIndex), regIndex,
      std::byteswap<u32>(static_cast<u32>(data)));
#endif

    LOG_TRACE(Xenos, "Write Addr = {:#x}, reg: {:#x}, data = {:#x}.", writeAddress, regIndex,
      std::byteswap<u32>(static_cast<u32>(data)));

    XeRegister reg = static_cast<XeRegister>(regIndex);

    // Set our internal width.
    if (reg == XeRegister::D1GRPH_X_END) {
      Xe_Main->renderer->internalWidth = std::byteswap<u32>(static_cast<u32>(data));
      LOG_INFO(Xenos, "Setting new Internal Width: {:#x}", Xe_Main->renderer->internalWidth);
    }
    // Set our internal height.
    if (reg == XeRegister::D1GRPH_Y_END) {
      Xe_Main->renderer->internalHeight = std::byteswap<u32>(static_cast<u32>(data));
      LOG_INFO(Xenos, "Setting new Internal Height: {:#x}", Xe_Main->renderer->internalHeight);
    }

    memcpy(&xenosState.Regs[regIndex * 4], &data, byteCount);
    return true;
  }

  return false;
}

void Xe::Xenos::XGPU::ConfigRead(u64 readAddress, u64* data, u8 byteCount) {
  std::lock_guard lck(mutex);
  memcpy(data, &xgpuConfigSpace.data[readAddress & 0xFF], byteCount);
  return;
}

void Xe::Xenos::XGPU::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {
  std::lock_guard lck(mutex);
  // Check if we're being scanned.
  if (static_cast<u8>(writeAddress) >= 0x10 && static_cast<u8>(writeAddress) < 0x34) {
    const u32 regOffset = (static_cast<u8>(writeAddress) - 0x10) >> 2;
    if (pciDevSizes[regOffset] != 0)
    {
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

  memcpy(&xgpuConfigSpace.data[writeAddress & 0xFF], &data, byteCount);
  return;
}

bool Xe::Xenos::XGPU::isAddressMappedInBAR(u32 address) {
  #define ADDRESS_BOUNDS_CHECK(a, b) (address >= a && address <= (a + b))
  if (ADDRESS_BOUNDS_CHECK(xgpuConfigSpace.configSpaceHeader.BAR0, XGPU_DEVICE_SIZE) ||
    ADDRESS_BOUNDS_CHECK(xgpuConfigSpace.configSpaceHeader.BAR1, XGPU_DEVICE_SIZE) ||
    ADDRESS_BOUNDS_CHECK(xgpuConfigSpace.configSpaceHeader.BAR2, XGPU_DEVICE_SIZE) ||
    ADDRESS_BOUNDS_CHECK(xgpuConfigSpace.configSpaceHeader.BAR3, XGPU_DEVICE_SIZE) ||
    ADDRESS_BOUNDS_CHECK(xgpuConfigSpace.configSpaceHeader.BAR4, XGPU_DEVICE_SIZE) ||
    ADDRESS_BOUNDS_CHECK(xgpuConfigSpace.configSpaceHeader.BAR5, XGPU_DEVICE_SIZE))
  {
    return true;
  }

  return false;
}
