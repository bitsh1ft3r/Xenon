#pragma once

#include <iostream>

#include "Xenon/Core/PCIBus/PCIDevice.h"

#define HDD_DEV_SIZE	0x30

class HDD : public PCIDevice
{
public:
	HDD();
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void ConfigRead(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;
	void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) override;

private:
};