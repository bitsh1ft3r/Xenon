#pragma once

#include <vector>

#include "Xenon/Base/SystemDevice.h"
#include "Xenon/Core/PCIBus/PCIBridge.h"

class Bus
{
public:
	void Init();

    void AddPCIBridge(PCIBridge* newPCIBridge);
	void AddDevice(SystemDevice* device);
	void Read(u64 readAddress, u64* data, u8 byteCount, bool SOC = false);
	void Write(u64 writeAddress, u64 data, u8 byteCount, bool SOC = false);
private:
	PCIBridge* _PCIBridge;
	u32 deviceCount;
	std::vector<SystemDevice*> conectedDevices;

	u8* biu0Data = new u8[0x50000];
	u8* biuData = new u8[0x10000];
};