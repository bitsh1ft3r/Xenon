#include "UART.h"

#include<iostream>
#include<memory.h>

UART::UART()
{
}

void UART::Read(u64 readAddress, u64* data, u8 byteCount)
{
	u64 writtenData = 0xF;
	memcpy(data, &writtenData, byteCount);
}

void UART::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	u32 writtenData = 0x0;
	memcpy(&writtenData, &data, byteCount);

	
	if ((char)writtenData == '\r')
	{
		// End of line, output string and reset counter
		std::cout << "UART: " << uartDataStr.c_str() << std::endl;
		//dataSize = 0;
		uartData.resize(0);
	}
	else {
		uartDataStr.push_back((char)writtenData);
	}
}
