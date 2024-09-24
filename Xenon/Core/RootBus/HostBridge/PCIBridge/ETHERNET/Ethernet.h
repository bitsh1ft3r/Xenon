#pragma once

#include <iostream>

#include "Xenon/Core/RootBus/HostBridge/PCIBridge/PCIDevice.h"

namespace Xe
{
	namespace PCIDev
	{
		namespace ETHERNET
		{
#define ETHERNET_DEV_SIZE	0x80

			class ETHERNET : public PCIDevice
			{
			public:
				ETHERNET();
				void Read(u64 readAddress, u64* data, u8 byteCount) override;
				void ConfigRead(u64 readAddress, u64* data, u8 byteCount) override;
				void Write(u64 writeAddress, u64 data, u8 byteCount) override;
				void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) override;

			private:
			};

		}
	}
}