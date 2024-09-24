#pragma once

#include <thread>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

#include "Xenon/Core/RootBus/HostBridge/PCIBridge/PCIDevice.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/PCIBridge.h"

#define SMC_DEV_SIZE	0x100

class SMC : public PCIDevice
{
public:
	SMC(PCIBridge *parentPCIBridge);
	void Read(u64 readAddress, u64* data, u8 byteCount) override;
	void ConfigRead(u64 readAddress, u64* data, u8 byteCount) override;
	void Write(u64 writeAddress, u64 data, u8 byteCount) override;
	void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) override;

private:
	// PCI Bridge pointer. Used for Interrupts.
	PCIBridge* parentBus;
	void smcProcessRead(u64 readAddress, u64* data, u8 byteCount);
	void smcProcessWrite(u64 writeAddress, u64 data, u8 byteCount);

	//
	// FIFO Queue
	//
	void smcFIFOProcessRead(u64 readAddress, u64* data, u8 byteCount);
	void smcFIFOProcessWrite(u64 writeAddress, u64 data, u8 byteCount);
	u8 fifoStatusReg = 0x4;

	u8 fifoWrittenMsg[16] = { 0 };
	u8 currentWritePos = 0;

	u8 fifoReadedMsg[16] = { 0 };
	u8 currentReadPos = 0;

	//
	// UART stuff.
	//
	FILE* uartOutput;
	void uartWrite(u64 writeAddress, u64 data, u8 byteCount);
	void uartRead(u64 readAddress, u64* data, u8 byteCount);

	// UART Status Register.
	u32 uartStatus = 0x2;

	// SMC @ addr 0x50 must be 1 when an smm interrupt ocurrs, int pending flag??
	u32 intPending = 0;

	// Character buffer for UART output.
	std::string uartDataStr = "";

	void initComPort();

	bool comPortInitialized = false;
	DCB comPortDCB;
	HANDLE comPortHandle;
	DWORD byteswritten;
	bool retVal;
	OVERLAPPED comOvEvent = { 0 };
	bool waitingOnStatusChange = false;
	DWORD comEvent;
	COMSTAT comStat;
	DWORD comErr;
};