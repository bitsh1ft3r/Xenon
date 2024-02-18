#include "HDD.h"

HDD::HDD()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x58031414;
	memcpy(&configReg[0], &devID, 4);
}

void HDD::Read(u64 readAddress, u64* data, u8 byteCount)
{

}

void HDD::Write(u64 writeAddress, u64 data, u8 byteCount)
{

}

void HDD::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &configReg[static_cast<u8>(readAddress)], byteCount);
}

void HDD::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&configReg[static_cast<u8>(writeAddress)], &data, byteCount);
}
