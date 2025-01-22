// Copyright 2025 Xenon Emulator Project

#pragma once

#include <filesystem>
#include <vector>
#include "Types.h"

namespace Config {

void loadConfig(const std::filesystem::path& path);
void saveConfig(const std::filesystem::path& path);

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
// XGPU Options.
//

// Screen W/H. 
s32 windowWidth();
s32 windowHeight();

// s32 getGpuId();

}; // namespace Config
