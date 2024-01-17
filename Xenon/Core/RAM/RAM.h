#pragma once

#include "Xenon/Base/SystemDevice.h"

#define RAM_START_ADDR	0x0
#define RAM_SIZE		0x20000000

class RAM : public SystemDevice
{
public:
	RAM();
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;

private:
	u8* RAMData = new u8[RAM_SIZE];
};