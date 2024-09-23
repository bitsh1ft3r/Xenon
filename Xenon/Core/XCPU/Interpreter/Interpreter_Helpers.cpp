#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

void PPCInterpreter::ppcUpdateCR(PPU_STATE* hCore, s8 crNum, u32 crValue) {

    switch (crNum)
    {
    case 0:
        hCore->ppuThread[hCore->currentThread].CR.CR0 = crValue;
        break;
    case 1:
        hCore->ppuThread[hCore->currentThread].CR.CR1 = crValue;
        break;
    case 2:
        hCore->ppuThread[hCore->currentThread].CR.CR2 = crValue;
        break;
    case 3:
        hCore->ppuThread[hCore->currentThread].CR.CR3 = crValue;
        break;
    case 4:
        hCore->ppuThread[hCore->currentThread].CR.CR4 = crValue;
        break;
    case 5:
        hCore->ppuThread[hCore->currentThread].CR.CR5 = crValue;
        break;
    case 6:
        hCore->ppuThread[hCore->currentThread].CR.CR6 = crValue;
        break;
    case 7:
        hCore->ppuThread[hCore->currentThread].CR.CR7 = crValue;
        break;
    }
}

u32 PPCInterpreter::CRCompU(PPU_STATE* hCore, u64 num1, u64 num2)
{
    u32 CR = 0;

    if (num1 < num2)
        BSET(CR, 4, CR_BIT_LT);
    else if (num1 > num2)
        BSET(CR, 4, CR_BIT_GT);
    else
        BSET(CR, 4, CR_BIT_EQ);

    if (hCore->ppuThread[hCore->currentThread].SPR.XER.SO)
        BSET(CR, 4, CR_BIT_SO);

    return(CR);
}

u32 PPCInterpreter::CRCompS32(PPU_STATE* hCore, u32 num1, u32 num2)
{
    u32 CR = 0;

    if ((long)num1 < (long)num2)
        BSET(CR, 4, CR_BIT_LT);
    else if ((long)num1 > (long)num2)
        BSET(CR, 4, CR_BIT_GT);
    else
        BSET(CR, 4, CR_BIT_EQ);

    if (hCore->ppuThread[hCore->currentThread].SPR.XER.SO)
        BSET(CR, 4, CR_BIT_SO);

    return(CR);
}

u32 PPCInterpreter::CRCompS64(PPU_STATE* hCore, u64 num1, u64 num2)
{
    u32 CR = 0;

    if ((s64)num1 < (s64)num2)
        BSET(CR, 4, CR_BIT_LT);
    else if ((s64)num1 > (s64)num2)
        BSET(CR, 4, CR_BIT_GT);
    else
        BSET(CR, 4, CR_BIT_EQ);

    if (hCore->ppuThread[hCore->currentThread].SPR.XER.SO)
        BSET(CR, 4, CR_BIT_SO);

    return(CR);
}

u32 PPCInterpreter::CRCompS(PPU_STATE* hCore, u64 num1, u64 num2)
{
    if(hCore->ppuThread[hCore->currentThread].SPR.MSR.SF)
        return(CRCompS64(hCore, num1, num2));
    else
        return(CRCompS32(hCore, (u32)num1, (u32)num2));
}

void PPCInterpreter::ppcDebugLoadImageSymbols(PPU_STATE* hCore, u64 moduleNameAddress, u64 moduleInfoAddress)
{
    // Loaded module name.
    char moduleName[128];
    // Loaded module info.
    KD_SYMBOLS_INFO Kdinfo;

    mmuReadString(hCore, moduleNameAddress, moduleName, 128);
    u8 a = 0;
    Kdinfo.BaseOfDll = MMURead32(hCore, moduleInfoAddress);
    Kdinfo.ProcessId = MMURead32(hCore, moduleInfoAddress + 4);
    Kdinfo.CheckSum = MMURead32(hCore, moduleInfoAddress + 8);
    Kdinfo.SizeOfImage = MMURead32(hCore, moduleInfoAddress + 12);

    std::cout << "XCPU[" << hCore->ppuName << "]: *** DebugLoadImageSymbols ***\n\t > Loaded: "
        << moduleName << " at address 0x" << Kdinfo.BaseOfDll << " - 0x" << Kdinfo.BaseOfDll + Kdinfo.SizeOfImage << std::endl;
}

void PPCInterpreter::ppcDebugUnloadImageSymbols(PPU_STATE* hCore, u64 moduleNameAddress, u64 moduleInfoAddress)
{
}

u64 PPCInterpreter::ppcAddCarrying(PPU_STATE* hCore, u64 op1, u64 op2, u64 carryBit)
{
    u64 operand1 = (u64)op1;
    u64 operand2 = (u64)op2;
    u64 carry = (u64)carryBit;

    bool SF = hCore->ppuThread[hCore->currentThread].SPR.MSR.SF;

    hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 0;

    operand1 += operand2;

    if (SF)
    {
        if (operand1 < operand2)
        {
            hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 1;
        }
    }
    else
    {
        // 32 bit mode, reflect operation as in 32 bit systems
        if ((u32)operand1 < (u32)operand2)
        {
            hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 1;
        }
    }

    operand1 += carry;

    if (SF)
    {
        if (operand1 < carry)
        {
            hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 1;
        }
    }
    else
    {
        // 32 bit mode, reflect operation as in 32 bit systems
        if ((u32)operand1 < (u32)carry)
        {
            hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 1;
        }
    }

    return(EXTS(operand1, 32));
}

void PPCInterpreter::ppcMul64(u64 operand0, u64 operand1, u64* u64High, u64* u64Low)
{
    u64 high0 = ((u32)(((u64)(operand0)) >> 32));
    u64 low0 = (u32)operand0;
    u64 high1 = ((u32)(((u64)(operand1)) >> 32));
    u64 low1 = (u32)operand1;

    u64 high = high0 * high1;
    u64 low = low0 * low1;

    u64 high0low1 = high0 * low1;
    u64 low0high1 = low0 * high1;

    high += high0low1 >> 32; high0low1 <<= 32;
    high += low0high1 >> 32; low0high1 <<= 32;

    low += high0low1;
    high += (high0low1 > low);

    low += low0high1;
    high += (low0high1 > low);

    *u64High = high;
    *u64Low = low;
}

void PPCInterpreter::ppcMul64Signed(u64 operand0, u64 operand1, u64* u64High, u64* u64Low)
{
    u64 high, low, qwC;
    bool   fNeg;

    if ((s64)operand0 < 0)
    {
        operand0 = ~operand0;

        if ((s64)operand1 < 0)
        {
            // qwH:qwL = NEG(qw1) * NEG(qw2)
            //         = ((~qw1) + 1) * ((~qw2) + 1)
            //         = ((~qw1) * (~qw2)) + (~qw1) + (~qw2) + 1

            operand1 = ~operand1;
            qwC = operand0 + operand1 + 1;
            fNeg = false;
        }
        else
        {
            // qwH:qwL = NEG(NEG(qw1) * qw2)
            //         = NEG(((~qw1) + 1) * qw2)
            //         = NEG((~qw1) * qw2 + qw2)

            qwC = operand1;
            fNeg = true;
        }
    }
    else if ((s64)operand1 < 0)
    {
        // qwH:qwL = NEG(qw1 * NEG(qw2))
        //         = NEG(qw1 * ((~qw2) + 1))
        //         = NEG((qw1 * (~qw2)) + qw1)

        operand1 = ~operand1;
        qwC = operand0;
        fNeg = true;
    }
    else
    {
        // qwH:qwL = qw1 * qw2

        qwC = 0;
        fNeg = false;
    }

    ppcMul64(operand0, operand1, &high, &low);

    low += qwC;
    high += (qwC > low);

    if (fNeg)
    {
        low = ~low;
        high = ~high;

        if (++low == 0)
            ++high;
    }

    *u64High = high;
    *u64Low = low;
}

bool PPCInterpreter::ppcDidCarry(u64 input1, u64 input2, u64 input3)
{
    if ((input1 + input2) < input1)
    {
        return true;

    }
    if ((input1 +input2 + input3) < input3)
    {
        return true;
    }
    return false;
}
