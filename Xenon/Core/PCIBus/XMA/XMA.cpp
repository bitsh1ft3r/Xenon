#include "XMA.h"

void XMA::Read(u64 readAddress, u64* data, u8 byteCount)
{

}

void XMA::Write(u64 writeAddress, u64 data, u8 byteCount)
{

}

void XMA::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(&data, &configReg[static_cast<u8>(readAddress)], byteCount);
}

void XMA::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&configReg[static_cast<u8>(writeAddress)], &data, byteCount);
}
