#pragma once

#include <Windows.h>

#include "Xenon/Base/SystemDevice.h"

#define POST_BUS_ADDR 0x20000061010

class PostBus : public SystemDevice
{
public:
	PostBus();
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;

private:
	// Text colored output (for now)
	HANDLE hConsole;

	u64 lastBusData = 0;
	void POST(u64 data);
};