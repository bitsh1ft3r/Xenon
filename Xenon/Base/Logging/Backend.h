// Copyright 2025 Xenon Emulator Project

#pragma once

#include <string_view>
#include "Filter.h"

namespace Base::Log {

class Filter;

/// Initializes the logging system. This should be the first thing called in main.
void Initialize(std::string_view log_file = "");

bool IsActive();

/// Starts the logging threads.
void Start();

/// Explictily stops the logger thread and flushes the buffers
void Stop();

/// The global filter will prevent any messages from even being processed if they are filtered.
void SetGlobalFilter(const Filter& filter);

void SetColorConsoleBackendEnabled(bool enabled);

} // namespace Base::Log
