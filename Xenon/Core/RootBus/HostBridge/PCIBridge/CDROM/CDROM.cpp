/*
	Xenon ATAPI Emulation. 
*/

#include "CDROM.h"

CDROM::CDROM()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x58021414;
	memcpy(&pciConfigSpace.data[0], &devID, 4);
}

void CDROM::Read(u64 readAddress, u64* data, u8 byteCount)
{

}

void CDROM::Write(u64 writeAddress, u64 data, u8 byteCount)
{

}

void CDROM::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void CDROM::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
