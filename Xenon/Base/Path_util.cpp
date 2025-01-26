// Copyright 2025 Xenon Emulator Project

#include <unordered_map>
#include "Path_util.h"

namespace Base::FS {

namespace fs = std::filesystem;

static auto UserPaths = [] {

    auto user_dir = std::filesystem::current_path() / PORTABLE_DIR;

    std::unordered_map<PathType, fs::path> paths;

    const auto create_path = [&](PathType xenon_path, const fs::path& new_path) {
        std::filesystem::create_directory(new_path);
        paths.insert_or_assign(xenon_path, new_path);
    };

    create_path(PathType::UserDir, user_dir);
    return paths;
}();

std::string PathToUTF8String(const std::filesystem::path& path) {
    const auto u8_string = path.u8string();
    return std::string{u8_string.begin(), u8_string.end()};
}

const fs::path& GetUserPath(PathType xenon_path) {
    return UserPaths.at(xenon_path);
}

std::string GetUserPathString(PathType xenon_path) {
    return PathToUTF8String(GetUserPath(xenon_path));
}

void SetUserPath(PathType xenon_path, const fs::path& new_path) {
    UserPaths.insert_or_assign(xenon_path, new_path);
}
}
