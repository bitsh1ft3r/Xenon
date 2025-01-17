// Copyright 2025 Xenon Emulator Project

#pragma once

#include <iostream>

#include "Core/RootBus/HostBridge/PCIBridge/PCIDevice.h"

namespace Xe
{
	namespace PCIDev
	{
		namespace AUDIOCTRLR
		{
#define AUDIO_CTRLR_DEV_SIZE	0x40

			class AUDIOCTRLR : public PCIDevice
			{
			public:
				AUDIOCTRLR();
				void Read(u64 readAddress, u64* data, u8 byteCount) override;
				void ConfigRead(u64 readAddress, u64* data, u8 byteCount) override;
				void Write(u64 writeAddress, u64 data, u8 byteCount) override;
				void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) override;

			private:
			};

		}
	}
}