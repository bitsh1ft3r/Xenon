#include <stdio.h>
#include <thread>
#include <stdlib.h>
#include <iostream>


#include "Xenon/Core/Bus/Bus.h"
#include "Xenon/Core/PCIBridge/PCIBridge.h"
#include "Xenon/Core/PostBus/PostBus.h"

#include "Xenon/Core/XCPU/Xenon.h"

#include "Xenon/Core/RAM/RAM.h"
#include "Xenon/Core/NAND/NAND.h"

#include "Xenon/Core/PCIBridge/SFCX/SFCX.h"
#include "Xenon/Core/PCIBridge/XMA/XMA.h"
#include "Xenon/Core/PCIBridge/HDD/HDD.h"
#include "Xenon/Core/PCIBridge/SMC/SMC.h"
#include "Xenon/Core/PCIBridge/CDROM/CDROM.h"

int main(int argc, char* argv[])
{
	eFuses jasperCpuFuses;
	jasperCpuFuses.fuseLine00 = 0xc0ffffffffffffff;
	jasperCpuFuses.fuseLine01 = 0x0f0f0f0f0f0f0ff0;
	jasperCpuFuses.fuseLine02 = 0x0000000000000000;
	jasperCpuFuses.fuseLine03 = 0x8CBA33C6B70BF641;
	jasperCpuFuses.fuseLine04 = 0x8CBA33C6B70BF641;
	jasperCpuFuses.fuseLine05 = 0x2AC5A81E6B41BFE6;
	jasperCpuFuses.fuseLine06 = 0x2AC5A81E6B41BFE6;
	jasperCpuFuses.fuseLine07 = 0x0000000000000000;
	jasperCpuFuses.fuseLine08 = 0x0000000000000000;
	jasperCpuFuses.fuseLine09 = 0x0000000000000000;
	jasperCpuFuses.fuseLine10 = 0x0000000000000000;
	jasperCpuFuses.fuseLine11 = 0x0000000000000000;

	eFuses xenonCpuFuses;
	xenonCpuFuses.fuseLine00 = 0xc0ffffffffffffff;
	xenonCpuFuses.fuseLine01 = 0x0f0f0f0f0f0f0f0f;
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
	
	Bus Bus;
	PCIBridge pciBridge;
	
	SFCX sfcx;
	XMA xma;
	CDROM cdrom;
	HDD hdd;
	SMC smc;

	PostBus postBus;
	NAND nandDevice;
	RAM ram;

	Bus.Init();

	sfcx.Initialize("SFCX", SFCX_DEV_SIZE);
	xma.Initialize("XMA", XMA_DEV_SIZE);
	cdrom.Initialize("CDROM", CDROM_DEV_SIZE);
	hdd.Initialize("HDD", HDD_DEV_SIZE);
	smc.Initialize("SMC", SMC_DEV_SIZE);

	pciBridge.addPCIDevice(&sfcx);
	pciBridge.addPCIDevice(&xma);
	pciBridge.addPCIDevice(&cdrom);
	pciBridge.addPCIDevice(&hdd);
	pciBridge.addPCIDevice(&smc);

	postBus.Initialize("PostBus", POST_BUS_ADDR, POST_BUS_ADDR, true);
	nandDevice.Initialize("NAND", NAND_START_ADDR, NAND_END_ADDR, true);
	ram.Initialize("RAM", RAM_START_ADDR, RAM_START_ADDR + RAM_SIZE, false);

	Bus.AddPCIBridge(&pciBridge);
	Bus.AddDevice(&postBus);
	Bus.AddDevice(&nandDevice);
	Bus.AddDevice(&ram);

	// NAND Load Path.
    nandDevice.Load("C://Xbox/jasper_xellmod.bin");

	Xenon xenonCPU(&Bus, "C://Xbox/1bl.bin", jasperCpuFuses);

	// CPU Start routine and entry point.
	xenonCPU.Start(0x20000000100);
	return 0;
}