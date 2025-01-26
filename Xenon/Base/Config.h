// Copyright 2025 Xenon Emulator Project

#pragma once

#include "Types.h"
#include <filesystem>

namespace Config {

void loadConfig(const std::filesystem::path &path);
void saveConfig(const std::filesystem::path &path);

//
// General Options.
//

// Show in fullscreen.
bool fullscreenMode();
// Enable GPU Render thread.
bool gpuThreadEnabled();

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

} // namespace Config
