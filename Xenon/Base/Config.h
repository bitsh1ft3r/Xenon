// Copyright 2025 Xenon Emulator Project

#pragma once

#include <filesystem>
#include <vector>
#include "Types.h"

namespace Config {

void load(const std::filesystem::path& path);
void save(const std::filesystem::path& path);

bool fullscreenMode();

s32 getScreenWidth();
s32 getScreenHeight();

// s32 getGpuId();

}; // namespace Config
