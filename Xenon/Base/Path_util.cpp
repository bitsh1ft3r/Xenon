// Copyright 2025 Xenon Emulator Project

#include "Path_util.h"
#include <unordered_map>
#include <fstream>

namespace Base::FS {

static auto UserPaths = [] {
  auto userDir = fs::current_path() / PORTABLE_DIR;

  std::unordered_map<PathType, fs::path> paths;

  bool createUserDir = true;

  // Vali: This is required to play nice
  if (!fs::exists(userDir)) {
    userDir = fs::current_path();
    // If we have xenon_config in the root of our directory, then just use it and create files there instead.
    std::ifstream f(userDir / "xenon_config.toml");
    if (f.is_open()) {
      createUserDir = false;
    }
  }

  const auto insert_path = [&](PathType xenon_path, const fs::path &new_path, bool create = true) {
    if (create && !fs::exists(new_path))
      fs::create_directory(new_path);

    paths.insert_or_assign(xenon_path, new_path);
  };

  insert_path(PathType::UserDir, userDir, createUserDir);
  insert_path(PathType::LogDir, userDir / LOG_DIR);
  return paths;
}();

std::string PathToUTF8String(const fs::path &path) {
  const auto u8_string = path.u8string();
  return std::string{u8_string.begin(), u8_string.end()};
}

const fs::path &GetUserPath(PathType xenon_path) {
  return UserPaths.at(xenon_path);
}

std::string GetUserPathString(PathType xenon_path) {
  return PathToUTF8String(GetUserPath(xenon_path));
}

void SetUserPath(PathType xenon_path, const fs::path &new_path) {
  UserPaths.insert_or_assign(xenon_path, new_path);
}
} // namespace Base::FS
