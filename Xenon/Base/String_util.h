// Copyright 2025 Xenon Emulator Project

#pragma once

#include <string>
#include <vector>

namespace Base {

/// Make a string lowercase
[[nodiscard]] std::string ToLower(std::string_view str);

void ToLowerInPlace(std::string& str);

std::vector<std::string> SplitString(const std::string& str, char delimiter);

std::string_view U8stringToString(std::u8string_view u8str);

#ifdef _WIN32
[[nodiscard]] std::string UTF16ToUTF8(std::wstring_view input);
[[nodiscard]] std::wstring UTF8ToUTF16W(std::string_view str);
#endif

} // namespace Base
