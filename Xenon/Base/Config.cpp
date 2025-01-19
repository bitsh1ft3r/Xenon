// Copyright 2025 Xenon Emulator Project

#include <iostream>
#include <toml.hpp>

#include <fstream>
#include <string>

#include "Config.h"
#include "Path_util.h"

namespace toml {
template <typename TC, typename K>
std::filesystem::path find_fs_path_or(const basic_value<TC>& v, const K& ky,
                                      std::filesystem::path opt) {
    try {
        auto str = find<std::string>(v, ky);
        if (str.empty()) {
            return opt;
        }
        std::u8string u8str{(char8_t*)&str.front(), (char8_t*)&str.back() + 1};
        return std::filesystem::path{u8str};
    } catch (...) {
        return opt;
    }
}
} // namespace toml

namespace Config {

static bool isFullscreen = false;

static s32 screenWidth = 1280;
static s32 screenHeight = 720;
// static s32 gpuId = -1; // Vulkan physical device index. Set to negative for auto select


bool getIsFullscreen() {
    return isFullscreen;
}

s32 getScreenWidth() {
    return screenWidth;
}

s32 getScreenHeight() {
    return screenHeight;
}

// s32 getGpuId() {
//     return gpuId;
// }

void load(const std::filesystem::path& path) {
    // If the configuration file does not exist, create it and return
    std::error_code error;
    if (!std::filesystem::exists(path, error)) {
        save(path);
        return;
    }

    toml::value data;

    try {
        data = toml::parse(path);
    } catch (std::exception& ex) {
			std::cout << "Got exception trying to load config file. Exception: " << ex.what() << std::endl;
        return;
    }
    if (data.contains("General")) {
        const toml::value& general = data.at("General");

        isFullscreen = toml::find_or<bool>(general, "Fullscreen", false);
	}

    if (data.contains("GPU")) {
        const toml::value& gpu = data.at("GPU");

        screenWidth = toml::find_or<int>(gpu, "screenWidth", screenWidth);
        screenHeight = toml::find_or<int>(gpu, "screenHeight", screenHeight);
//        gpuId = toml::find_or<int>(gpu, "gpuId", -1);
    }
}

void save(const std::filesystem::path& path) {
    toml::value data;

    std::error_code error;
    if (std::filesystem::exists(path, error)) {
        try {
        data = toml::parse(path);
        } catch (const std::exception& ex) {
				std::cout << "Exception trying to parse config file. Exception: " << ex.what() << std::endl;
            return;
        }
    } else {
        if (error) {
            std::cout << "Filesystem error: " << error.message() << std::endl;
        }
        std::cout << "Saving new configuration file " << path.string() << std::endl;
    }

    data["General"]["Fullscreen"] = isFullscreen;

    data["GPU"]["screenWidth"] = screenWidth;
    data["GPU"]["screenHeight"] = screenHeight;
//    data["GPU"]["gpuId"] = gpuId;

    std::ofstream file(path, std::ios::binary);
    file << data;
    file.close();
}

} // namespace Config
