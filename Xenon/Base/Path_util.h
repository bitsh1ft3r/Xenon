// Copyright 2025 Xenon Emulator Project

#pragma once

#include <filesystem>

namespace Base::FS {

enum class PathType { UserDir };

constexpr auto PORTABLE_DIR = "Xenon";

[[nodiscard]] std::string PathToUTF8String(const std::filesystem::path &path);

[[nodiscard]] const std::filesystem::path &GetUserPath(PathType user_path);

[[nodiscard]] std::string GetUserPathString(PathType user_path);

void SetUserPath(PathType user_path, const std::filesystem::path &new_path);

} // namespace Base::FS
