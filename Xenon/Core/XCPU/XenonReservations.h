// Copyright 2025 Xenon Emulator Project

#pragma once

#include <vector>
#include <Windows.h>

#include "Base/Types.h"

struct PPU_RES
{
	u8 ppuID;
	volatile bool V;
	volatile u64 resAddr;
};

class XenonReservations
{
public:
	XenonReservations();
	virtual bool Register(PPU_RES* Res);
	void Increment(void) { InterlockedIncrement(&nReservations); }
	void Decrement(void) { InterlockedDecrement(&nReservations); }
	void Check(u64 x) { if (nReservations) Scan(x); }
	virtual void Scan(u64 PhysAddress);
	void AcquireLock(void) { EnterCriticalSection(&ReservationLock); }
	void ReleaseLock(void) { LeaveCriticalSection(&ReservationLock); }

private:
	long nReservations;
	CRITICAL_SECTION ReservationLock;
	int nProcessors;
	struct PPU_RES* Reservations[6];
};
