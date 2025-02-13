// Copyright 2025 Xenon Emulator Project

#include <toml.hpp>

#include "Config.h"

#include "Logging/Log.h"

namespace toml {
template <typename TC, typename K>
std::filesystem::path find_fs_path_or(const basic_value<TC> &v, const K &ky,
                                      std::filesystem::path opt) {
  try {
    auto str = find<std::string>(v, ky);
    if (str.empty()) {
      return opt;
    }
    std::u8string u8str{(char8_t *)&str.front(), (char8_t *)&str.back() + 1};
    return std::filesystem::path{u8str};
  } catch (...) {
    return opt;
  }
}
} // namespace toml

namespace Config {

// General.
static bool gpuRenderThreadEnabled = true;
static bool isFullscreen = false;
static bool shouldQuitOnWindowClosure = false;
Base::Log::Level currentLogLevel = Base::Log::Level::Warning;
static bool islogAdvanced = false;

// SMC.
static int smcAvPackType = 31; // Set to HDMI_NO_AUDIO. See SMC.cpp for a list of values.

static int smcPowerOnReason = 0x11; // Valid values are: 0x11 (SMC_PWR_REAS_PWRBTN) and 0x12
                                    // (SMC_PWR_REAS_EJECT).
static int comPort = 2;
static std::string com = "";

// PowerPC.
static u64 SKIP_HW_INIT_1 = 0;
static u64 SKIP_HW_INIT_2 = 0;

// GPU.
static s32 screenWidth = 1280;
static s32 screenHeight = 720;
// static s32 gpuId = -1; // Vulkan physical device index. Set to negative for
// auto select

// Filepaths.
static std::string fusesTxtPath = "C:/Xbox/fuses.txt";
static std::string oneBlBinPath = "C:/Xbox/1bl.bin";
static std::string nandBinPath = "C:/Xbox/nand.bin";
static std::string oddDiscImagePath = "C:/Xbox/xenon.iso";

// Highly experimental.
static int ticksPerInstruction = 1;

std::string* COMPort() {
    com = "\\\\.\\COM" + std::to_string(comPort);
    return &com;
}

bool fullscreenMode() { return isFullscreen; }

bool gpuThreadEnabled() { return gpuRenderThreadEnabled; }

bool quitOnWindowClosure() { return shouldQuitOnWindowClosure; }

Base::Log::Level getCurrentLogLevel() { return currentLogLevel; }

bool logAdvanced() { return islogAdvanced; }

int smcCurrentAvPack() { return smcAvPackType; }

int smcPowerOnType() { return smcPowerOnReason; }

u64 HW_INIT_SKIP1() { return SKIP_HW_INIT_1; }

u64 HW_INIT_SKIP2() { return SKIP_HW_INIT_2; }

s32 windowWidth() { return screenWidth; }

s32 windowHeight() { return screenHeight; }

std::string fusesPath() { return fusesTxtPath; }

std::string oneBlPath() { return oneBlBinPath; }

std::string nandPath() { return nandBinPath; }

std::string oddImagePath() { return oddDiscImagePath; }

// s32 getGpuId() {
//     return gpuId;
// }

int tpi() { return ticksPerInstruction; }

void loadConfig(const std::filesystem::path &path) {
  // If the configuration file does not exist, create it and return.
  LOG_INFO(Config, "Loading configuration from: {}", path.string());
  std::error_code error;
  if (!std::filesystem::exists(path, error)) {
    saveConfig(path);
    return;
  }

  toml::value data;

  try {
    data = toml::parse(path);
  } catch (std::exception &ex) {
    LOG_ERROR(Config, "Got exception trying to load config file. Exception: {}", 
        ex.what());
    return;
  }

  if (data.contains("General")) {
    const toml::value &general = data.at("General");
    gpuRenderThreadEnabled =
        toml::find_or<bool>(general, "GPURenderThreadEnabled", false);
    isFullscreen = toml::find_or<bool>(general, "Fullscreen", false);
    shouldQuitOnWindowClosure =
        toml::find_or<bool>(general, "QuitOnWindowClosure", false);
    currentLogLevel = (Base::Log::Level)find_or<int>(general, "Loglevel", false);
    islogAdvanced = toml::find_or<bool>(general, "logAdvanced", false);
  }

  if (data.contains("SMC")) {
    const toml::value &smc = data.at("SMC");
    comPort = toml::find_or<int>(smc, "COMPort", false);
    smcAvPackType = toml::find_or<int>(smc, "SMCAvPackType", false);
    smcPowerOnReason = toml::find_or<int>(smc, "SMCPowerOnType", false);
  }

  if (data.contains("PowerPC")) {
    const toml::value &powerpc = data.at("PowerPC");
    SKIP_HW_INIT_1 = toml::find_or<u64>(powerpc, "HW_INIT_SKIP1", false);
    SKIP_HW_INIT_2 = toml::find_or<u64>(powerpc, "HW_INIT_SKIP2", false);
  }

  if (data.contains("GPU")) {
    const toml::value &gpu = data.at("GPU");
    screenWidth = toml::find_or<int>(gpu, "screenWidth", screenWidth);
    screenHeight = toml::find_or<int>(gpu, "screenHeight", screenHeight);
    // gpuId = toml::find_or<int>(gpu, "gpuId", -1);
  }

  if (data.contains("Paths")) {
    const toml::value &paths = data.at("Paths");
    fusesTxtPath =
        toml::find_or<std::string>(paths, "Fuses", fusesTxtPath);
    oneBlBinPath =
        toml::find_or<std::string>(paths, "OneBL", oneBlBinPath);
    nandBinPath =
        toml::find_or<std::string>(paths, "Nand", nandBinPath);
    oddDiscImagePath =
        toml::find_or<std::string>(paths, "ODDImage", oddDiscImagePath);
  }

  if (data.contains("HighlyExperimental")) {
    const toml::value &highlyExperimental = data.at("HighlyExperimental");
    ticksPerInstruction =
        toml::find_or<int>(highlyExperimental, "TPI", ticksPerInstruction);
  }
}

void saveConfig(const std::filesystem::path &path) {
  toml::value data;

  std::error_code error;
  if (std::filesystem::exists(path, error)) {
    try {
      data = toml::parse(path);
    } catch (const std::exception &ex) {
      LOG_ERROR(Config, "Exception trying to parse config file. Exception: {}",
          ex.what());
      return;
    }
  } else {
    if (error) {
      LOG_ERROR(Config, "Filesystem error: {}", error.message());
    }
      LOG_INFO(Config, "Config not found. Saving new configuration file: {}", path.string());
  }

  // General.
  data["General"]["GPURenderThreadEnabled"].comments().push_back("# Enable the GPU Render thread and main window.");
  data["General"]["GPURenderThreadEnabled"] = gpuRenderThreadEnabled;
  data["General"]["GPURenderThreadEnabled"].comments().push_back("");
  data["General"]["Fullscreen"] = isFullscreen;
  data["General"]["QuitOnWindowClosure"] = shouldQuitOnWindowClosure;
  data["General"]["Loglevel"].comments().push_back("# Controls the current log level output filter.");
  data["General"]["Loglevel"] = (int)currentLogLevel;
  data["General"]["logAdvanced"] = islogAdvanced;

  // SMC.                                      
  data["SMC"]["COMPort"].comments().push_back("# Current vCOM Port used for communication between Xenon and your PC.");
  data["SMC"]["COMPort"] = comPort;
  data["SMC"]["SMCAvPackType"].comments().push_back("# The current connected AV Pack. Used to set Xenos internal render resolution.");
  data["SMC"]["SMCAvPackType"].comments().push_back("# Default value is 31 (HDMI_NO_AUDIO) = 1280*720.");
  data["SMC"]["SMCAvPackType"].comments().push_back("# Lowest value is 87 (COMPOSITE) = 640*480.");
  data["SMC"]["SMCAvPackType"].comments().push_back("# Note: The window size must never be smaller than the internal resolution.");
  data["SMC"]["SMCAvPackType"] = smcAvPackType;
  data["SMC"]["SMCPowerOnType"].comments().push_back("# SMC power-up type/cause (Power Button, Eject Button, etc...)");
  data["SMC"]["SMCPowerOnType"].comments().push_back("# Most used values are:");
  data["SMC"]["SMCPowerOnType"].comments().push_back("# 17: Console is being powered by a Power button press.");
  data["SMC"]["SMCPowerOnType"].comments().push_back("# 18: Console is being powered by an Eject button press.");
  data["SMC"]["SMCPowerOnType"].comments().push_back("# Note: When trying to boot Linux/XeLL Reloaded this must be set to 18.");
  data["SMC"]["SMCPowerOnType"] = smcPowerOnReason;

  // PowerPC.
  data["PowerPC"]["HW_INIT_SKIP1"].comments().push_back("# Hardware Init Skip address 1.");
  data["PowerPC"]["HW_INIT_SKIP1"] = SKIP_HW_INIT_1;
  data["PowerPC"]["HW_INIT_SKIP2"].comments().push_back("# Hardware Init Skip address 2.");
  data["PowerPC"]["HW_INIT_SKIP2"] = SKIP_HW_INIT_2;

  // GPU.
  data["GPU"].comments().push_back("# Window Size (Not the Internal Resolution!)");
  data["GPU"]["screenWidth"] = screenWidth;
  data["GPU"]["screenHeight"] = screenHeight;
  // data["GPU"]["gpuId"] = gpuId;

  // Paths.
  data["Paths"]["Fuses"] = fusesTxtPath;
  data["Paths"]["OneBL"] = oneBlBinPath;
  data["Paths"]["Nand"] = nandBinPath;
  data["Paths"]["ODDImage"] = oddDiscImagePath;

  // HighlyExperimental.
  data["HighlyExperimental"].comments().push_back("# Do not touch these options unless you know what you're doing!");
  data["HighlyExperimental"].comments().push_back("# It can break execution! User beware.");
  data["HighlyExperimental"]["TPI"] = ticksPerInstruction;

  std::ofstream file(path, std::ios::binary);
  file << data;
  file.close();
}

} // namespace Config
