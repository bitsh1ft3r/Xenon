#include "XMA.h"

XMA::XMA()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x58011414;
	memcpy(&configReg[0], &devID, 4);
}

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
