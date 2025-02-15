// Copyright 2025 Xenon Emulator Project

#include "Xenon.h"
#include <Core/Xe_Main.h>

#include "Base/Logging/Log.h"

Xenon::Xenon(RootBus *inBus, const std::string blPath, eFuses inFuseSet) {
  // First, Initialize system bus.
  mainBus = inBus;

  // Set SROM to 0.
  memset(xenonContext.SROM, 0, XE_SROM_SIZE);

  // Set SRAM to 0.
  memset(xenonContext.SRAM, 0, XE_SRAM_SIZE);

  // Set Security Engine context to 0.
  memset(&xenonContext.secEngBlock, 0, sizeof(SOCSECENG_BLOCK));
  memset(xenonContext.secEngData, 0, XE_SECENG_SIZE);

  // Populate FuseSet.
  xenonContext.fuseSet = inFuseSet;

  // Initilize PPU's.
  ppu0.Initialize(&xenonContext, mainBus, XE_PVR, 0, "PPU0"); // Threads 0-1
  ppu1.Initialize(&xenonContext, mainBus, XE_PVR, 2, "PPU1"); // Threads 2-3
  ppu2.Initialize(&xenonContext, mainBus, XE_PVR, 4, "PPU2"); // Threads 4-5

  // Load 1BL from path.
  std::ifstream file(blPath, std::ios_base::in | std::ios_base::binary);
  if (!file.is_open()) {
    LOG_CRITICAL(Xenon, "Unable to open file: {} for reading. Check your file path. System Stopped!", blPath);
    printf("Press Enter to continue..."); (void)getchar();
  } else {
    size_t fileSize = std::filesystem::file_size(blPath);
    if (fileSize == XE_SROM_SIZE) {      
      file.read(reinterpret_cast<char*>(xenonContext.SROM), XE_SROM_SIZE);
      LOG_INFO(Xenon, "1BL Loaded.");
    }
  }
}

Xenon::~Xenon() {}

void Xenon::Start(u64 resetVector) {
  // Start execution on every thread.
  ppu0Thread = std::thread(&PPU::StartExecution, PPU(ppu0));
  ppu0Thread.detach();

  ppu1Thread = std::thread(&PPU::StartExecution, PPU(ppu1));
  ppu1Thread.detach();

  ppu2Thread = std::thread(&PPU::StartExecution, PPU(ppu2));
  ppu2Thread.detach();

  while (Xe_Main->isRunning()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
