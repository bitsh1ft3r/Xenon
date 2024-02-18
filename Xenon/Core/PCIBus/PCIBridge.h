#pragma once

#include <iostream>
#include <vector>

#include "Xenon/Core/PCIBus/PCIDevice.h"

#define PCI_BRIDGE_CONFIG_SPACE_ADDRESS_BASE 0xD0000000
#define PCI_BRIDGE_CONFIG_SPACE_SIZE		 0x1000

class PCIBridge
{
public:
	PCIBridge();
	void addPCIDevice(PCIDevice* device);

	void Read(u64 readAddress, u64* data, u8 byteCount);
	void Write(u64 writeAddress, u64 data, u8 byteCount);

	void ConfigRead(u64 readAddress, u64* data, u8 byteCount);
	void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount);

private:
	std::vector<PCIDevice*> connectedPCIDevices;

	u8* pciBridgeConfigSpace = new u8[0x1000];
};