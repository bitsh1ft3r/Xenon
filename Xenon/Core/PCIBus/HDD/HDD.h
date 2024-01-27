#pragma once

#include <iostream>

#include "Xenon/Core/PCIBus/PCIDevice.h"

#define HDD_START_ADDR	0x200EA001300
#define HDD_END_ADDR	0x200EA001330

class HDD : public PCIDevice
{
public:
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void ConfigRead(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;
	void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) override;

private:
	u8 configReg[256] = { 0x14,0x14,0x03,0x58 };
};