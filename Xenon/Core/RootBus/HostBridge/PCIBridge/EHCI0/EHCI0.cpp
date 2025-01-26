// Copyright 2025 Xenon Emulator Project

#include "EHCI0.h"

Xe::PCIDev::EHCI0::EHCI0::EHCI0()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x58051414;
	memcpy(&pciConfigSpace.data[0], &devID, 4);
}

void Xe::PCIDev::EHCI0::EHCI0::Read(u64 readAddress, u64* data, u8 byteCount)
{

}

void Xe::PCIDev::EHCI0::EHCI0::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void Xe::PCIDev::EHCI0::EHCI0::Write(u64 writeAddress, u64 data, u8 byteCount)
{
}

void Xe::PCIDev::EHCI0::EHCI0::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
