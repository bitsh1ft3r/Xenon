// Copyright 2025 Xenon Emulator Project

#include "PPCInterpreter.h"

void PPCInterpreter::PPCInterpreter_bc(PPU_STATE* hCore)
{
    B_FORM_BO_BI_BD_AA_LK;

    if (!BO_GET(2))
    {
        hCore->ppuThread[hCore->currentThread].SPR.CTR -= 1;
    }

    bool ctrOk = BO_GET(2) | ((hCore->ppuThread[hCore->currentThread].SPR.CTR != 0) ^ BO_GET(3));
    bool condOk = BO_GET(0) || (CR_GET(BI) == BO_GET(1));

    if (ctrOk && condOk)
    {
        hCore->ppuThread[hCore->currentThread].NIA = (AA ? 0 : hCore->ppuThread[hCore->currentThread].CIA) + (EXTS(BD, 14) << 2);
    }

    if (LK)
    {
        hCore->ppuThread[hCore->currentThread].SPR.LR = hCore->ppuThread[hCore->currentThread].CIA + 4;
    }
}

void PPCInterpreter::PPCInterpreter_b(PPU_STATE* hCore)
{
    I_FORM_LI_AA_LK;
    hCore->ppuThread[hCore->currentThread].NIA = (AA ? 0 : hCore->ppuThread[hCore->currentThread].CIA) + (EXTS(LI, 24) << 2);

    if (LK)
    {
        hCore->ppuThread[hCore->currentThread].SPR.LR = hCore->ppuThread[hCore->currentThread].CIA + 4;
    }
}

void PPCInterpreter::PPCInterpreter_bcctr(PPU_STATE* hCore)
{
	XL_FORM_BO_BI_BH_LK;

    bool condOk = BO_GET(0) || (CR_GET(BI) == BO_GET(1));

    if (condOk)
    {
        hCore->ppuThread[hCore->currentThread].NIA = hCore->ppuThread[hCore->currentThread].SPR.CTR & ~3;
    }

    if (LK)
    {
        hCore->ppuThread[hCore->currentThread].SPR.LR = hCore->ppuThread[hCore->currentThread].CIA + 4;
    }
}

void PPCInterpreter::PPCInterpreter_bclr(PPU_STATE* hCore)
{
    XL_FORM_BO_BI_BH_LK;

    if (!BO_GET(2))
    {
        hCore->ppuThread[hCore->currentThread].SPR.CTR -= 1;
    }

    bool ctrOk = BO_GET(2) | ((hCore->ppuThread[hCore->currentThread].SPR.CTR != 0) ^ BO_GET(3));
    bool condOk = BO_GET(0) || (CR_GET(BI) == BO_GET(1));

    // Jrunner XDK build offsets are 0x0000000003003f48 AND 0x0000000003003fdc
    // xell version are 0x0000000003003dc0 AND 0x0000000003003e54

    // HW_INIT Skip on XDK 17.489.0
    if (hCore->ppuThread[hCore->currentThread].CIA == 0x0000000003003f48)
       condOk = false;

    if (hCore->ppuThread[hCore->currentThread].CIA == 0x0000000003003fdc)
       condOk = true;

    if (ctrOk && condOk)
    {
        hCore->ppuThread[hCore->currentThread].NIA = hCore->ppuThread[hCore->currentThread].SPR.LR & ~3;
    }

    if (LK)
    {
        hCore->ppuThread[hCore->currentThread].SPR.LR = hCore->ppuThread[hCore->currentThread].CIA + 4;
    }
}