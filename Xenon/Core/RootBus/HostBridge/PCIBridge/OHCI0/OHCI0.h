// Copyright 2025 Xenon Emulator Project

#pragma once

#include <iostream>

#include "Core/RootBus/HostBridge/PCIBridge/PCIDevice.h"

namespace Xe
{
	namespace PCIDev
	{
		namespace OHCI0
		{
		#define OHCI0_DEV_SIZE	0x1000

			class OHCI0 : public PCIDevice
			{
			public:
				OHCI0();
				void Read(u64 readAddress, u64* data, u8 byteCount) override;
				void ConfigRead(u64 readAddress, u64* data, u8 byteCount) override;
				void Write(u64 writeAddress, u64 data, u8 byteCount) override;
				void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) override;

			private:
			};

		}
	}
}