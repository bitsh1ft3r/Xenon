#pragma once

#include "Xenon/Base/SystemDevice.h"

#include <vector>
#include <string>

#define UART_START_ADDR	0x80000200EA001000
#define UART_END_ADDR	0x80000200EA0010FF // Is this okay??

class UART : public SystemDevice
{
public:
	UART();
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;

private:
	size_t dataSize = 0;
	std::vector<char> uartData = {};
	std::string uartDataStr = "";
};