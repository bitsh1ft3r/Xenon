#include "PCIBus.h"

void PCIBus::addPCIDevice(PCIDevice* device)
{
	std::cout << "PCI Bus > New device attatched: " << device->GetDeviceName() << " "
		<< std::hex << "0x" << device->GetStartAddress() << " - 0x"
		<< device->GetEndAddress() << std::endl;

	connectedPCIDevices.push_back(device);
}

void PCIBus::Read(u64 readAddress, u64* data, u8 byteCount)
{
	for (auto& device : connectedPCIDevices)
	{
		if (readAddress >= device->GetStartAddress() && readAddress <= device->GetEndAddress())
		{
			// Hit
			device->Read(readAddress, data, byteCount);
			return;
		}
	}

	// Device not found
	std::cout << "PCI Bus: Read failed at address 0x" << std::hex << readAddress << std::endl;
	*data = 0xff;
}

void PCIBus::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	// Nothing connected to pci bus
	if (connectedPCIDevices.empty())
		return;
	for (auto& device : connectedPCIDevices)
	{
		if (writeAddress >= device->GetStartAddress() && writeAddress <= device->GetEndAddress())
		{
			// Hit
			device->Write(writeAddress, data, byteCount);
			return;
		}
	}

	// Device not found
	std::cout << "PCI Bus: Write failed at address 0x" << std::hex << writeAddress << std::endl;
}

void PCIBus::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
}

void PCIBus::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
}
