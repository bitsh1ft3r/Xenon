// Copyright 2025 Xenon Emulator Project

#include "XenonReservations.h"

XenonReservations::XenonReservations()
{
	InitializeCriticalSectionAndSpinCount(&ReservationLock, 40000);
	nReservations = 0;
	nProcessors = 0;
	Reservations[0] = {0};
}

bool XenonReservations::Register(PPU_RES* Res)
{
	AcquireLock();
	Reservations[nProcessors] = Res;
	nProcessors++;
	ReleaseLock();
	return true;
}

void XenonReservations::Scan(u64 PhysAddress)
{
	PhysAddress &= ~3;

	AcquireLock();
	for (int i = 0; i < nProcessors; i++)
	{
		// NB: order of checks matters!
		if ((Reservations[i]->V) &&
			PhysAddress >= Reservations[i]->resAddr)
		{
			Reservations[i]->V = false;
			Decrement();
		}
	}
	ReleaseLock();
}
