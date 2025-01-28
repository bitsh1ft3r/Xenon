// Copyright 2025 Xenon Emulator Project

#include "Arch.h"
#include "Assert.h"
#include "Logging/Backend.h"

#if defined(ARCH_X86_64)
#define Crash() __asm__ __volatile__("int $3")
#elif defined(ARCH_ARM64)
#define Crash() __asm__ __volatile__("brk 0")
#else
#error "Missing Crash() implementation for target CPU architecture."
#endif

void assert_fail_impl() {
    Base::Log::Stop();
    std::fflush(stdout);
    Crash();
}

[[noreturn]] void unreachable_impl() {
    Base::Log::Stop();
    std::fflush(stdout);
    Crash();
    throw std::runtime_error("Unreachable code");
}

void assert_fail_debug_msg(const char* msg) {
    LOG_CRITICAL(Debug, "Assertion Failed!\n{}", msg);
    assert_fail_impl();
}
