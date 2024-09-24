#pragma once

// Xenon - Xbox 360 CPU Implementation.
// 
// Contains:
// - 3 PPU's with SMT and VMX support.
// - 1MB L2 Cache with custom address decoding logic for hashing and crypto on L2 Cache.
// - Pseudo Random Number Generator.
// - 64 Kb SRAM.
// - 32 Kb SROM.
// - 768 bits of IBM's eFuse technology.

#include <thread>

#include "Xenon/Core/RootBus/RootBus.h"
#include "Xenon/Core/XCPU/PPU/PPU.h"


class Xenon
{
public:
	Xenon(RootBus* inBus, std::string blPath, eFuses inFuseSet);
	~Xenon();

	void Start(u64 resetVector = 0x100);
	Xe::XCPU::IIC::XenonIIC *GetIICPointer() { return &xenonContext.xenonIIC; }
private:
	// System Bus
	RootBus* mainBus = nullptr;

	XENON_CONTEXT xenonContext = {};

	// Power Processing Units, the effective execution units inside the XBox
	// 360 CPU.
	PPU ppu0;
	PPU ppu1;
	PPU ppu2;
	std::thread ppu0Thread;
	std::thread ppu1Thread;
	std::thread ppu2Thread;
};