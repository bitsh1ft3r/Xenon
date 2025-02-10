// Copyright 2025 Xenon Emulator Project

#pragma once

#include <cstdint>
#include <type_traits>
#include <format>

// Signed
using s8 = signed char;
using s16 = short;
using s32 = int;
using sl32 = long;
using s64 = long long;

// Unsigned
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using ul32 = unsigned long;
using u64 = unsigned long long;

// Floating point
using f32 = float;
using f64 = double;

// UDLs for memory size values
[[nodiscard]] inline constexpr u64 operator""_KB(const u64 x) {
    return 1024ULL * x;
}
[[nodiscard]] inline constexpr u64 operator""_MB(const u64 x) {
    return 1024_KB * x;
}
[[nodiscard]] inline constexpr u64 operator""_GB(const u64 x) {
    return 1024_MB * x;
}

// Min/max value of a typetemplate <typename T>
template <typename T>
static consteval T min_t() {
	if constexpr (std::is_unsigned_v<T>) {
		return 0;
	} else if constexpr (std::is_same_v<T, s64>) {
		return (-0x7FFFFFFFFFFFFFFF - 1);
	} else if constexpr (std::is_same_v<T, s32> || std::is_same_v<T, sl32>) {
		return (-0x7FFFFFFF - 1);
	} else if constexpr (std::is_same_v<T, s16>) {
		return (-0x7FFF - 1);
	} else if constexpr (std::is_same_v<T, s8>) {
		return (-0x7F - 1);
	}
}
template <typename T>
class min {
public:
	static constexpr T value = min_t<T>();
};
template <typename T>
constexpr T min_v = min<T>::value;
template <typename T>
static consteval T max_t() {
	if constexpr (std::is_same_v<T, u64>) {
		return 0xFFFFFFFFFFFFFFFFu;
	} else if constexpr (std::is_same_v<T, s64>) {
		return 0x7FFFFFFFFFFFFFFF;
	} else if constexpr (std::is_same_v<T, u32> || std::is_same_v<T, ul32>) {
		return 0xFFFFFFFF;
	} else if constexpr (std::is_same_v<T, s32> || std::is_same_v<T, sl32>) {
		return 0x7FFFFFFF;
	} else if constexpr (std::is_same_v<T, u16>) {
		return 0xFFFF;
	} else if constexpr (std::is_same_v<T, s16>) {
		return 0x7FFF;
	} else if constexpr (std::is_same_v<T, u8>) {
		return 0xFF;
	} else if constexpr (std::is_same_v<T, s8>) {
		return 0x7F;
	}
}
template <typename T>
class max {
public:
	static constexpr T value = max_t<T>();
};
template <typename T>
constexpr T max_v = max<T>::value;

extern void assert_fail_debug_msg(const std::string& msg);

// Array accessors
template <typename cT, typename T>
	requires requires (cT&& x) { std::size(x); std::data(x); } || requires (cT && x) {	std::size(x); x.front(); }
[[nodiscard]] constexpr auto& c_at(cT&& c, T&& idx) {
	// Associative container												
	size_t cSize = c.size();
	if (cSize <= idx) [[unlikely]] {
		assert_fail_debug_msg(std::format("Range check failed! (index: {}{})", idx, cSize != max_v<size_t> ? std::format(", size: {}", cSize) : ""));
	}
	auto it = std::begin(std::forward<cT>(c));
	std::advance(it, idx);
	return *it;
}
template <typename cT, typename T>
	requires requires(cT&& x, T&& y) { x.count(y); x.find(y); }
[[nodiscard]] static constexpr auto& c_at(cT&& c, T&& idx) {
	// Associative container
	const auto found = c.find(std::forward<T>(idx));
	size_t cSize = max_v<size_t>;
	if constexpr ((requires() { c.size(); }))
		cSize = c.size();
	if (found == c.end()) [[unlikely]] {
		assert_fail_debug_msg(std::format("Range check failed! (index: {}{})", idx, cSize != max_v<size_t> ? std::format(", size: {}", cSize) : ""));
	}
	return found->second;
}