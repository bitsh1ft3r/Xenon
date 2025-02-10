// Copyright 2025 Xenon Emulator Project

#pragma once

#include "Logging/Log.h"

// Sometimes we want to try to continue even after hitting an assert.
// However touching this file yields a global recompilation as this header is included almost
// everywhere. So let's just move the handling of the failed assert to a single cpp file.

void assert_fail_impl();
[[noreturn]] void unreachable_impl();
void assert_fail_debug_msg(const std::string& msg);

#ifdef _MSC_VER
#define XENON_NO_INLINE __declspec(noinline)
#else
#define XENON_NO_INLINE __attribute__((noinline))
#endif

#define ASSERT(_a_)                                                                                \
    ([&]() XENON_NO_INLINE {                                                                        \
        if (!(_a_)) [[unlikely]] {                                                                 \
            LOG_CRITICAL(Debug, "Assertion Failed!");                                              \
            assert_fail_impl();                                                                    \
        }                                                                                          \
    }())

#define ASSERT_MSG(_a_, ...)                                                                       \
    ([&]() XENON_NO_INLINE {                                                                        \
        if (!(_a_)) [[unlikely]] {                                                                 \
            LOG_CRITICAL(Debug, "Assertion Failed!\n" __VA_ARGS__);                                \
            assert_fail_impl();                                                                    \
        }                                                                                          \
    }())

#define UNREACHABLE()                                                                              \
    do {                                                                                           \
        LOG_CRITICAL(Debug, "Unreachable code!");                                                  \
        unreachable_impl();                                                                        \
    } while (0)

#define UNREACHABLE_MSG(...)                                                                       \
    do {                                                                                           \
        LOG_CRITICAL(Debug, "Unreachable code!\n" __VA_ARGS__);                                    \
        unreachable_impl();                                                                        \
    } while (0)

#ifdef _DEBUG
#define DEBUG_ASSERT(_a_) ASSERT(_a_)
#define DEBUG_ASSERT_MSG(_a_, ...) ASSERT_MSG(_a_, __VA_ARGS__)
#else // not debug
#define DEBUG_ASSERT(_a_)                                                                          \
    do {                                                                                           \
    } while (0)
#define DEBUG_ASSERT_MSG(_a_, _desc_, ...)                                                         \
    do {                                                                                           \
    } while (0)
#endif

#define UNIMPLEMENTED() ASSERT_MSG(false, "Unimplemented code!")
#define UNIMPLEMENTED_MSG(...) ASSERT_MSG(false, __VA_ARGS__)

#define UNIMPLEMENTED_IF(cond) ASSERT_MSG(!(cond), "Unimplemented code!")
#define UNIMPLEMENTED_IF_MSG(cond, ...) ASSERT_MSG(!(cond), __VA_ARGS__)

// If the assert is ignored, execute _b_
#define ASSERT_OR_EXECUTE(_a_, _b_)                                                                \
    do {                                                                                           \
        ASSERT(_a_);                                                                               \
        if (!(_a_)) [[unlikely]] {                                                                 \
            _b_                                                                                    \
        }                                                                                          \
    } while (0)

// If the assert is ignored, execute _b_
#define ASSERT_OR_EXECUTE_MSG(_a_, _b_, ...)                                                       \
    do {                                                                                           \
        ASSERT_MSG(_a_, __VA_ARGS__);                                                              \
        if (!(_a_)) [[unlikely]] {                                                                 \
            _b_                                                                                    \
        }                                                                                          \
    } while (0)
