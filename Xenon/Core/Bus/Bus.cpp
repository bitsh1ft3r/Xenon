#include <iostream>

#include "Bus.h"

#define PCI_CONFIG_SPACE_BEGIN 0xD0000000
#define PCI_CONFIG_SPACE_END 0XD1000000

#define PCI_BUS_START_ADDR 0xEA000000
#define PCI_BUS_END_ADDR 0xEA010000

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

	//SystemDevice newDevice = *device;
	
	std::cout << "BUS-> New device attatched: " << device->GetDeviceName() << " "
		<< std::hex << "0x" << device->GetStartAddress() << " - 0x"
		<< device->GetEndAddress() << std::endl;

	conectedDevices.push_back(device);
}

void Bus::Read(u64 readAddress, u64* data, u8 byteCount)
{
	for (auto& device : conectedDevices)
	{
		if ((u32)readAddress >= (u32)device->GetStartAddress() && (u32)readAddress <= (u32)device->GetEndAddress())
		{
			// Hit
			device->Read(readAddress, data, byteCount);
			return;
		}
	}

	PCI_TYPE1_CFG_BITS cfg;
	cfg.u.AsUINT32 = (u32)readAddress;

	if ((u32)readAddress >= PCI_CONFIG_SPACE_BEGIN && (u32)readAddress <= PCI_CONFIG_SPACE_END)
	{
		std::cout << "BUS: PCI Configuration read, Dev = 0x" << std::hex << cfg.u.bits.DeviceNumber << " Func = 0x" << cfg.u.bits.FunctionNumber
			<< " Reg = 0x" << cfg.u.bits.RegisterNumber << std::endl;
		*data = 0xff;
		return;
	}

	if ((u32)readAddress >= PCI_BUS_START_ADDR && (u32)readAddress <= PCI_BUS_END_ADDR)
	{
		// PCI Device, ask PCI Bus for it.
		_PCIBus->Read(readAddress, data, byteCount);
	}
	
	// Device not found
	std::cout << "BUS: Read failed at address 0x" << std::hex << readAddress << std::endl;
	*data = 0x0;

}

void Bus::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	PCI_TYPE1_CFG_BITS cfg;

	cfg.u.AsUINT32 = (u32)writeAddress;
	for (auto& device : conectedDevices)
	{
		if ((u32)writeAddress >= (u32)device->GetStartAddress() && (u32)writeAddress <= (u32)device->GetEndAddress())
		{
			// Hit
			device->Write(writeAddress, data, byteCount);
			return;
		}
	}

	if ((u32)writeAddress >= PCI_CONFIG_SPACE_BEGIN && (u32)writeAddress <= PCI_CONFIG_SPACE_END)
	{
		std::cout << "BUS: PCI Configuration write(0x" << writeAddress << "), Dev = 0x" << std::hex 
			<< cfg.u.bits.DeviceNumber << " Func = 0x" << cfg.u.bits.FunctionNumber
			<< " Reg = 0x" << cfg.u.bits.RegisterNumber << std::endl;
		return;
	}

	if ((u32)writeAddress >= PCI_BUS_START_ADDR && (u32)writeAddress <= PCI_BUS_END_ADDR)
	{
		// PCI Device, ask PCI Bus for it.
		_PCIBus->Write(writeAddress, data, byteCount);
	}

	// Device not found
	if (data != 0)
	{
		std::cout<<"BUS: Data write failed > (" << writeAddress << ") data = 0x"<<std::hex << data
			<< " data LE: 0x" << _byteswap_uint64(data) << std::endl;
	}
	else
	{
		//std::cout << "BUS: Write failed:\n"
		//	"(0x" << writeAddress << ") data: 0x" << data
		//	<< " LE: 0x" << _byteswap_uint64(data) << std::endl;
	}
}
