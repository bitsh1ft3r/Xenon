// Copyright 2025 Xenon Emulator Project

#pragma once

#include <filesystem>

#include "Types.h"
#include "Logging/Backend.h"

namespace Config {
// General.
inline bool gpuRenderThreadEnabled = true;
inline bool isFullscreen = false;
inline bool vsyncEnabled = true;
inline bool shouldQuitOnWindowClosure = false;
inline Base::Log::Level currentLogLevel = Base::Log::Level::Warning;

// SMC.
inline int smcPowerOnReason = 0x11; // SMC_PWR_REAS_EJECT
inline int comPort = 2;
inline std::string com = "";

// PowerPC.
inline u64 SKIP_HW_INIT_1 = 0;
inline u64 SKIP_HW_INIT_2 = 0;

// GPU.
inline s32 screenWidth = 1280;
inline s32 screenHeight = 720;
// inline s32 gpuId = -1; // Vulkan physical device index. Set to negative for auto select

// Filepaths.
inline std::string fusesTxtPath = "C:/Xbox/fuses.txt";
inline std::string oneBlBinPath = "C:/Xbox/1bl.bin";
inline std::string nandBinPath = "C:/Xbox/nand.bin";
inline std::string oddDiscImagePath = "C:/Xbox/xenon.iso";

// Highly experimental.
inline int ticksPerInstruction = 1;

void loadConfig(const std::filesystem::path &path);
void saveConfig(const std::filesystem::path &path);

//
// General Options.
//

// Selected COM Port.
std::string* COMPort();
// Show in fullscreen.
bool fullscreenMode(); 
// Enable VSync.
bool vsync();
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
// Filepaths.
//

// Fuses path
std::string fusesPath();
// 1bl.bin path
std::string oneBlPath();
// nand.bin path
std::string nandPath();
// ODD Image path
std::string oddImagePath();

//
// Highly experimental. (things that can either break the emulator or drastically increase performance)
//
int tpi();

} // namespace Config
