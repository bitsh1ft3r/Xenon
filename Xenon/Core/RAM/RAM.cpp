#include <string.h>
#include <iostream>

#include "RAM.h"

RAM::RAM()
{
	memset(RAMData, 0, sizeof(RAMData));
}

void RAM::Read(u64 readAddress, u64* data, u8 byteCount)
{
	u64 offset = (u32)(readAddress - RAM_START_ADDR); 
	memcpy(data, &RAMData[offset], byteCount);
}

void RAM::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	u64 offset = (u32)(writeAddress - RAM_START_ADDR);
	memcpy(&RAMData[offset], &data, byteCount);
}
