#include "EHCI1.h"

Xe::PCIDev::EHCI1::EHCI1::EHCI1()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x58071414;
	memcpy(&pciConfigSpace.data[0], &devID, 4);
}

void Xe::PCIDev::EHCI1::EHCI1::Read(u64 readAddress, u64* data, u8 byteCount)
{

}

void Xe::PCIDev::EHCI1::EHCI1::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void Xe::PCIDev::EHCI1::EHCI1::Write(u64 writeAddress, u64 data, u8 byteCount)
{
}

void Xe::PCIDev::EHCI1::EHCI1::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}