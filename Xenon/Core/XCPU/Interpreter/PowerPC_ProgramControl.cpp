#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

void PPCInterpreter::PPCInterpreter_bc(PPCState* hCore)
{
    B_FORM_BO_BI_BD_AA_LK;

    if (!BO_GET(2))
    {
        hCore->SPR[SPR_CTR] -= 1;
    }

    bool ctrOk = BO_GET(2) || ((hCore->SPR[SPR_CTR] != 0) ^ BO_GET(3));
    bool condOk = BO_GET(0) || (CR_GET(BI) == BO_GET(1));

    if (ctrOk && condOk)
    {
        hCore->lastJumpAddress = hCore->CIA;
        hCore->NIA = (AA ? 0 : hCore->CIA) + (EXTS(BD, 14) << 2);
    }

    if (LK)
    {
        hCore->SPR[SPR_LR] = hCore->CIA + 4;
    }
}

void PPCInterpreter::PPCInterpreter_b(PPCState* hCore)
{
    I_FORM_LI_AA_LK;
    hCore->lastJumpAddress = hCore->CIA;
    hCore->NIA = (AA ? 0 : hCore->CIA) + (EXTS(LI, 24) << 2);

    if (LK)
    {
        hCore->SPR[SPR_LR] = hCore->CIA + 4;
    }
}

void PPCInterpreter::PPCInterpreter_bcctr(PPCState* hCore)
{
	XL_FORM_BO_BI_BH_LK;

    bool condOk = BO_GET(0) || (CR_GET(BI) == BO_GET(1));

    if (condOk)
    {
        hCore->lastJumpAddress = hCore->CIA;
        hCore->NIA = hCore->SPR[SPR_CTR] & ~3;
    }

    if (LK)
    {
        hCore->SPR[SPR_LR] = hCore->CIA + 4;
    }
}

void PPCInterpreter::PPCInterpreter_bclr(PPCState* hCore)
{
    XL_FORM_BO_BI_BH_LK;

    if (!BO_GET(2))
    {
        hCore->SPR[SPR_CTR] -= 1;
    }

    bool ctrOk = BO_GET(2) || ((hCore->SPR[SPR_CTR] != 0) ^ BO_GET(3));
    bool condOk = BO_GET(0) || (CR_GET(BI) == BO_GET(1));

    if (ctrOk && condOk)
    {
        hCore->lastJumpAddress = hCore->CIA;
        hCore->NIA = hCore->SPR[SPR_LR] & ~3;
    }

    if (LK)
    {
        hCore->SPR[SPR_LR] = hCore->CIA + 4;
    }
}