// Copyright 2025 Xenon Emulator Project

#include <toml.hpp>

#include "Config.h"
#include "Path_util.h"
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

std::string* COMPort() {
    com = "\\\\.\\COM" + std::to_string(comPort);
    return &com;
}

bool fullscreenMode() { return isFullscreen; }

bool vsync() { return vsyncEnabled; }

bool gpuThreadEnabled() { return gpuRenderThreadEnabled; }

bool quitOnWindowClosure() { return shouldQuitOnWindowClosure; }

Base::Log::Level getCurrentLogLevel() { return currentLogLevel; }

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
    vsyncEnabled = toml::find_or<bool>(general, "VSync", false);
    shouldQuitOnWindowClosure =
        toml::find_or<bool>(general, "QuitOnWindowClosure", false);
    currentLogLevel = (Base::Log::Level)find_or<int>(general, "Loglevel", false);
  }

  if (data.contains("SMC")) {
    const toml::value &smc = data.at("SMC");
    comPort = toml::find_or<int>(smc, "COMPort", false);
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
    //        gpuId = toml::find_or<int>(gpu, "gpuId", -1);
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
  data["General"]["GPURenderThreadEnabled"] = gpuRenderThreadEnabled;
  data["General"]["Fullscreen"] = isFullscreen;
  data["General"]["VSync"] = vsyncEnabled;
  data["General"]["QuitOnWindowClosure"] = shouldQuitOnWindowClosure;
  data["General"]["Loglevel"] = (int)currentLogLevel;

  // SMC.                                      
  data["SMC"]["COMPort"] = comPort;
  data["SMC"]["SMCPowerOnType"] = smcPowerOnReason;

  // PowerPC.
  data["PowerPC"]["HW_INIT_SKIP1"] = SKIP_HW_INIT_1;
  data["PowerPC"]["HW_INIT_SKIP2"] = SKIP_HW_INIT_2;

  // GPU.
  data["GPU"]["screenWidth"] = screenWidth;
  data["GPU"]["screenHeight"] = screenHeight;
  //data["GPU"]["gpuId"] = gpuId;

  // Paths.
  data["Paths"]["Fuses"] = fusesTxtPath;
  data["Paths"]["OneBL"] = oneBlBinPath;
  data["Paths"]["Nand"] = nandBinPath;
  data["Paths"]["ISO"] = oddDiscImagePath;

  // HighlyExperimental.
  data["HighlyExperimental"].comments().push_back("# Do not touch these options unless you know what you're doing!");
  data["HighlyExperimental"].comments().push_back("# It can break execution! User beware.");
  data["HighlyExperimental"]["TPI"] = ticksPerInstruction;

  std::ofstream file(path, std::ios::binary);
  file << data;
  file.close();
}

} // namespace Config
