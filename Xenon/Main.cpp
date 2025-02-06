// Copyright 2025 Xenon Emulator Project

#include "Core/Xe_Main.h"

int main(int argc, char *argv[]) {
  Xe_Main = std::make_unique<STRIP_UNIQUE(Xe_Main)>();
  LOG_INFO(System, "Starting Xenon.");
  Xe_Main->start();
  // Vali: You're telling me a race condition that is literally 100µs is enough to flip that bit? Are you fucking kidding me????
  // Vali: Because of that, XeMain::isRunning is set then checked here. I hate C++
  Xe_Main->setRunning();
  while (Xe_Main->isRunning()) {}
  Xe_Main.reset();
  return 0;
}
