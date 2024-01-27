#pragma once

#include <iostream>

#include "Xenon/Core/PCIBus/PCIDevice.h"

#define XMA_START_ADDR	0x200EA001800
#define XMA_END_ADDR	0x200EA001C00

class XMA : public PCIDevice
{
public:
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void ConfigRead(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;
	void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) override;

private:
	u8 configReg[256] = { 0x14,0x14,0x01,0x58 };
};