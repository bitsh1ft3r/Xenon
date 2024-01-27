#pragma once

#include <iostream>

#include "Xenon/Core/PCIBus/PCIDevice.h"

#define CDROM_START_ADDR	0x200EA001200
#define CDROM_END_ADDR		0x200EA001230

class CDROM : public PCIDevice
{
public:
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void ConfigRead(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;
	void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) override;

private:
	u8 configReg[256] = { 0x14,0x14,0x02,0x58 };
};