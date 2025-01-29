// Copyright 2025 Xenon Emulator Project

#include <array>
#include <cstdio>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "Base/Assert.h"
#include "Filter.h"
#include "Log.h"
#include "Log_entry.h"
#include "Text_formatter.h"

namespace Base::Log {

std::string FormatLogMessage(const Entry& entry) {
    const u32 time_seconds = static_cast<u32>(entry.timestamp.count() / 1000000);
    const u32 time_fractional = static_cast<u32>(entry.timestamp.count() % 1000000);

    const char* class_name = GetLogClassName(entry.log_class);
    const char* level_name = GetLevelName(entry.log_level);

#ifndef _DEBUG
    return fmt::format("[{}] <{}> {}", class_name, level_name, entry.message);
#else
    return fmt::format("[{}] <{}> {}:{}:{}: {}", class_name, level_name, entry.filename,
        entry.function, entry.line_num, entry.message);
#endif // !_DEBUG
}

void PrintMessage(const Entry& entry) {
    const auto str = FormatLogMessage(entry).append(1, '\n');
    fputs(str.c_str(), stdout);
}

void PrintColoredMessage(const Entry& entry) {
#ifdef _WIN32
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    if (console_handle == INVALID_HANDLE_VALUE) {
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO original_info{};
    GetConsoleScreenBufferInfo(console_handle, &original_info);

    WORD color = 0;
    switch (entry.log_level) {
    case Level::Trace: // Grey
        color = FOREGROUND_INTENSITY;
        break;
    case Level::Debug: // Cyan
        color = FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case Level::Info: // Bright gray
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        break;
    case Level::Warning: // Bright yellow
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case Level::Error: // Bright red
        color = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case Level::Critical: // Bright magenta
        color = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case Level::Guest: // Bright green
        color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case Level::Count:
        UNREACHABLE();
    }

    SetConsoleTextAttribute(console_handle, color);
#else
#define ESC "\x1b"
    const char* color = "";
    switch (entry.log_level) {
    case Level::Trace: // Grey
        color = ESC "[1;30m";
        break;
    case Level::Debug: // Cyan
        color = ESC "[0;36m";
        break;
    case Level::Info: // Bright gray
        color = ESC "[0;37m";
        break;
    case Level::Warning: // Bright yellow
        color = ESC "[1;33m";
        break;
    case Level::Error: // Bright red
        color = ESC "[1;31m";
        break;
    case Level::Critical: // Bright magenta
        color = ESC "[1;35m";
        break;
    case Level::Guest: // Green
        color = ESC "[0;92m";
        break;
    case Level::Count:
        UNREACHABLE();
    }

    fputs(color, stdout);
#endif

    PrintMessage(entry);

#ifdef _WIN32
    SetConsoleTextAttribute(console_handle, original_info.wAttributes);
#else
    fputs(ESC "[0m", stdout);
#undef ESC
#endif
}

} // namespace Base::Log
