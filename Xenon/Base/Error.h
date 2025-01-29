// Copyright 2025 Xenon Emulator Project

#pragma once

#include <string>

namespace Base {

// Generic function to get last error message.
// Call directly after the command or use the error num.
// This function might change the error code.
// Defined in error.cpp.
[[nodiscard]] std::string GetLastErrorMsg();

// Like GetLastErrorMsg(), but passing an explicit error code.
// Defined in error.cpp.
[[nodiscard]] std::string NativeErrorToString(int e);

} // namespace Base
