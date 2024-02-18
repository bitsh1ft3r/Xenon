#include <iostream>

#include "Bus.h"

#define PCI_CONFIG_SPACE_BEGIN	0xD0000000
#define PCI_CONFIG_SPACE_END	0xD1000000

#define PCI_BRIDGE_START_ADDR	0xEA000000
#define PCI_BRIDGE_END_ADDR		0xEA010000

void Bus::Init()
{
	deviceCount = 0;
	conectedDevices.resize(deviceCount);
}

void Bus::AddPCIBridge(PCIBridge* newPCIBridge)
{

	_PCIBridge = newPCIBridge;
}

void Bus::AddDevice(SystemDevice *device)
{
	deviceCount++;
	
	std::cout << "BUS-> New device attatched: " << device->GetDeviceName() << " "
		<< std::hex << "0x" << device->GetStartAddress() << " - 0x"
		<< device->GetEndAddress() << std::endl;

	conectedDevices.push_back(device);
}

void Bus::Read(u64 readAddress, u64* data, u8 byteCount, bool SOC)
{
	// First check if its a configuration Read.
	if (readAddress >= PCI_CONFIG_SPACE_BEGIN && readAddress <= PCI_CONFIG_SPACE_END)
	{
		_PCIBridge->ConfigRead(readAddress, data, byteCount);
		return;
	}
	// PCI Bridge read?
	if (readAddress >= PCI_BRIDGE_START_ADDR && readAddress <= PCI_BRIDGE_END_ADDR)
	{
		// PCI Device, ask PCI Bridge for it.
		_PCIBridge->Read(readAddress, data, byteCount);
		return;
	}

	// System device.
	if (SOC)
	{
		for (auto& device : conectedDevices)
		{
			if (device->IsSOCDevice() && readAddress >= device->GetStartAddress() && readAddress <= device->GetEndAddress())
			{
				// Hit
				device->Read(readAddress, data, byteCount);
				return;
			}
		}
	}
	else
	{
		for (auto& device : conectedDevices)
		{
			if (device->GetDeviceName() == "NAND" && readAddress >= device->GetStartAddress() && readAddress <= device->GetEndAddress())
			{
				// special case for now, ram is accesible from both soc and phys access,
				// this is probably due to SFC being mm to that address.
				device->Read(readAddress, data, byteCount);
				return;
			}
			if (!device->IsSOCDevice() && readAddress >= device->GetStartAddress() && readAddress <= device->GetEndAddress())
			{
				// Hit
				device->Read(readAddress, data, byteCount);
				return;
			}
		}

	}
	
	// Device not found
	std::cout << "BUS: Read failed at address 0x" << std::hex << readAddress << " SOC = " << SOC << std::endl;

	// Any reads to bus that dont belong to any device are always 0xFF.
	*data = 0xFFFFFFFFFFFFFFFF;

}

void Bus::Write(u64 writeAddress, u64 data, u8 byteCount, bool SOC)
{

	if (writeAddress >= PCI_CONFIG_SPACE_BEGIN && writeAddress <= PCI_CONFIG_SPACE_END)
	{
		_PCIBridge->ConfigWrite(writeAddress, data, byteCount);
		return;
	}

	if (writeAddress >= PCI_BRIDGE_START_ADDR && writeAddress <= PCI_BRIDGE_END_ADDR)
	{
		// PCI Device, ask PCI Bridge for it.
		_PCIBridge->Write(writeAddress, data, byteCount);
		return;
	}

	// System device.
	if (SOC)
	{
		for (auto& device : conectedDevices)
		{
			if (device->IsSOCDevice() && writeAddress >= device->GetStartAddress() && writeAddress <= device->GetEndAddress())
			{
				// Hit
				device->Write(writeAddress, data, byteCount);
				return;
			}
		}
	}
	else
	{
		for (auto& device : conectedDevices)
		{
			if (device->GetDeviceName() == "NAND" && writeAddress >= device->GetStartAddress() && writeAddress <= device->GetEndAddress())
			{
				// special case for now, ram is accesible from both soc and phys access,
				// this is probably due to SFC being mm to that address.
				device->Write(writeAddress, data, byteCount);
				return;
			}
				
			if (!device->IsSOCDevice() && writeAddress >= device->GetStartAddress() && writeAddress <= device->GetEndAddress())
			{
				// Hit
				device->Write(writeAddress, data, byteCount);
				return;
			}
		}

	}

	// Device not found

	std::cout<<"BUS: Write failed > (" << writeAddress << ") data = 0x"<<std::hex << data << std::endl;


}
