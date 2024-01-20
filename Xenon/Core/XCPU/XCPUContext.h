#pragma once

#include <vector>

#include "Xenon/Core/Bus/Bus.h"
#include "Xenon/Core/XCPU/Interpreter/PowerPCState.h"


// SROM 32Kb - Stores 1BL
#define SROM_ADDR			0x20000000000ULL
#define SROM_SIZE			0x8000

// SRAM 64Kb
#define SRAM_ADDR			0x20000010000ULL
#define SRAM_SIZE			0xffff

// 768 Bit eFuse
#define FUSESET_LOC			0x20000020000ULL
#define FUSESET_SIZE		0x17ff 

#define XCPU_START_ADDR		0x20000000100ULL

// Cache Entry used for L2
struct CACHE_WAY
{
	bool V = false; // Valid entry
	u32 entryAddress = 0;
	u32 cacheFlags = 0;;
	u32 address = 0;
};

struct L2_CACHE_BLOCK
{
	bool V = 0;
	u64 address = 0;
	u8* data = new(u8[128]);
};

struct XCPUContext {
	// PowerPC CPU Cores
	PPCState cpuCores[6];

	// Current CPU ID
	s8 currentCoreID;

	// Execution Status
	bool executionRunning;

	// Bus pointer
	Bus* bus;

	u8* SROM = new u8[SROM_SIZE];
	u8* SRAM = new u8[SRAM_SIZE];

	// 0x2000 cache blocks * 0x80 bytes CACHELINE_SIZE = 1Mb Cache. 
	L2_CACHE_BLOCK l2Cache[0x2000];
};
