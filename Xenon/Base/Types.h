// Copyright 2025 Xenon Emulator Project

#pragma once

#include <cstdint>

// Signed
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

// Unsigned
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

// UDLs for memory size values
constexpr u64 operator""_KB(const u64 x) {
    return 1024ULL * x;
}
constexpr u64 operator""_MB(const u64 x) {
    return 1024_KB * x;
}
constexpr u64 operator""_GB(const u64 x) {
    return 1024_MB * x;
}
