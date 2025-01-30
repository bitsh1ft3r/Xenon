// Copyright 2025 Xenon Emulator Project

#pragma once

#include <filesystem>

#include "Types.h"
#include "Logging/Backend.h"

namespace Config {

void loadConfig(const std::filesystem::path &path);
void saveConfig(const std::filesystem::path &path);

//
// General Options.
//

// Selected COM Port.
std::string* COMPort();
// Show in fullscreen.
bool fullscreenMode();
// Enable GPU Render thread.
bool gpuThreadEnabled();
// Should we quit when our rendering window is closed?
bool quitOnWindowClosure();
// Current log level.
Base::Log::Level getCurrentLogLevel();

//
// SMC Options.
//

// SMC Power On type (PowerButton, eject button, controller, etc...).
int smcPowerOnType();

//
// PowerPC Options.
//

// HW_INIT_SKIP.
u64 HW_INIT_SKIP1();
u64 HW_INIT_SKIP2();

//
// GPU Options.
//

// Screen Size.
s32 windowWidth();
s32 windowHeight();
// GPU ID Selection (Only for Vulkan)
// s32 getGpuId();

//
// Filepaths
//

// Fuses path
std::string fusesPath();
// 1bl.bin path
std::string oneBlPath();
// nand.bin path
std::string nandPath();

} // namespace Config
