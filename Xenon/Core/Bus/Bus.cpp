#include <iostream>

#include "Bus.h"

#define PCI_CONFIG_SPACE_BEGIN	0x200D0000000
#define PCI_CONFIG_SPACE_END	0x200D1000000

#define PCI_BUS_START_ADDR		0x200EA000000
#define PCI_BUS_END_ADDR		0x200EA010000

void Bus::Init()
{
	deviceCount = 0;
	conectedDevices.resize(deviceCount);
}

void Bus::AddPCIBus(PCIBus* newPCIBus)
{
	_PCIBus = newPCIBus;
}

void Bus::AddDevice(SystemDevice *device)
{
	deviceCount++;
	
	std::cout << "BUS-> New device attatched: " << device->GetDeviceName() << " "
		<< std::hex << "0x" << device->GetStartAddress() << " - 0x"
		<< device->GetEndAddress() << std::endl;

	conectedDevices.push_back(device);
}

void Bus::Read(u64 readAddress, u64* data, u8 byteCount)
{
	for (auto& device : conectedDevices)
	{
		if (readAddress >= device->GetStartAddress() && readAddress <= device->GetEndAddress())
		{
			// Hit
			device->Read(readAddress, data, byteCount);
			return;
		}
	}

	if (readAddress >= PCI_CONFIG_SPACE_BEGIN && readAddress <= PCI_CONFIG_SPACE_END)
	{
		_PCIBus->ConfigRead(readAddress, data, byteCount);
		return;
	}

	if (readAddress >= PCI_BUS_START_ADDR && readAddress <= PCI_BUS_END_ADDR)
	{
		// PCI Device, ask PCI Bus for it.
		_PCIBus->Read(readAddress, data, byteCount);
		return;
	}
	
	// Device not found
	std::cout << "BUS: Read failed at address 0x" << std::hex << readAddress << std::endl;
	*data = 0x0;

}

void Bus::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	for (auto& device : conectedDevices)
	{
		if (writeAddress >= device->GetStartAddress() && writeAddress <= device->GetEndAddress())
		{
			// Hit
			device->Write(writeAddress, data, byteCount);
			return;
		}
	}

	if (writeAddress >= PCI_CONFIG_SPACE_BEGIN && writeAddress <= PCI_CONFIG_SPACE_END)
	{
		_PCIBus->ConfigWrite(writeAddress, data, byteCount);
		return;
	}

	if (writeAddress >= PCI_BUS_START_ADDR && writeAddress <= PCI_BUS_END_ADDR)
	{
		// PCI Device, ask PCI Bus for it.
		_PCIBus->Write(writeAddress, data, byteCount);
		return;
	}

	// Device not found
	if (data != 0)
	{
		std::cout<<"BUS: Data write failed > (" << writeAddress << ") data = 0x"<<std::hex << data
			<< " data LE: 0x" << _byteswap_uint64(data) << std::endl;
	}
	else
	{
		std::cout << "BUS: Write failed:\n"
			"(0x" << writeAddress << ") data: 0x" << data
			<< " LE: 0x" << _byteswap_uint64(data) << std::endl;
	}
}
