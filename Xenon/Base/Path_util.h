// Copyright 2025 Xenon Emulator Project

#pragma once

#include <filesystem>

namespace Base::FS {

enum class PathType {
  UserDir,   // Where Xenon stores its data.
  LogDir,    // Where log files are stored.
};

// Vali: This needs to be blank for Linux to play nice
constexpr auto PORTABLE_DIR = "";

constexpr auto LOG_DIR = "log";

constexpr auto LOG_FILE = "xenon_log.txt";

[[nodiscard]] std::string PathToUTF8String(const std::filesystem::path &path);

[[nodiscard]] const std::filesystem::path &GetUserPath(PathType user_path);

[[nodiscard]] std::string GetUserPathString(PathType user_path);

void SetUserPath(PathType user_path, const std::filesystem::path &new_path);

} // namespace Base::FS
