#pragma once

#include <vector>

#include "Xenon/Base/SystemDevice.h"
#include "Xenon/Core/PCIBus/PCIBus.h"

class Bus
{
public:
	void Init();

    void AddPCIBus(PCIBus* newPCIBus);
	void AddDevice(SystemDevice* device);
	void Read(u64 readAddress, u64* data, u8 byteCount);
	void Write(u64 writeAddress, u64 data, u8 byteCount);
private:
    PCIBus* _PCIBus;
	u32 deviceCount;
	std::vector<SystemDevice*> conectedDevices;
};