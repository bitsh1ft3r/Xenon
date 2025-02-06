// Copyright 2025 Xenon Emulator Project

#include "Core/Xe_Main.h"

int main(int argc, char *argv[]) {
  Xe_Main = std::make_unique<STRIP_UNIQUE(Xe_Main)>();
  LOG_INFO(System, "Starting Xenon.");
  Xe_Main->start();
  Xe_Main.reset();
  return 0;
}
