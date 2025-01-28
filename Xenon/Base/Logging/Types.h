// Copyright 2025 Xenon Emulator Project

#pragma once

#include "Base/Types.h"

namespace Base::Log {

/// Specifies the severity or level of detail of the log message.
enum class Level : u8 {
    Trace, ///< Extremely detailed and repetitive debugging information that is likely to
    ///< pollute logs.
    Debug,   ///< Less detailed debugging information.
    Info,    ///< Status information from important points during execution.
    Warning, ///< Minor or potential problems found during execution of a task.
    Error,   ///< Major problems found during execution of a task that prevent it from being
    ///< completed.
    Critical, ///< Major problems during execution that threaten the stability of the entire
    ///< application.

    Count, ///< Total number of logging levels
};

/**
 * Specifies the sub-system that generated the log message.
 *
 * @note If you add a new entry here, also add a corresponding one to `ALL_LOG_CLASSES` in
 * filter.cpp.
 */
enum class Class : u8 {
    Log,                   ///< Messages about the log system itself
    Base,                  ///< Library routines
    Base_Filesystem,       ///< Filesystem interface library
    Core,                  ///< LLE emulation core
    Config,                ///< Emulator configuration (including commandline)
    Debug,                 ///< Debugging tools
    Kernel,                ///< The HLE implementation of the PS4 kernel.
    Lib,                   ///< HLE implementation of system library. Each major library
    Frontend,              ///< Emulator UI
    Render,                ///< Video Core
    Render_Vulkan,         ///< Vulkan backend
    Render_Recompiler,     ///< Shader recompiler
    Loader,                ///< ROM loader
    Count                  ///< Total number of logging classes
};

} // namespace Base::Log
