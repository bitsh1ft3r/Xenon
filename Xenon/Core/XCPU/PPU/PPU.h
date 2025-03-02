// Copyright 2025 Xenon Emulator Project

#pragma once

#include "PowerPC.h"

#include "Core/RootBus/RootBus.h"

class PPU {
public:
  PPU(XENON_CONTEXT *inXenonContext, RootBus *mainBus, u32 PVR,
                  u32 PIR, const char *ppuName);

  void StartExecution();

  // Returns a pointer to a thread.
  PPU_THREAD_REGISTERS *GetPPUThread(u8 thrdID);

private:
  // Thread handle
  std::thread ppuThread;

  // PPU running?
  bool ppuRunning = false;

  // Reset ocurred or signaled?
  bool systemReset = false;

  // Execution threads inside this PPU.
  std::shared_ptr<PPU_STATE> ppuState;

  // Main CPU Context.
  XENON_CONTEXT *xenonContext = nullptr;

  // Amount of CPU ticks per instruction executed.
  u32 ticksPerInstruction = 0;

  // Helpers

  // Returns the number of instructions per second the current
  // host computer can process.
  u32 getIPS();
  // Read next intruction from memory,
  bool ppuReadNextInstruction();
  // Check for pending exceptions.
  void ppuCheckExceptions();
  // Updates the current PPU's time base and decrementer based on
  // the amount of ticks per instr we should perform.
  void updateTimeBase();
  // Gets the current running threads.
  PPU_THREAD getCurrentRunningThreads();
};
