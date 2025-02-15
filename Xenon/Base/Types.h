// Copyright 2025 Xenon Emulator Project

#pragma once

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

#ifndef _MSC_VER
using u128 = __uint128_t;
using s128 = __int128_t;
#else

extern "C"
{
	union __m128;
	union __m128i;
	struct __m128d;

	unsigned char _addcarry_u64(unsigned char, u64, u64, u64*);
	unsigned char _subborrow_u64(unsigned char, u64, u64, u64*);
	u64 __shiftleft128(u64, u64, unsigned char);
	u64 __shiftright128(u64, u64, unsigned char);
	u64 _umul128(u64, u64, u64*);
}

// Unsigned 128-bit integer implementation.
struct alignas(16) u128
{
	u64 lo, hi;

	u128() noexcept = default;

	template <typename T, std::enable_if_t<std::is_unsigned_v<T>, u64> = 0>
	constexpr u128(T arg) noexcept
		: lo(arg)
		, hi(0)
	{
	}

	template <typename T, std::enable_if_t<std::is_signed_v<T>, s64> = 0>
	constexpr u128(T arg) noexcept
		: lo(s64{ arg })
		, hi(s64{ arg } >> 63)
	{
	}

	constexpr explicit operator bool() const noexcept
	{
		return !!(lo | hi);
	}

	constexpr explicit operator u64() const noexcept
	{
		return lo;
	}

	constexpr explicit operator s64() const noexcept
	{
		return lo;
	}

	constexpr friend u128 operator+(const u128& l, const u128& r)
	{
		u128 value = l;
		value += r;
		return value;
	}

	constexpr friend u128 operator-(const u128& l, const u128& r)
	{
		u128 value = l;
		value -= r;
		return value;
	}

	constexpr friend u128 operator*(const u128& l, const u128& r)
	{
		u128 value = l;
		value *= r;
		return value;
	}

	constexpr u128 operator+() const
	{
		return *this;
	}

	constexpr u128 operator-() const
	{
		u128 value{};
		value -= *this;
		return value;
	}

	constexpr u128& operator++()
	{
		*this += 1;
		return *this;
	}

	constexpr u128 operator++(int)
	{
		u128 value = *this;
		*this += 1;
		return value;
	}

	constexpr u128& operator--()
	{
		*this -= 1;
		return *this;
	}

	constexpr u128 operator--(int)
	{
		u128 value = *this;
		*this -= 1;
		return value;
	}

	constexpr u128 operator<<(u128 shift_value) const
	{
		u128 value = *this;
		value <<= shift_value;
		return value;
	}

	constexpr u128 operator>>(u128 shift_value) const
	{
		u128 value = *this;
		value >>= shift_value;
		return value;
	}

	constexpr u128 operator~() const
	{
		u128 value{};
		value.lo = ~lo;
		value.hi = ~hi;
		return value;
	}

	constexpr friend u128 operator&(const u128& l, const u128& r)
	{
		u128 value{};
		value.lo = l.lo & r.lo;
		value.hi = l.hi & r.hi;
		return value;
	}

	constexpr friend u128 operator|(const u128& l, const u128& r)
	{
		u128 value{};
		value.lo = l.lo | r.lo;
		value.hi = l.hi | r.hi;
		return value;
	}

	constexpr friend u128 operator^(const u128& l, const u128& r)
	{
		u128 value{};
		value.lo = l.lo ^ r.lo;
		value.hi = l.hi ^ r.hi;
		return value;
	}

	constexpr u128& operator+=(const u128& r)
	{
		if (std::is_constant_evaluated())
		{
			lo += r.lo;
			hi += r.hi + (lo < r.lo);
		}
		else
		{
			_addcarry_u64(_addcarry_u64(0, r.lo, lo, &lo), r.hi, hi, &hi);
		}

		return *this;
	}

	constexpr u128& operator-=(const u128& r)
	{
		if (std::is_constant_evaluated())
		{
			hi -= r.hi + (lo < r.lo);
			lo -= r.lo;
		}
		else
		{
			_subborrow_u64(_subborrow_u64(0, lo, r.lo, &lo), hi, r.hi, &hi);
		}

		return *this;
	}

	constexpr u128& operator*=(const u128& r)
	{
		const u64 _hi = r.hi * lo + r.lo * hi;

		if (std::is_constant_evaluated())
		{
			hi = (lo >> 32) * (r.lo >> 32) + (((lo >> 32) * (r.lo & 0xffffffff)) >> 32) + (((r.lo >> 32) * (lo & 0xffffffff)) >> 32);
			lo = lo * r.lo;
		}
		else
		{
			lo = _umul128(lo, r.lo, &hi);
		}

		hi += _hi;
		return *this;
	}

	constexpr u128& operator<<=(const u128& r)
	{
		if (std::is_constant_evaluated())
		{
			if (r.hi == 0 && r.lo < 64)
			{
				hi = (hi << r.lo) | (lo >> (64 - r.lo));
				lo = (lo << r.lo);
				return *this;
			}
			else if (r.hi == 0 && r.lo < 128)
			{
				hi = (lo << (r.lo - 64));
				lo = 0;
				return *this;
			}
		}

		const u64 v0 = lo << (r.lo & 63);
		const u64 v1 = __shiftleft128(lo, hi, static_cast<unsigned char>(r.lo));
		lo = (r.lo & 64) ? 0 : v0;
		hi = (r.lo & 64) ? v0 : v1;
		return *this;
	}

	constexpr u128& operator>>=(const u128& r)
	{
		if (std::is_constant_evaluated())
		{
			if (r.hi == 0 && r.lo < 64)
			{
				lo = (lo >> r.lo) | (hi << (64 - r.lo));
				hi = (hi >> r.lo);
				return *this;
			}
			else if (r.hi == 0 && r.lo < 128)
			{
				lo = (hi >> (r.lo - 64));
				hi = 0;
				return *this;
			}
		}

		const u64 v0 = hi >> (r.lo & 63);
		const u64 v1 = __shiftright128(lo, hi, static_cast<unsigned char>(r.lo));
		lo = (r.lo & 64) ? v0 : v1;
		hi = (r.lo & 64) ? 0 : v0;
		return *this;
	}

	constexpr u128& operator&=(const u128& r)
	{
		lo &= r.lo;
		hi &= r.hi;
		return *this;
	}

	constexpr u128& operator|=(const u128& r)
	{
		lo |= r.lo;
		hi |= r.hi;
		return *this;
	}

	constexpr u128& operator^=(const u128& r)
	{
		lo ^= r.lo;
		hi ^= r.hi;
		return *this;
	}
};

// Signed 128-bit integer implementation
struct s128 : u128
{
	using u128::u128;

	constexpr s128 operator>>(u128 shift_value) const
	{
		s128 value = *this;
		value >>= shift_value;
		return value;
	}

	constexpr s128& operator>>=(const u128& r)
	{
		if (std::is_constant_evaluated())
		{
			if (r.hi == 0 && r.lo < 64)
			{
				lo = (lo >> r.lo) | (hi << (64 - r.lo));
				hi = (static_cast<s64>(hi) >> r.lo);
				return *this;
			}
			else if (r.hi == 0 && r.lo < 128)
			{
				s64 _lo = static_cast<s64>(hi) >> (r.lo - 64);
				lo = _lo;
				hi = _lo >> 63;
				return *this;
			}
		}

		const u64 v0 = static_cast<s64>(hi) >> (r.lo & 63);
		const u64 v1 = __shiftright128(lo, hi, static_cast<unsigned char>(r.lo));
		lo = (r.lo & 64) ? v0 : v1;
		hi = (r.lo & 64) ? static_cast<s64>(hi) >> 63 : v0;
		return *this;
	}
};
#endif
