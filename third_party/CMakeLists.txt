# SPDX-FileCopyrightText: Copyright 2025 Xenon Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set_directory_properties(PROPERTIES
    EXCLUDE_FROM_ALL ON
    SYSTEM ON
)

# Set CMP0069 policy to "NEW" in order to ensure consistent behavior when building external targets with LTO enabled
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

# SDL
if (NOT TARGET SDL2::SDL2)
    add_subdirectory(SDL2)
endif()