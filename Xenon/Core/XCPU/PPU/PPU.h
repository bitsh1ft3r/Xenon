// Copyright 2025 Xenon Emulator Project

#pragma once

#include "PowerPC.h"

#include "Core/RootBus/RootBus.h"

class PPU
{
public:
	PPU();
	
	void Initialize(XENON_CONTEXT* inXenonContext, RootBus* mainBus, u32 PVR, u32 PIR,
		const char* ppuName);
	
	void StartExecution();

	// Returns a pointer to a thread.
	PPU_THREAD_REGISTERS* GetPPUThread(u8 thrdID);
private:
	// PPU running?
	bool ppuRunning = false;

	// Reset ocurred or signaled?
	bool systemReset = false;

	// Execution threads inside this PPU.
	PPU_STATE* ppuState;

	// Main CPU Context.
	XENON_CONTEXT *xenonContext = nullptr;

	// Helpers
	bool ppuReadNextInstruction();
	void ppuCheckExceptions();
	PPU_THREAD getCurrentRunningThreads();
};