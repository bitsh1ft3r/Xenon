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

struct SMC_STATE
{
	u32 reg00;
	u32 reg04;
	u32 reg08;
	u32 reg0C;
	// Offset 0x10
	u32 uartInReg;
	// Offset 0x14
	u32 uartOutReg;
	// Offset 0x18
	u32 uartStatusReg;
	// Offset 0x1C
	u32 uartConfigReg;
	u32 reg20;
	u32 reg24;
	u32 reg28;
	u32 reg2C;
	u32 reg30;
	u32 reg34;
	u32 reg38;
	u32 reg3C;
	u32 reg40;
	u32 reg44;
	u32 reg48;
	u32 reg4C;
	u32 smiIntPendingReg;
	u32 reg54;
	u32 smiIntAckReg;
	u32 smiIntEnabledReg;
	u32 reg60;
	u32 reg64;
	u32 reg68;
	u32 clockIntPendingReg;
	u32 reg70;
	u32 reg74;
	u32 reg78;
	u32 reg7C;
	// Offset 0x80
	u32 fifoWriteMsgReg;
	// Offset 0x84
	u32 fifoWriteStatusReg;
	u32 reg88;
	u32 reg8C;
	// Offset 0x90
	u32 fifoReadMsgReg;
	// Offset 0x94
	u32 fifoReadStatusReg;
	u32 reg98;
	u32 reg9C;
	u32 regA0;
	u32 regA4;
	u32 regA8;
	u32 regAC;
	u32 regB0;
	u32 regB4;
	u32 regB8;
	u32 regBC;
	u32 regC0;
	u32 regC4;
	u32 regC8;
	u32 regCC;
	u32 regD0;
	u32 regD4;
	u32 regD8;
	u32 regDC;
	u32 regE0;
	u32 regE4;
	u32 regE8;
	u32 regEC;
	u32 regF0;
	u32 regF4;
	u32 regF8;
	u32 regFC;
	// Tracks the number of times the system has written to the clock, 
	// useful for handling interrupts correctly.
	s8 clockWriteNum = 0;
};

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

	// SMC thread object
	std::thread smcThread;

	// SMC State
	SMC_STATE smcState = {};

	// SMC Device internal state.
	

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

	// SMC loop
	void smcLoop();
	void smcClockTick();

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