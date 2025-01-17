// Copyright 2025 Xenon Emulator Project

#include "Ethernet.h"

Xe::PCIDev::ETHERNET::ETHERNET::ETHERNET()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x580A1414;
	memcpy(&pciConfigSpace.data[0], &devID, 4);
}

void Xe::PCIDev::ETHERNET::ETHERNET::Read(u64 readAddress, u64* data, u8 byteCount)
{

}

void Xe::PCIDev::ETHERNET::ETHERNET::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void Xe::PCIDev::ETHERNET::ETHERNET::Write(u64 writeAddress, u64 data, u8 byteCount)
{
}

void Xe::PCIDev::ETHERNET::ETHERNET::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
