#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

void PPCInterpreter::ppcUpdateCR(PPCState* hCore, s8 crNum, u64 input0, u64 input1) {
    u8 crFlag = 0;

    if (input0 < input1)
    {
        // LT
        crFlag |= 0b1000;
    }
    if (input0 > input1)
    {
        // GT
        crFlag |= 0b0100;
    }
    if (input0 == input1)
    {
        // EQ
        crFlag |= 0b0010;
    }

    if (hCore->XER.SO)
    {
        crFlag |= 0b0001;
    }

    switch (crNum)
    {
    case 0:
        hCore->CR.CR0 = crFlag;
        break;
    case 1:
        hCore->CR.CR1 = crFlag;
        break;
    case 2:
        hCore->CR.CR2 = crFlag;
        break;
    case 3:
        hCore->CR.CR3 = crFlag;
        break;
    case 4:
        hCore->CR.CR4 = crFlag;
        break;
    case 5:
        hCore->CR.CR5 = crFlag;
        break;
    case 6:
        hCore->CR.CR6 = crFlag;
        break;
    case 7:
        hCore->CR.CR7 = crFlag;
        break;
    }
}

u64 PPCInterpreter::ppcAddCarrying(PPCState* hCore, u64 op1, u64 op2, u64 carryBit)
{
    u32 operand1 = (u32)op1;
    u32 operand2 = (u32)op2;
    u32 carry = (u32)carryBit;

    //TODO: Carry out of bit 32 only, needs 64 bit support.

    hCore->XER.CA = 0;

    operand1 += operand2;

    if (operand1 < operand2)
    {
        hCore->XER.CA = 1;
    }

    operand1 += carry;

    if (operand1 < carry)
    {
        hCore->XER.CA = 1;
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