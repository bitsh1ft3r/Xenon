#include <stdlib.h>
#include <iostream>

#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

void PPCInterpreter::PPCInterpreter_addx(PPCState* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	hCore->GPR[rD] = hCore->GPR[rA] + hCore->GPR[rB];

	if (OE)
	{
		std::cout << "PPC Interpreter< ADDX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rD], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_addi(PPCState* hCore)
{
	D_FORM_rD_rA_SI;
	SI = EXTS(SI, 16);

	hCore->GPR[rD] = (rA ? hCore->GPR[rA] : 0) + SI;
}

void PPCInterpreter::PPCInterpreter_addic(PPCState* hCore)
{
	D_FORM_rD_rA_SI; 
	SI = EXTS(SI, 16);

	hCore->GPR[rD] = ppcAddCarrying(hCore, hCore->GPR[rA], SI, 0);
}

void PPCInterpreter::PPCInterpreter_addic_rc(PPCState* hCore)
{
	D_FORM_rD_rA_SI;
	SI = EXTS(SI, 16);

	hCore->GPR[rD] = ppcAddCarrying(hCore, hCore->GPR[rA], SI, 0);
	u32 CR = CRCompS(hCore, hCore->GPR[rD], 0);
	ppcUpdateCR(hCore, 0, CR);
}

void PPCInterpreter::PPCInterpreter_addis(PPCState* hCore)
{
	D_FORM_rD_rA_SI; 
	SI = EXTS(SI, 16);

	hCore->GPR[rD] = (rA ? hCore->GPR[rA] : 0) + (SI << 16);
}

void PPCInterpreter::PPCInterpreter_and(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	hCore->GPR[rA] = hCore->GPR[rS] & hCore->GPR[rB];

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_andc(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	hCore->GPR[rA] = hCore->GPR[rS] & ~hCore->GPR[rB];

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_andi(PPCState* hCore)
{
	D_FORM_rS_rA_UI;

	hCore->GPR[rA] = hCore->GPR[rS] & UI;
	u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
	ppcUpdateCR(hCore, 0, CR);
}

void PPCInterpreter::PPCInterpreter_andis(PPCState* hCore)
{
	D_FORM_rS_rA_UI;

	hCore->GPR[rA] = hCore->GPR[rS] & (UI << 16);
	ppcUpdateCR(hCore, 0, static_cast<u32>(hCore->GPR[rA]));
}

void PPCInterpreter::PPCInterpreter_cmp(PPCState* hCore)
{
	X_FORM_BF_L_rA_rB;

	u32 CR;

	if (L)
	{
		CR = CRCompS64(hCore, hCore->GPR[rA], hCore->GPR[rB]);
	}
	else
	{
		CR = CRCompS32(hCore, static_cast<u32>(hCore->GPR[rA]), 
			static_cast<u32>(hCore->GPR[rB]));
	}

	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_cmpi(PPCState* hCore)
{
	D_FORM_BF_L_rA_SI; 
	SI = EXTS(SI, 16);

	u32 CR;

	if (L)
	{
		CR = CRCompS64(hCore, hCore->GPR[rA], SI);
	}
	else
	{
		CR = CRCompS32(hCore, (u32)hCore->GPR[rA], (u32)SI);
	}

	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_cmpl(PPCState* hCore)
{
	X_FORM_BF_L_rA_rB;

	u32 CR;

	if (L)
	{
		CR = CRCompU(hCore, hCore->GPR[rA], hCore->GPR[rB]);
	}
	else
	{
		CR = CRCompU(hCore, (u32)hCore->GPR[rA], (u32)hCore->GPR[rB]);
	}

	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_cmpli(PPCState* hCore)
{
	D_FORM_BF_L_rA_UI;

	u32 CR;

	if (L)
	{
		CR = CRCompU(hCore, hCore->GPR[rA], UI);
	}
	else
	{
		CR = CRCompU(hCore, (u32)hCore->GPR[rA], UI);
	}

	ppcUpdateCR(hCore, BF, CR);
}

void PPCInterpreter::PPCInterpreter_cntlzw(PPCState* hCore)
{
	X_FORM_rS_rA_RC;

	u32 RegS = (u32)hCore->GPR[rS];
	u32 Mask = (1u << 31);
	u32 regA = 0;

	for (; Mask && (RegS & Mask) == 0; Mask >>= 1)
	{
		regA += 1;
	}

	hCore->GPR[rA] = regA;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_divdu(PPCState* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	if (hCore->GPR[rB] != 0)
	{
		hCore->GPR[rD] = (u64)((u64)hCore->GPR[rA] / (u64)hCore->GPR[rB]);
	}

	if (OE)
	{
		std::cout << "PPC Interpreter: DIVDUX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_extsbx(PPCState* hCore)
{
	X_FORM_rS_rA_RC;

	u32 extendedByte = (u8)hCore->GPR[rS];

	hCore->GPR[rA] = EXTS(extendedByte, 8);

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_extshx(PPCState* hCore)
{
	X_FORM_rS_rA_RC;

	u32 extended_u16 = (u16)hCore->GPR[rS];

	hCore->GPR[rA] = EXTS(extended_u16, 16);

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_extswx(PPCState* hCore)
{
	X_FORM_rS_rA_RC;

	u32 extendedWord = (u32)hCore->GPR[rS];

	hCore->GPR[rA] = EXTS(extendedWord, 32);

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_mfcr(PPCState* hCore)
{
	XFX_FORM_rD;

	hCore->GPR[rD] = hCore->CR.CR_Hex;
}

void PPCInterpreter::PPCInterpreter_mftb(PPCState* hCore)
{
	XFX_FORM_rD_spr;   // because 5-bit fields are swapped

	switch (spr)
	{
	case 268:
		hCore->GPR[rD] = hCore->SPR[SPR_TBL];
		break;

	case 269:
		hCore->GPR[rD] = HIDW(hCore->SPR[SPR_TBL]);
		break;

	default:
		std::cout << "PPC Interpreter: mftb-> Illegal instruction form!" << std::endl;
		break;
	}
}

void PPCInterpreter::PPCInterpreter_mtcrf(PPCState* hCore)
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

	hCore->CR.CR_Hex = ((u32)hCore->GPR[rS] & Mask) | (hCore->CR.CR_Hex & ~Mask);
}

void PPCInterpreter::PPCInterpreter_mulli(PPCState* hCore)
{
	D_FORM_rD_rA_SI;
	SI = EXTS(SI, 16);

	u64 operand0, operand1;

	ppcMul64Signed(hCore->GPR[rA], SI, &operand0, &operand1);

	hCore->GPR[rD] = operand1;
}

void PPCInterpreter::PPCInterpreter_mulldx(PPCState* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	u64 qwH, qwL;

	ppcMul64Signed(hCore->GPR[rA], hCore->GPR[rB], &qwH, &qwL);

	hCore->GPR[rD] = qwL;
}

void PPCInterpreter::PPCInterpreter_mullw(PPCState* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	hCore->GPR[rD] = (u32)((s64)(long)hCore->GPR[rA] * (s64)(long)hCore->GPR[rB]);

	if (OE)
	{
		std::cout << "PPC Interpreter: mullw -> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rD], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_mulhdux(PPCState* hCore)
{
	XO_FORM_rD_rA_rB_RC;

	u64 qwH, qwL;

	ppcMul64(hCore->GPR[rA], hCore->GPR[rB], &qwH, &qwL);

	hCore->GPR[rD] = qwH;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rD], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_negx(PPCState* hCore)
{
	XO_FORM_rD_rA_OE_RC;

	hCore->GPR[rD] = ~hCore->GPR[rA] + 1;

	if (OE)
	{
		std::cout << "PPC Interpreter: NEGX -> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rD], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_norx(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	hCore->GPR[rA] = ~(hCore->GPR[rS] | hCore->GPR[rB]);

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_ori(PPCState* hCore)
{
	D_FORM_rS_rA_UI;

	hCore->GPR[rA] = hCore->GPR[rS] | UI;
}

void PPCInterpreter::PPCInterpreter_oris(PPCState* hCore)
{
	D_FORM_rS_rA_UI;

	hCore->GPR[rA] = hCore->GPR[rS] | (UI << 16);
}

void PPCInterpreter::PPCInterpreter_orx(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	hCore->GPR[rA] = hCore->GPR[rS] | hCore->GPR[rB];

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldicx(PPCState* hCore)
{
	MD_FORM_rS_rA_sh_mb_RC;

	u64 r = _rotl64(hCore->GPR[rS], sh);
	u32 e = 63 - sh;
	u64 m = QMASK(mb, e);

	hCore->GPR[rA] = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldcrx(PPCState* hCore)
{
	MDS_FORM_rS_rA_rB_me_RC;

	u64 qwRb = hCore->GPR[rB];
	u32 n = (u32)QGET(qwRb, 58, 63);
	u64 r = _rotl64(hCore->GPR[rS], n);
	u64 m = QMASK(0, me);

	hCore->GPR[rA] = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldiclx(PPCState* hCore)
{
	MD_FORM_rS_rA_sh_mb_RC;

	u64 r = _rotl64(hCore->GPR[rS], sh);
	u64 m = QMASK(mb, 63);

	hCore->GPR[rA] = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldicrx(PPCState* hCore)
{
	MD_FORM_rS_rA_sh_me_RC;

	u64 r = _rotl64(hCore->GPR[rS], sh);
	u64 m = QMASK(0, me);
	hCore->GPR[rA] = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rldimix(PPCState* hCore)
{
	MD_FORM_rS_rA_sh_mb_RC;

	u64 r = _rotl64(hCore->GPR[rS], sh);
	u32 e = 63 - sh;
	u64 m = QMASK(mb, e);

	hCore->GPR[rA] = (r & m) | (hCore->GPR[rA] & ~m);

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rlwimix(PPCState* hCore)
{
	M_FORM_rS_rA_SH_MB_ME_RC;

	u32 r = _rotl((u32)hCore->GPR[rS], SH);
	u32 m = (MB <= ME) ? DMASK(MB, ME) : (DMASK(0, ME) | DMASK(MB, 31));

	hCore->GPR[rA] = (r & m) | ((u32)hCore->GPR[rA] & ~m);

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rlwnmx(PPCState* hCore)
{
	M_FORM_rS_rA_rB_MB_ME_RC;

	u32 m = (MB <= ME) ? DMASK(MB, ME) : (DMASK(0, ME) | DMASK(MB, 31));

	hCore->GPR[rA] = _rotl((u32)hCore->GPR[rS], ((u32)hCore->GPR[rB]) & 31) & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_rlwinmx(PPCState* hCore)
{
	M_FORM_rS_rA_SH_MB_ME_RC;

	u32 m = (MB <= ME) ? DMASK(MB, ME) : (DMASK(0, ME) | DMASK(MB, 31));

	hCore->GPR[rA] = _rotl((u32)hCore->GPR[rS], SH) & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_sldx(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u64 regB = hCore->GPR[rB];
	u32 n = (u32)QGET(regB, 58, 63);
	u64 r = _rotl64(hCore->GPR[rS], n);
	u64 m = QGET(regB, 57, 57) ? 0 : QMASK(0, 63 - n);

	hCore->GPR[rA] = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_slwx(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u32 n = (u32)(hCore->GPR[rB]) & 63;

	hCore->GPR[rA] = (n < 32) ? ((u32)(hCore->GPR[rS]) << n) : 0;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_sradx(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u64 regRS = hCore->GPR[rS];
	u32 n = (u32)hCore->GPR[rB] & 127;
	u64 r = _rotl64(regRS, 64 - (n & 63));
	u64 m = (n & 0x40) ? 0 : QMASK(n, 63);
	u64 s = BGET(regRS, 64, 0) ? QMASK(0, 63) : 0;

	hCore->GPR[rA] = (r & m) | (s & ~m);

	if (s && ((r & ~m) != 0))
		hCore->XER.CA = 1;
	else
		hCore->XER.CA = 0;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_sradix(PPCState* hCore)
{
	X_FORM_rS_rA_SH_XO_RC;

	SH |= (XO & 1) << 5;

	if (SH == 0)
	{
		hCore->GPR[rA] = hCore->GPR[rS];
		hCore->XER.CA = 0;
	}
	else
	{
		u64 r = _rotl64(hCore->GPR[rS], 64 - SH);
		u64 m = QMASK(SH, 63);
		u64 s = BGET(hCore->GPR[rS], 64, 0);

		hCore->GPR[rA] = (r & m) | (((u64)(-(long long)s)) & ~m);

		if (s && (r & ~m) != 0)
			hCore->XER.CA = 1;
		else
			hCore->XER.CA = 0;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_srawix(PPCState* hCore)
{
	X_FORM_rS_rA_SH_RC;

	u64 rSReg = hCore->GPR[rS];
	u64 r = _rotl(static_cast<u32>(rSReg), 64 - SH);
	u64 m = QMASK(SH + 32, 63);
	u64 s = BGET(rSReg, 32, 0) ? QMASK(0, 63) : 0;

	hCore->GPR[rA] = (r & m) | (s & ~m);

	if (s && (((u32)(r & ~m)) != 0))
		hCore->XER.CA = 1;
	else
		hCore->XER.CA = 0;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_srdx(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;


	u64 regS = hCore->GPR[rS];
	u32 n = (u32)hCore->GPR[rB] & 127;
	u64 r = _rotl64(regS, 64 - (n & 63));
	u64 m = (n & 0x40) ? 0 : QMASK(n, 63);

	hCore->GPR[rA] = r & m;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_srwx(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	u32 n = (u32)hCore->GPR[rB] & 63;

	hCore->GPR[rA] = (n < 32) ? (hCore->GPR[rS] >> n) : 0;

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_subfcx(PPCState* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	hCore->GPR[rD] = ~hCore->GPR[rA] + hCore->GPR[rB] + 1;
	hCore->XER.CA = (hCore->GPR[rD] < ~hCore->GPR[rA]);

	if (OE)
	{
		std::cout << "PPC Interpreter: SUBFCX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rD], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_subfx(PPCState* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	hCore->GPR[rD] = ~hCore->GPR[rA] + hCore->GPR[rB] + 1;

	if (OE)
	{
		std::cout << "PPC Interpreter: SUBFX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rD], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_subfex(PPCState* hCore)
{
	XO_FORM_rD_rA_rB_OE_RC;

	hCore->GPR[rD] = ppcAddCarrying(hCore, ~hCore->GPR[rA], hCore->GPR[rB], hCore->XER.CA);

	if (OE)
	{
		std::cout << "PPC Interpreter: SUBFEX-> Fatal error, OE not implemented!" << std::endl;
	}

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rD], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}

void PPCInterpreter::PPCInterpreter_subfic(PPCState* hCore)
{
	D_FORM_rD_rA_SI;
	SI = EXTS(SI, 16);

	hCore->GPR[rD] = ppcAddCarrying(hCore, ~hCore->GPR[rA], SI, 1);
}

void PPCInterpreter::PPCInterpreter_xori(PPCState* hCore)
{
	D_FORM_rS_rA_UI;

	hCore->GPR[rA] = hCore->GPR[rS] ^ UI;
}

void PPCInterpreter::PPCInterpreter_xoris(PPCState* hCore)
{
	D_FORM_rS_rA_UI;

	hCore->GPR[rA] = hCore->GPR[rS] ^ (UI << 16);
}

void PPCInterpreter::PPCInterpreter_xorx(PPCState* hCore)
{
	X_FORM_rS_rA_rB_RC;

	hCore->GPR[rA] = hCore->GPR[rS] ^ hCore->GPR[rB];

	if (RC)
	{
		u32 CR = CRCompS(hCore, hCore->GPR[rA], 0);
		ppcUpdateCR(hCore, 0, CR);
	}
}
