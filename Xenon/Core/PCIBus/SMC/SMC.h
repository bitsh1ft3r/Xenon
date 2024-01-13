#pragma once

#include <iostream>

#include "Xenon/Base/SystemDevice.h"

#define SMC_START_ADDR	0x80000200ea001000
#define SMC_END_ADDR	0x80000200ea001100

class SMC : public SystemDevice
{
public:
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;

private:
	
};