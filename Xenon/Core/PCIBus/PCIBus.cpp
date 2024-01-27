#include "PCIBus.h"

// Device tree, taken from an actual system, Corona V6.
/*
	BUS0
		-Dev 0, Func 0: Microsoft Corp. XMA Decoder
		-Dev 1, Func 0: Microsoft Corp. SATA Controller - CDROM
		-Dev 2, Func 0: Microsoft Corp. SATA Controller - HDD
		-Dev 4, Func 0: Microsoft Corp. OHCI Controller 0
		-Dev 4, Func 1: Microsoft Corp. EHCI Controller 0
		-Dev 5, Func 0: Microsoft Corp. OHCI Controller 1
		-Dev 5, Func 1: Microsoft Corp. EHCI Controller 1
		-Dev 7, Func 0: Microsoft Corp. Fast Ethernet Adapter
		-Dev 8, Func 0: Microsoft Corp. Secure Flash Controller
		-Dev 9, Func 0: Microsoft Corp. 580C
		-Dev A, Func 0: Microsoft Corp. System Management Controller
		-Dev F, Func 0: Microsoft Corp. 5841
*/

#define XMA_DEV_NUM			0x0
#define CDROM_DEV_NUM		0x1
#define HDD_DEV_NUM			0x2
#define OHCI0_DEV_NUM		0x4
#define EHCI0_DEV_NUM		0x4
#define OHCI1_DEV_NUM		0x5
#define EHCI1_DEV_NUM		0x5
#define FAST_ETH_DEV_NUM	0x7
#define SFC_DEV_NUM			0x8
#define _580C_DEV_NUM		0x9
#define SMC_DEV_NUM			0xa
#define _5841_DEV_NUM		0xF

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
	std::cout << "PCI Bus: Write failed at address 0x" << std::hex 
		<< writeAddress << " data = 0x" << data << std::endl;
}

void PCIBus::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	u16 bus = (readAddress >> 20) & 0xF;
	u16 device = (readAddress >> 15) & 0x1F;
	u16 reg = readAddress & 0xFFF;

	// Current device Name
	std::string currentDevName = "";

	switch (device)
	{
	case XMA_DEV_NUM:
		currentDevName = "XMA";
		break;
	case CDROM_DEV_NUM:
		currentDevName = "CDROM";
		break;
	case HDD_DEV_NUM:
		currentDevName = "HDD";
		break;
	case OHCI0_DEV_NUM:
		currentDevName = "OHCI0";
		break;
	case OHCI1_DEV_NUM:
		currentDevName = "OHCI1";
		break;
	case FAST_ETH_DEV_NUM:
		currentDevName = "ETHERNET";
		break;
	case SFC_DEV_NUM:
		currentDevName = "SFC";
		break;
	case _580C_DEV_NUM:
		currentDevName = "580C";
		break;
	case SMC_DEV_NUM:
		currentDevName = "SMC";
		break;
	case _5841_DEV_NUM:
		currentDevName = "5841";
		break;
	default:
		std::cout << "PCI Config Space Read: Unknown device accessed: Dev 0x"
			<< device << " Reg 0x" << reg << std::endl;
		return;
		break;
	}

	for (auto& device : connectedPCIDevices)
	{
		if (device->GetDeviceName() == currentDevName)
		{
			// Hit!
			device->ConfigRead(readAddress, data, byteCount);
			return;
		}
	}

	std::cout << "PCI Read to unimplemented device: "
		<< currentDevName.c_str() << std::endl;
}

void PCIBus::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	u16 bus = (writeAddress >> 20) & 0xF;
	u16 device = (writeAddress >> 15) & 0x1F;
	u16 reg = writeAddress & 0xFFF;

	// Current device Name
	std::string currentDevName = "";

	switch (device)
	{
	case XMA_DEV_NUM:
		currentDevName = "XMA";
		break;
	case CDROM_DEV_NUM:
		currentDevName = "CDROM";
		break;
	case HDD_DEV_NUM:
		currentDevName = "HDD";
		break;
	case OHCI0_DEV_NUM:
		currentDevName = "OHCI0";
		break;
	case OHCI1_DEV_NUM:
		currentDevName = "OHCI1";
		break;
	case FAST_ETH_DEV_NUM:
		currentDevName = "ETHERNET";
		break;
	case SFC_DEV_NUM:
		currentDevName = "SFC";
		break;
	case _580C_DEV_NUM:
		currentDevName = "580C";
		break;
	case SMC_DEV_NUM:
		currentDevName = "SMC";
		break;
	case _5841_DEV_NUM:
		currentDevName = "5841";
		break;
	default:
		std::cout << "PCI Config Space Write: Unknown device accessed: Dev 0x"
			<< device << " Func 0x" << " Reg 0x" << reg << " data = 0x"
			<< data << std::endl;
		return;
		break;
	}
	
	for (auto &device : connectedPCIDevices)
	{
		if (device->GetDeviceName() == currentDevName)
		{
			// Hit!
			device->ConfigWrite(writeAddress, data, byteCount);
			return;
		}
	}
	std::cout << "PCI Write to unimplemented device: " 
		<< currentDevName.c_str() << std::endl;
}
