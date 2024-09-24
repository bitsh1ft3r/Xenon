#include <iostream>

#include "RootBus.h"

#define PCI_CONFIG_SPACE_BEGIN	0xD0000000
#define PCI_CONFIG_SPACE_END	0xD1000000

#define PCI_BRIDGE_START_ADDR	0xEA000000
#define PCI_BRIDGE_END_ADDR		0xEA010000

void RootBus::Init()
{
	deviceCount = 0;
	conectedDevices.resize(deviceCount);
}

void RootBus::AddHostBridge(HostBridge* newHostBridge)
{
	hostBridge = newHostBridge;
}

void RootBus::AddDevice(SystemDevice *device)
{
	deviceCount++;
	
	std::cout << "BUS-> New device attatched: " << device->GetDeviceName() << " "
		<< std::hex << "0x" << device->GetStartAddress() << " - 0x"
		<< device->GetEndAddress() << std::endl;

	conectedDevices.push_back(device);
}

void RootBus::Read(u64 readAddress, u64* data, u8 byteCount, bool ramAccess)
{
	// Configuration Read?
	if (readAddress >= PCI_CONFIG_REGION_ADDRESS && readAddress <= PCI_CONFIG_REGION_ADDRESS
		+ PCI_CONFIG_REGION_SIZE)
	{
		ConfigRead(readAddress, data, byteCount);
		return;
	}

	for (auto& device : conectedDevices)
	{
		if (readAddress >= device->GetStartAddress() && readAddress <= device->GetEndAddress())
		{
			// Hit
			device->Read(readAddress, data, byteCount);
			return;
		}
	}

	// Check on the other Busses.
	if (hostBridge->Read(readAddress, data, byteCount))
	{
		return;
	}

	// Device not found
	std::cout << "BUS: Read failed at address 0x" << std::hex << readAddress << " RAM Access = " << ramAccess << std::endl;

	// Any reads to bus that dont belong to any device are always 0xFF.
	*data = 0xFFFFFFFFFFFFFFFF;

}

void RootBus::Write(u64 writeAddress, u64 data, u8 byteCount, bool ramAccess)
{
	// PCI Configuration Write?
	if (writeAddress >= PCI_CONFIG_REGION_ADDRESS && writeAddress <= PCI_CONFIG_REGION_ADDRESS
		+ PCI_CONFIG_REGION_SIZE)
	{
		ConfigWrite(writeAddress, data, byteCount);
		return;
	}

	for (auto& device : conectedDevices)
	{
		if (writeAddress >= device->GetStartAddress() && writeAddress <= device->GetEndAddress())
		{
			// Hit
			device->Write(writeAddress, data, byteCount);
			return;
		}
	}

	// Check on the other Busses.
	if (hostBridge->Write(writeAddress, data, byteCount))
	{
		return;
	}

	// Device or address not found
	std::cout<<"BUS: Write failed > (" << writeAddress << ") data = 0x"<<std::hex << data << std::endl;
}

//
// Configuration R/W.
//

void RootBus::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	hostBridge->ConfigRead(readAddress, data, byteCount);
}

void RootBus::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	hostBridge->ConfigWrite(writeAddress, data, byteCount);
}
