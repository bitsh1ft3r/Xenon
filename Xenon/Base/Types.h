// Copyright 2025 Xenon Emulator Project

#pragma once

// Signed
using s8 = signed char;
using s16 = short;
using s32 = int;
using s64 = long long;

// Unsigned
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using f32 = float;
using f64 = double;

// UDLs for memory size values
constexpr unsigned long long operator""_KB(unsigned long long x) {
    return 1024ULL * x;
}
constexpr unsigned long long operator""_MB(unsigned long long x) {
    return 1024_KB * x;
}
constexpr unsigned long long operator""_GB(unsigned long long x) {
    return 1024_MB * x;
}
