// Copyright 2025 Xenon Emulator Project

#include <stdlib.h>
#include <iostream>

#include "PPCInterpreter.h"

#define GPR(x)	hCore->ppuThread[hCore->currentThread].GPR[x]
#define XER_SET_CA(v)	hCore->ppuThread[hCore->currentThread].SPR.XER.CA = v

void PPCInterpreter::PPCInterpreter_addx(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	GPR(rD) = GPR(rA) + GPR(rB);


	if (OE)
	{
		std::cout << "PPC Interpreter< ADDX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_addex(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	GPR(rD) = ppcAddCarrying(hCore, GPR(rA), GPR(rB),
		hCore->ppuThread[hCore->currentThread].SPR.XER.CA);

	if (OE)
	{
		std::cout << "PPC Interpreter< ADDEX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

/* Add Immediate */
void PPCInterpreter::PPCInterpreter_addi(PPU_STATE* hCore)
{
	D_FORM_rD_rA_SI;
	SI = EXTS(SI, 16);

	GPR(rD) = (rA ? GPR(rA) : 0) + SI;
}

void PPCInterpreter::PPCInterpreter_addic(PPU_STATE* hCore)
{
	D_FORM_rD_rA_SI; 
	SI = EXTS(SI, 16);
	GPR(rD) = ppcAddCarrying(hCore, GPR(rA), SI, 0);
}

void PPCInterpreter::PPCInterpreter_addic_rc(PPU_STATE* hCore)
{
	D_FORM_rD_rA_SI;
	SI = EXTS(SI, 16);

	GPR(rD) = ppcAddCarrying(hCore, GPR(rA), SI, 0);
	u32 CR = CRCompS(hCore, GPR(rD), 0);
	ppcUpdateCR(hCore, 0, CR);
}

void PPCInterpreter::PPCInterpreter_addis(PPU_STATE* hCore)
{
	D_FORM_rD_rA_SI; 
	SI = EXTS(SI, 16);

	GPR(rD) = (rA ? GPR(rA) : 0) + (SI << 16);
}

void PPCInterpreter::PPCInterpreter_addzex(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_OE_RC;

	GPR(rD) = ppcAddCarrying(hCore, GPR(rA),
		0, hCore->ppuThread[hCore->currentThread].SPR.XER.CA);

	if (OE)
	{
		std::cout << "PPC Interpreter: ADDZEX -> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_and(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	GPR(rA) = GPR(rS) & GPR(rB);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_andc(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	GPR(rA) = GPR(rS) & ~GPR(rB);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_andi(PPU_STATE* hCore)
{
	D_FORM_rS_rA_UI;

	GPR(rA) = GPR(rS) & UI;
	u32 CR = CRCompS(hCore, GPR(rA), 0);
	ppcUpdateCR(hCore, 0, CR);
}

void PPCInterpreter::PPCInterpreter_andis(PPU_STATE* hCore)
{
	D_FORM_rS_rA_UI;

	GPR(rA) = GPR(rS) & (UI << 16);
	ppcUpdateCR(hCore, 0, static_cast<u32>(GPR(rA)));
}

void PPCInterpreter::PPCInterpreter_cmp(PPU_STATE* hCore)
{
	X_FORM_BF_L_rA_rB;

	u32 CR;

	if (L)
	{
		CR = CRCompS64(hCore, GPR(rA), GPR(rB));
	}
	else
	{
		CR = CRCompS32(hCore, static_cast<s32>(GPR(rA)), 
			static_cast<s32>(GPR(rB)));
	}

	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_cmpi(PPU_STATE* hCore)
{
	D_FORM_BF_L_rA_SI; 
	SI = EXTS(SI, 16);

	u32 CR;

	if (L)
	{
		CR = CRCompS64(hCore, GPR(rA), SI);
	}
	else
	{
		CR = CRCompS32(hCore, (s32)GPR(rA), (s32)SI);
	}

	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_cmpl(PPU_STATE* hCore)
{
	X_FORM_BF_L_rA_rB;

	u32 CR;

	if (L)
	{
		CR = CRCompU(hCore, GPR(rA), GPR(rB));
	}
	else
	{
		CR = CRCompU(hCore, (u32)GPR(rA), (u32)GPR(rB));
	}

	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_cmpli(PPU_STATE* hCore)
{
	D_FORM_BF_L_rA_UI;

	u32 CR;

	if (L)
	{
		CR = CRCompU(hCore, GPR(rA), UI);
	}
	else
	{
		CR = CRCompU(hCore, (u32)GPR(rA), UI);
	}

	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_cntlzd(PPU_STATE* hCore)
{
	X_FORM_rS_rA_RC;

	u64 RegS = GPR(rS);
	u64 Mask = 0x8000000000000000;
	u64 regA = 0;

	for (; Mask && (RegS & Mask) == 0; Mask >>= 1)
	{
		regA += 1;
	}

	GPR(rA) = regA;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_cntlzw(PPU_STATE* hCore)
{
	X_FORM_rS_rA_RC;

	u32 RegS = (u32)GPR(rS);
	u32 Mask = (1u << 31);
	u32 regA = 0;

	for (; Mask && (RegS & Mask) == 0; Mask >>= 1)
	{
		regA += 1;
	}

	GPR(rA) = regA;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}
/* CR And */
void PPCInterpreter::PPCInterpreter_crand(PPU_STATE* hCore)
{
	XL_FORM_BT_BA_BB;

	const u32 a = CR_GET(BA);
	const u32 b = CR_GET(BB);

	const u32 crAnd = a & b;

	if (crAnd & 1)
		BSET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	else
		BCLR(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
}
/* CR And Carrying */
void PPCInterpreter::PPCInterpreter_crandc(PPU_STATE* hCore)
{
	XL_FORM_BT_BA_BB;

	const u32 a = CR_GET(BA);
	const u32 b = CR_GET(BB);

	const u32 crAndc = a & (1 ^ b);

	if (crAndc & 1)
		BSET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	else
		BCLR(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
}
/* CR Equivalent */
void PPCInterpreter::PPCInterpreter_creqv(PPU_STATE* hCore)
{
	XL_FORM_BT_BA_BB;

	const u32 a = CR_GET(BA);
	const u32 b = CR_GET(BB);

	const u32 crEqv = 1 ^ (a ^ b);

	if (crEqv & 1)
		BSET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	else
		BCLR(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
}
/* CR Nand */
void PPCInterpreter::PPCInterpreter_crnand(PPU_STATE* hCore)
{
	XL_FORM_BT_BA_BB;

	const u32 a = CR_GET(BA);
	const u32 b = CR_GET(BB);

	const u32 crNand = 1 ^ (a & b);

	if (crNand & 1)
		BSET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	else
		BCLR(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
}
/* CR Nor */
void PPCInterpreter::PPCInterpreter_crnor(PPU_STATE* hCore)
{
	XL_FORM_BT_BA_BB;

	const u32 a = CR_GET(BA);
	const u32 b = CR_GET(BB);

	const u32 crNor = 1 ^ (a | b);

	if (crNor & 1)
		BSET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	else
		BCLR(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
}
/* CR Or */
void PPCInterpreter::PPCInterpreter_cror(PPU_STATE* hCore)
{
	XL_FORM_BT_BA_BB;
	const u32 a = CR_GET(BA);
	const u32 b = CR_GET(BB);

	const u32 crOr = a | b;

	if (crOr & 1)
		BSET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	else
		BCLR(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
}
/* CR Or with Complement */
void PPCInterpreter::PPCInterpreter_crorc(PPU_STATE* hCore)
{
	XL_FORM_BT_BA_BB;

	const u32 a = CR_GET(BA);
	const u32 b = CR_GET(BB);

	const u32 crOrc = a | (1 ^ b);

	if (crOrc & 1)
		BSET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	else
		BCLR(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
}
/* CR Xor */
void PPCInterpreter::PPCInterpreter_crxor(PPU_STATE* hCore)
{
	XL_FORM_BT_BA_BB;

	const u32 a = CR_GET(BA);
	const u32 b = CR_GET(BB);

	const u32 crXor = a ^ b;

	if(crXor & 1)
		BSET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	else
		BCLR(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, 32, BT);
	
}

void PPCInterpreter::PPCInterpreter_divd(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	if (GPR(rB) != 0 && (GPR(rA) != 0x8000000000000000 
		|| GPR(rB) != 0xFFFFFFFFFFFFFFFF))
	{
		s64 r = (s64)GPR(rA) / (s64)GPR(rB);
		GPR(rD) = (u64)r;
	}

	if (OE)
	{
		std::cout << "PPC Interpreter: DIVDX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_divdu(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	if (GPR(rB) != 0)
	{
		GPR(rD) = (u64)((u64)GPR(rA) / (u64)GPR(rB));
	}

	if (OE)
	{
		std::cout << "PPC Interpreter: DIVDUX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_divwx(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	if (GPR(rB) != 0 && (GPR(rA) != 0x80000000 || GPR(rB) != 0xFFFFFFFF))
	{
		GPR(rD) = (UINT32)((INT64)(LONG)GPR(rA) / (INT64)(LONG)GPR(rB));
	}

	if (OE)
	{
		std::cout << "PPC Interpreter: DIVWX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_divwux(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	if ((u32)GPR(rB) != 0)
	{
		GPR(rD) = 
			(u32)((u64)(u32)GPR(rA) 
				/ (u64)(u32)GPR(rB));
	}

	if (OE)
	{
		std::cout << "PPC Interpreter: DIVWUX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_extsbx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_RC;

	u32 extendedByte = (u8)GPR(rS);

	GPR(rA) = EXTS(extendedByte, 8);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_extshx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_RC;

	u32 extended_u16 = (u16)GPR(rS);

	GPR(rA) = EXTS(extended_u16, 16);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_extswx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_RC;

	u32 extendedWord = (u32)GPR(rS);

	GPR(rA) = EXTS(extendedWord, 32);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_mcrf(PPU_STATE* hCore)
{
	XL_FORM_BF_BFA;

	u32 CR = DGET(hCore->ppuThread[hCore->currentThread].CR.CR_Hex, (BFA) * 4, (BFA) * 4 + 3);
	
	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_mfcr(PPU_STATE* hCore)
{
	XFX_FORM_rD;

	GPR(rD) = hCore->ppuThread[hCore->currentThread].CR.CR_Hex;
}

void PPCInterpreter::PPCInterpreter_mftb(PPU_STATE* hCore)
{
	XFX_FORM_rD_spr;   // because 5-bit fields are swapped

	switch (spr)
	{
	case 268:
		GPR(rD) = hCore->SPR.TB;
		break;
	case 269:
		GPR(rD) = HIDW(hCore->SPR.TB);
		break;

	default:
		std::cout << "PPC Interpreter: mftb-> Illegal instruction form!" << std::endl;
		break;
	}
}

void PPCInterpreter::PPCInterpreter_mtcrf(PPU_STATE* hCore)
{
	XFX_FORM_rS_FXM;

	u32 Mask = 0;
	u32 b = 0x80;

	for (; b; b >>= 1)
	{
		Mask <<= 4;

		if (FXM & b)
		{
			Mask |= 0xF;
		}
	}

	hCore->ppuThread[hCore->currentThread].CR.CR_Hex = ((u32)GPR(rS) & Mask) | (hCore->ppuThread[hCore->currentThread].CR.CR_Hex & ~Mask);
}

void PPCInterpreter::PPCInterpreter_mulli(PPU_STATE* hCore)
{
	D_FORM_rD_rA_SI;
	SI = EXTS(SI, 16);

	u64 operand0, operand1;

	ppcMul64Signed(GPR(rA), SI, &operand0, &operand1);

	GPR(rD) = operand1;
}

void PPCInterpreter::PPCInterpreter_mulldx(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	u64 qwH, qwL;

	if (OE)
	{
		std::cout << "PPC Interpreter: mulldx -> Fatal error, OE not implemented!" << std::endl;
	}

	ppcMul64Signed(GPR(rA), GPR(rB), &qwH, &qwL);

	GPR(rD) = qwL;
}

void PPCInterpreter::PPCInterpreter_mullw(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	const s64 regA = static_cast<s32>(GPR(rA));
	const s64 regB = static_cast<s32>(GPR(rB));
	const s64 res = regA * regB;

	GPR(rD) = static_cast<u32>(res);

	if (OE)
	{
		std::cout << "PPC Interpreter: mullw -> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_mulhwux(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_RC;

	u64 operand0 = (u64)LODW(GPR(rA));
	u64 operand1 = (u64)LODW(GPR(rB));
	operand0 = operand0 * operand1;
	GPR(rD) = HIDW(operand0);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_mulhdux(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_RC;

	u64 qwH, qwL;

	ppcMul64(GPR(rA), GPR(rB), &qwH, &qwL);

	GPR(rD) = qwH;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}
/* Nand */
void PPCInterpreter::PPCInterpreter_nandx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	GPR(rA) = ~(GPR(rS) & GPR(rB));

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}
/* Negate */
void PPCInterpreter::PPCInterpreter_negx(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_OE_RC;

	GPR(rD) = (~GPR(rA)) + 1;

	if (OE)
	{
		std::cout << "PPC Interpreter: NEGX -> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}
/* Nor */
void PPCInterpreter::PPCInterpreter_norx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	GPR(rA) = ~(GPR(rS) | GPR(rB));

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}
/* Or Immediate */
void PPCInterpreter::PPCInterpreter_ori(PPU_STATE* hCore)
{
	D_FORM_rS_rA_UI;

	GPR(rA) = GPR(rS) | UI;
}
/* Or Immediate Shifted */
void PPCInterpreter::PPCInterpreter_oris(PPU_STATE* hCore)
{
	D_FORM_rS_rA_UI;

	GPR(rA) = GPR(rS) | (UI << 16);
}
/* Or */
void PPCInterpreter::PPCInterpreter_orx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	GPR(rA) = GPR(rS) | GPR(rB);

	if (RC)
	{
		u32 CR = CRCompU(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldicx(PPU_STATE* hCore)
{
	MD_FORM_rS_rA_sh_mb_RC;

	u64 r = _rotl64(GPR(rS), sh);
	u32 e = 63 - sh;
	u64 m = QMASK(mb, e);

	GPR(rA) = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldcrx(PPU_STATE* hCore)
{
	MDS_FORM_rS_rA_rB_me_RC;

	u64 qwRb = GPR(rB);
	u32 n = (u32)QGET(qwRb, 58, 63);
	u64 r = _rotl64(GPR(rS), n);
	u64 m = QMASK(0, me);

	GPR(rA) = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldiclx(PPU_STATE* hCore)
{
	MD_FORM_rS_rA_sh_mb_RC;

	u64 r = _rotl64(GPR(rS), sh);
	u64 m = QMASK(mb, 63);

	GPR(rA) = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldicrx(PPU_STATE* hCore)
{
	MD_FORM_rS_rA_sh_me_RC;

	u64 r = _rotl64(GPR(rS), sh);
	u64 m = QMASK(0, me);
	GPR(rA) = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldimix(PPU_STATE* hCore)
{
	MD_FORM_rS_rA_sh_mb_RC;

	u64 r = _rotl64(GPR(rS), sh);
	u32 e = 63 - sh;
	u64 m = QMASK(mb, e);

	GPR(rA) = (r & m) | (GPR(rA) & ~m);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rlwimix(PPU_STATE* hCore)
{
	M_FORM_rS_rA_SH_MB_ME_RC;

	u32 r = _rotl((u32)GPR(rS), SH);
	u32 m = (MB <= ME) ? DMASK(MB, ME) : (DMASK(0, ME) | DMASK(MB, 31));

	GPR(rA) = (r & m) | ((u32)GPR(rA) & ~m);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rlwnmx(PPU_STATE* hCore)
{
	M_FORM_rS_rA_rB_MB_ME_RC;

	u32 m = (MB <= ME) ? DMASK(MB, ME) : (DMASK(0, ME) | DMASK(MB, 31));

	GPR(rA) = _rotl((u32)GPR(rS), ((u32)GPR(rB)) & 31) & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rlwinmx(PPU_STATE* hCore)
{
	M_FORM_rS_rA_SH_MB_ME_RC;

	u32 m = (MB <= ME) ? DMASK(MB, ME) : (DMASK(0, ME) | DMASK(MB, 31));

	GPR(rA) = _rotl((u32)GPR(rS), SH) & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_sldx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u64 regB = GPR(rB);
	u32 n = (u32)QGET(regB, 58, 63);
	u64 r = _rotl64(GPR(rS), n);
	u64 m = QGET(regB, 57, 57) ? 0 : QMASK(0, 63 - n);

	GPR(rA) = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

/* Shift Left Word */
void PPCInterpreter::PPCInterpreter_slwx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u32 n = (u32)(GPR(rB)) & 63;

	GPR(rA) = (n < 32) ? ((u32)(GPR(rS)) << n) : 0;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_sradx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u64 regRS = GPR(rS);
	u32 n = (u32)GPR(rB) & 127;
	u64 r = _rotl64(regRS, 64 - (n & 63));
	u64 m = (n & 0x40) ? 0 : QMASK(n, 63);
	u64 s = BGET(regRS, 64, 0) ? QMASK(0, 63) : 0;

	GPR(rA) = (r & m) | (s & ~m);

	if (s && ((r & ~m) != 0))
		hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 1;
	else
		hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 0;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_sradix(PPU_STATE* hCore)
{
	X_FORM_rS_rA_SH_XO_RC;

	SH |= (XO & 1) << 5;

	if (SH == 0)
	{
		GPR(rA) = GPR(rS);
		hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 0;
	}
	else
	{
		u64 r = _rotl64(GPR(rS), 64 - SH);
		u64 m = QMASK(SH, 63);
		u64 s = BGET(GPR(rS), 64, 0);

		GPR(rA) = (r & m) | (((u64)(-(long long)s)) & ~m);

		if (s && (r & ~m) != 0)
			hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 1;
		else
			hCore->ppuThread[hCore->currentThread].SPR.XER.CA = 0;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

/* Shift Right Algebraic Word */
void PPCInterpreter::PPCInterpreter_srawx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u64 regRs = GPR(rS);
	u64 n = (u32)GPR(rB) & 63;
	u64 r = _rotl(static_cast<u32>(regRs), 64 - (n & 31));
	u64 m = (n & 0x20) ? 0 : QMASK(n + 32, 63);
	u64 s = BGET(regRs, 32, 0) ? QMASK(0, 63) : 0;
	GPR(rA) = (r & m) | (s & ~m);

	if (s && (((u32)(r & ~m)) != 0))
		XER_SET_CA(1);
	else
		XER_SET_CA(0);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

/* Shift Right Algebraic Word Immediate */
void PPCInterpreter::PPCInterpreter_srawix(PPU_STATE* hCore)
{
	X_FORM_rS_rA_SH_RC;

	u64 rSReg = GPR(rS);
	u64 r = _rotl(static_cast<u32>(rSReg), 64 - SH);
	u64 m = QMASK(SH + 32, 63);
	u64 s = BGET(rSReg, 32, 0) ? QMASK(0, 63) : 0;

	GPR(rA) = (r & m) | (s & ~m);

	if (s && (((u32)(r & ~m)) != 0))
		XER_SET_CA(1);
	else
		XER_SET_CA(0);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_srdx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;


	u64 regS = GPR(rS);
	u32 n = (u32)GPR(rB) & 127;
	u64 r = _rotl64(regS, 64 - (n & 63));
	u64 m = (n & 0x40) ? 0 : QMASK(n, 63);

	GPR(rA) = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}
/* Shift Right Word */
void PPCInterpreter::PPCInterpreter_srwx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u32 n = (u32)GPR(rB) & 63;

	GPR(rA) = (n < 32) ? (GPR(rS) >> n) : 0;

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_subfcx(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	GPR(rD) = ~GPR(rA) + GPR(rB) + 1;
	XER_SET_CA((GPR(rD) < ~GPR(rA)));

	if (OE)
	{
		std::cout << "PPC Interpreter: SUBFCX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_subfx(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	GPR(rD) = ~GPR(rA) + GPR(rB) + 1;

	if (OE)
	{
		std::cout << "PPC Interpreter: SUBFX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_subfex(PPU_STATE* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	GPR(rD) = ppcAddCarrying(hCore, ~GPR(rA), GPR(rB), hCore->ppuThread[hCore->currentThread].SPR.XER.CA);

	if (OE)
	{
		std::cout << "PPC Interpreter: SUBFEX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rD), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_subfic(PPU_STATE* hCore)
{
	D_FORM_rD_rA_SI;
	SI = EXTS(SI, 16);

	GPR(rD) = ppcAddCarrying(hCore, ~GPR(rA), SI, 1);
}

void PPCInterpreter::PPCInterpreter_xori(PPU_STATE* hCore)
{
	D_FORM_rS_rA_UI;

	GPR(rA) = GPR(rS) ^ UI;
}

void PPCInterpreter::PPCInterpreter_xoris(PPU_STATE* hCore)
{
	D_FORM_rS_rA_UI;

	GPR(rA) = GPR(rS) ^ (UI << 16);
}

void PPCInterpreter::PPCInterpreter_xorx(PPU_STATE* hCore)
{
	X_FORM_rS_rA_rB_RC;

	GPR(rA) = GPR(rS) ^ GPR(rB);

	if (RC)
	{
		u32 CR = CRCompS(hCore, GPR(rA), 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}