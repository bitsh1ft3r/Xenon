#include <iostream>

#include "Xenon/Core/RootBus/HostBridge/XGPU/XGPU.h"
#include "Xenon/Core/RootBus/HostBridge/XGPU/XGPUConfig.h"

Xe::Xenos::XGPU::XGPU()
{
	memset(&xgpuConfigSpace.data, 0xf, sizeof(GENRAL_PCI_DEVICE_CONFIG_SPACE));
	// Setup config space as per dump taken from a Jasper console.
	// Located at config address 0xD0010000.
	u8 i = 0;
	for (u16 idx = 0; idx < 256; idx += 4)
	{
		memcpy(&xgpuConfigSpace.data[idx], &xgpuConfigMap[i], 4);
		i++;
	}

	xenosState.Regs = new u8[0xFFFFF];
	memset(xenosState.Regs, 0, 0xFFFFF);

	// Set Clocks speeds.
	u32 reg = 0x09000000;
	memcpy(&xenosState.Regs[REG_GPU_CLK],&reg,4);
	reg = 0x11000c00;
	memcpy(&xenosState.Regs[REG_EDRAM_CLK], &reg, 4);
	reg = 0x1a000001;
	memcpy(&xenosState.Regs[REG_FSB_CLK], &reg, 4);
	reg = 0x19100000;
	memcpy(&xenosState.Regs[REG_MEM_CLK], &reg, 4);
	
}

bool Xe::Xenos::XGPU::Read(u64 readAddress, u64* data, u8 byteCount)
{
	if (isAddressMappedInBAR(static_cast<u32>(readAddress)))
	{
		std::cout << "Xenos Read Addr = 0x" << readAddress << std::endl;
		u32 index = readAddress & 0xFFFFF;
		memcpy(data, &xenosState.Regs[index], byteCount);
		return true;
	}

	return false;
}

bool Xe::Xenos::XGPU::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	if (isAddressMappedInBAR(static_cast<u32>(writeAddress)))
	{
		std::cout << "Xenos Write Addr = 0x" << writeAddress << " data = 0x" << data << std::endl;
		u32 index = writeAddress & 0xFFFFF;
		memcpy(&xenosState.Regs[index], &data, byteCount);
		return true;
	}

	return false;
}

void Xe::Xenos::XGPU::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &xgpuConfigSpace.data[readAddress & 0xFF], byteCount);
	return;
}

void Xe::Xenos::XGPU::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&xgpuConfigSpace.data[writeAddress & 0xFF], &data, byteCount);
	return;
}

bool Xe::Xenos::XGPU::isAddressMappedInBAR(u32 address)
{
	u32 bar0 = xgpuConfigSpace.configSpaceHeader.BAR0;
	u32 bar1 = xgpuConfigSpace.configSpaceHeader.BAR1;
	u32 bar2 = xgpuConfigSpace.configSpaceHeader.BAR2;
	u32 bar3 = xgpuConfigSpace.configSpaceHeader.BAR3;
	u32 bar4 = xgpuConfigSpace.configSpaceHeader.BAR4;
	u32 bar5 = xgpuConfigSpace.configSpaceHeader.BAR5;

	if (address >= bar0 && address <= bar0 + XGPU_DEVICE_SIZE
		|| address >= bar1 && address <= bar1 + XGPU_DEVICE_SIZE
		|| address >= bar2 && address <= bar2 + XGPU_DEVICE_SIZE
		|| address >= bar3 && address <= bar3 + XGPU_DEVICE_SIZE
		|| address >= bar4 && address <= bar4 + XGPU_DEVICE_SIZE
		|| address >= bar5 && address <= bar5 + XGPU_DEVICE_SIZE)
	{
		return true;
	}

	return false;
}
