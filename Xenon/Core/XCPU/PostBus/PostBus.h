// Copyright 2025 Xenon Emulator Project

#pragma once

#include <Windows.h>

#include "Base/Types.h"

#define POST_BUS_ADDR 0x61010

namespace Xe
{
	namespace XCPU
	{
		namespace POSTBUS
		{
			void POST(u64 postCode);
		}
	}
}