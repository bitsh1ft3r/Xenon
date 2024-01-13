#pragma once

#include <iostream>
#include <vector>

#include "Xenon/Core/PCIBus/PCIDevice.h"

class PCIBus
{
public:
	void addPCIDevice(PCIDevice* device);

	void Read(u64 readAddress, u64* data, u8 byteCount);
	void Write(u64 writeAddress, u64 data, u8 byteCount);

	void ConfigRead(u64 readAddress, u64* data, u8 byteCount);
	void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount);

private:
	std::vector<PCIDevice*> connectedPCIDevices;
};