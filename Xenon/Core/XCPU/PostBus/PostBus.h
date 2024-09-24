#pragma once

#include <Windows.h>

#include "Xenon/Base/TypeDefs.h"

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