// Copyright 2025 Xenon Emulator Project

#include <string.h>
#include <iostream>

#include "RAM.h"

/***Sets the destination, value (205) and size (RAMData)***/
RAM::RAM()
{
	memset(RAMData, 0xcd, RAM_SIZE);
}

/*****************Responsible for RAM reading*****************/
void RAM::Read(u64 readAddress, u64* data, u8 byteCount)
{
	const u64 offset = (u32)(readAddress - RAM_START_ADDR); 
	memcpy(data, &RAMData[offset], byteCount);
}

/******************Responsible for RAM writing*****************/
void RAM::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	const u64 offset = (u32)(writeAddress - RAM_START_ADDR);
	memcpy(&RAMData[offset], &data, byteCount);
}

u8* RAM::getPointerToAddress(u32 address)
{
	const u64 offset = (u32)(address - RAM_START_ADDR);
	return RAMData + offset;
}
