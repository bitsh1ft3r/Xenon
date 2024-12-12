#include "Xenon/Core/RootBus/RootBus.h"
#include "Xenon/Core/RAM/RAM.h"
#include "Xenon/Core/NAND/NAND.h"
#include "Xenon/Core/XCPU/Xenon.h"
#include "Xenon/Core/RootBus/HostBridge/HostBridge.h"
#include "Xenon/Core/RootBus/HostBridge/XGPU/XGPU.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/PCIBridge.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/ETHERNET/Ethernet.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/AUDIOCTRLLR/AudioController.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/SFCX/SFCX.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/XMA/XMA.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/HDD/HDD.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/SMC.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/ODD/ODD.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/OHCI0/OHCI0.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/OHCI1/OHCI1.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/EHCI0/EHCI0.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/EHCI1/EHCI1.h"

int main(int argc, char* argv[])
{
	eFuses jasperCpuFuses;
	jasperCpuFuses.fuseLine00 = 0xc0ffffffffffffff;
	jasperCpuFuses.fuseLine01 = 0x0f0f0f0f0f0f0ff0;
	jasperCpuFuses.fuseLine02 = 0x0000000000000000;
	jasperCpuFuses.fuseLine03 = 0x2EBCD846F1A7711C;
	jasperCpuFuses.fuseLine04 = 0x2EBCD846F1A7711C;
	jasperCpuFuses.fuseLine05 = 0x8F06C4C7E3EC4961;
	jasperCpuFuses.fuseLine06 = 0x8F06C4C7E3EC4961;
	jasperCpuFuses.fuseLine07 = 0x0000000000000000;
	jasperCpuFuses.fuseLine08 = 0x0000000000000000;
	jasperCpuFuses.fuseLine09 = 0x0000000000000000;
	jasperCpuFuses.fuseLine10 = 0x0000000000000000;
	jasperCpuFuses.fuseLine11 = 0x0000000000000000;

	eFuses xenonCpuFuses;
	xenonCpuFuses.fuseLine00 = 0xc0ffffffffffffff;
	xenonCpuFuses.fuseLine01 = 0x0f0f0f0f0f0f0ff0;
	xenonCpuFuses.fuseLine02 = 0x0000000000000000;
	xenonCpuFuses.fuseLine03 = 0xF98C9725B2052FE2;
	xenonCpuFuses.fuseLine04 = 0xF98C9725B2052FE2;
	xenonCpuFuses.fuseLine05 = 0x08EE3C57932DCACA;
	xenonCpuFuses.fuseLine06 = 0x08EE3C57932DCACA;
	xenonCpuFuses.fuseLine07 = 0x0000000000000000;
	xenonCpuFuses.fuseLine08 = 0x0000000000000000;
	xenonCpuFuses.fuseLine09 = 0x0000000000000000;
	xenonCpuFuses.fuseLine10 = 0x0000000000000000;
	xenonCpuFuses.fuseLine11 = 0x0000000000000000;

	eFuses xedkCpuFuses;
	xedkCpuFuses.fuseLine00 = 0xc0ffffffffffffff;
	xedkCpuFuses.fuseLine01 = 0x0f0f0f0f0f0f0ff0;
	xedkCpuFuses.fuseLine02 = 0x0000000000000000;
	xedkCpuFuses.fuseLine03 = 0x8CBA33C6B70BF641;
	xedkCpuFuses.fuseLine04 = 0x8CBA33C6B70BF641;
	xedkCpuFuses.fuseLine05 = 0x2AC5A81E6B41BFE6;
	xedkCpuFuses.fuseLine06 = 0x2AC5A81E6B41BFE6;
	xedkCpuFuses.fuseLine07 = 0x0000000000000000;
	xedkCpuFuses.fuseLine08 = 0x0000000000000000;
	xedkCpuFuses.fuseLine09 = 0x0000000000000000;
	xedkCpuFuses.fuseLine10 = 0x0000000000000000;
	xedkCpuFuses.fuseLine11 = 0x0000000000000000;
	
	
	RootBus RootBus;
	HostBridge hostBridge;
	PCIBridge pciBridge;

	Xe::PCIDev::ETHERNET::ETHERNET ethernet;
	Xe::PCIDev::AUDIOCTRLR::AUDIOCTRLR audioController;
	Xe::PCIDev::OHCI0::OHCI0 ohci0;
	Xe::PCIDev::OHCI1::OHCI1 ohci1;
	Xe::PCIDev::EHCI0::EHCI0 ehci0;
	Xe::PCIDev::EHCI1::EHCI1 ehci1;

	// Create the Secure Flash Cntroller for Xbox Device, and load the Nand dump for emulation.
	SFCX sfcx("C://Xbox/nandflash.bin", &pciBridge);
	RAM ram;
	XMA xma;
	ODD odd(&pciBridge, &ram);
	HDD hdd(&pciBridge);
	SMC smc(&pciBridge);
	NAND nandDevice;

	Xe::Xenos::XGPU xenos(&ram);

	RootBus.Init();

	ohci0.Initialize("OHCI0", OHCI0_DEV_SIZE);
	ohci1.Initialize("OHCI1", OHCI1_DEV_SIZE);
	ehci0.Initialize("EHCI0", OHCI0_DEV_SIZE);
	ehci1.Initialize("EHCI1", OHCI1_DEV_SIZE);
	audioController.Initialize("AUDIOCTRLR", AUDIO_CTRLR_DEV_SIZE);
	ethernet.Initialize("ETHERNET", ETHERNET_DEV_SIZE);
	sfcx.Initialize("SFCX", SFCX_DEV_SIZE);
	xma.Initialize("XMA", XMA_DEV_SIZE);
	odd.Initialize("CDROM", ODD_DEV_SIZE);
	hdd.Initialize("HDD", HDD_DEV_SIZE);
	smc.Initialize("SMC", SMC_DEV_SIZE);

	pciBridge.addPCIDevice(&ohci0);
	pciBridge.addPCIDevice(&ohci1);
	pciBridge.addPCIDevice(&ehci0);
	pciBridge.addPCIDevice(&ehci1);
	pciBridge.addPCIDevice(&audioController);
	pciBridge.addPCIDevice(&ethernet);
	pciBridge.addPCIDevice(&sfcx);
	pciBridge.addPCIDevice(&xma);
	pciBridge.addPCIDevice(&odd);
	pciBridge.addPCIDevice(&hdd);
	pciBridge.addPCIDevice(&smc);

	hostBridge.RegisterXGPU(&xenos);
	hostBridge.RegisterPCIBridge(&pciBridge);

	nandDevice.Initialize("NAND", NAND_START_ADDR, NAND_END_ADDR, true);
	ram.Initialize("RAM", RAM_START_ADDR, RAM_START_ADDR + RAM_SIZE, false);

	RootBus.AddHostBridge(&hostBridge);
	RootBus.AddDevice(&nandDevice);
	RootBus.AddDevice(&ram);

	// NAND Load Path.
    nandDevice.Load("C://Xbox/nandflash.bin");

	// Load 1BL here from given path.
	Xenon xenonCPU(&RootBus, "C://Xbox/1bl.bin", xedkCpuFuses);

	pciBridge.RegisterIIC(xenonCPU.GetIICPointer());

	// CPU Start routine and entry point.
	xenonCPU.Start(0x20000000100);

	return 0;
}