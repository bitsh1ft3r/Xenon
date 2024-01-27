#include <iostream>

#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"


void PPCInterpreter::PPCInterpreter_isync(PPCState* hCore)
{
	// Do nothing :)
}

void PPCInterpreter::PPCInterpreter_sc(PPCState* hCore)
{
	SC_FORM_LEV;

	ppcSystemCallException(hCore, LEV & 1);
}

void PPCInterpreter::PPCInterpreter_slbmte(PPCState* hCore)
{
	X_FORM_rS_rB;

	u64 VSID = QGET(hCore->GPR[rS], 0,51);

	u8 Ks = QGET(hCore->GPR[rS],52,52);
	u8 Kp = QGET(hCore->GPR[rS], 53, 53);
	u8 N = QGET(hCore->GPR[rS], 54, 54);
	u8 L = QGET(hCore->GPR[rS], 55, 55);
	u8 C = QGET(hCore->GPR[rS], 56, 56);
	u8 LP = QGET(hCore->GPR[rS], 57, 59);

	u64 ESID = QGET(hCore->GPR[rB], 0, 35);
	ESID = ESID << 28;
	bool V = QGET(hCore->GPR[rB], 36, 36);
	u16 Index = QGET(hCore->GPR[rB], 52, 63);
	
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             	hCore->SLB[Index].ESID = ESID;
	hCore->SLB[Index].VSID = VSID;
	hCore->SLB[Index].V = V;
	hCore->SLB[Index].Kp = Kp;
	hCore->SLB[Index].Ks = Ks;
	hCore->SLB[Index].N = N;
	hCore->SLB[Index].L = L;
	hCore->SLB[Index].C = C;
	hCore->SLB[Index].LP = LP;
}

void PPCInterpreter::PPCInterpreter_rfid(PPCState* hCore)
{
	u64 srr1, new_msr, diff_msr;
	u32 b3, b, usr;
	//return;

	// Compose new MSR as per specs
	srr1 = hCore->SPR[SPR_SRR1];
	new_msr = 0;

	usr = BGET(srr1, 64, 49);
	if (usr)
	{
		//(("WARNING!! Cannot really do Problem mode"));
		BSET(new_msr, 64, 49);
	}

	// MSR.0 = SRR1.0 | SRR1.1
	b = BGET(srr1, 64, 0) || BGET(srr1, 64, 1);
	if (b)
	{
		BSET(new_msr, 64, 0);
	}

	b3 = BGET(hCore->MSR.MSR_Hex, 64, 3);

	// MSR.51 = (MSR.3 & SRR1.51) | ((~MSR.3) & MSR.51)
	if ((b3 && BGET(srr1, 64, 51)) ||
		(!b3 && BGET(hCore->MSR.MSR_Hex, 64, 51)))
	{
		BSET(new_msr, 64, 51);
	}

	// MSR.3 = MSR.3 & SRR1.3
	if (b3 && BGET(srr1, 64, 3))
	{
		BSET(new_msr, 64, 3);
	}

	// MSR.48 = SRR1.48
	if (BGET(srr1, 64, 48))
	{
		BSET(new_msr, 64, 48);
	}

	// MSR.58 = SRR1.58 | SRR1.49
	if (usr || BGET(srr1, 64, 58))
	{
		BSET(new_msr, 64, 58);
	}

	// MSR.59 = SRR1.59 | SRR1.49
	if (usr || BGET(srr1, 64, 59))
	{
		BSET(new_msr, 64, 59);
	}

	// MSR.1:2,4:32,37:41,49:50,52:57,60:63 = SRR1.<same>
	new_msr = new_msr |
		(srr1 & (QMASK(1, 2) | QMASK(4, 32) | QMASK(37, 41) | QMASK(49, 50) | QMASK(52, 57) | QMASK(60, 63)));

	// See what changed and take actions
	// NB: we ignore a bunch of bits..
	diff_msr = hCore->MSR.MSR_Hex ^ new_msr;

	// NB: we dont do half-modes
	if (diff_msr & QMASK(58, 59))
	{
		if (usr) {
			hCore->MSR.IR = true;
			hCore->MSR.DR = true;
		}
		else if (new_msr & QMASK(58, 59)) {
			hCore->MSR.IR = true;
			hCore->MSR.DR = true;
		}
		else {
			hCore->MSR.IR = false;
			hCore->MSR.DR = false;
		}
	}

	hCore->MSR.MSR_Hex = new_msr;
	hCore->NIA = hCore->SPR[SPR_SRR0] & ~3;
}

void PPCInterpreter::PPCInterpreter_mfspr(PPCState* hCore)
{
	u64 rS, crm = 0;
	PPC_OPC_TEMPL_XFX(hCore->CI, rS, crm);
	u32 sprNum = ExtractBits(hCore->CI, 11, 20);
	sprNum = ((sprNum & 0x1f) << 5) | ((sprNum >> 5) & 0x1F);
	hCore->GPR[rS] = hCore->SPR[sprNum];
}

void PPCInterpreter::PPCInterpreter_mtspr(PPCState* hCore)
{
	XFX_FORM_rD_spr;
	switch (spr)
	{
	case SPR_SDR1:
		std::cout << "SPR_SDR1 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_DAR:
		std::cout << "SPR_DAR = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_DSISR:
		std::cout << "SPR_DSISR = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_CTR:
		break;
	case SPR_LR:
		break;
	case SPR_LPCR:
		std::cout << "SPR_LPCR = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_HID1:
		std::cout << "SPR_HID1 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_HID4:
		std::cout << "SPR_HID4 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_HID6:
		std::cout << "SPR_HID6 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_SRR0:
		std::cout << "SPR_SRR0 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_SRR1:
		std::cout << "SPR_SRR1 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_HRMOR:
		std::cout << "SPR_HRMOR = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_PpeTlbIndex:
		std::cout << "SPR_PpeTlbIndex = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_PpeTlbRpn:
		std::cout << "SPR_PpeTlbRpn = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_PpeTlbVpn:
		std::cout << "SPR_PpeTlbVpn = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		hCore->SPR[spr] = hCore->GPR[rD];
		mmuAddTlbEntry(hCore);
		break;
	case SPR_TTR:
		std::cout << "SPR_TTR = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_TSCR:
		std::cout << "SPR_TSCR = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_HSPRG0:
		std::cout << "SPR_HSPRG0 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_HSPRG1:
		std::cout << "SPR_HSPRG1 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_CTRLWR:
		std::cout<< "SPR_CTRL = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		hCore->SPR[SPR_CTRLRD] = hCore->GPR[rD]; // Also do the write on SPR_CTRLRD
		break;
	case SPR_RMOR:
		std::cout << "SPR_RMOR = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_LPIDR:
		std::cout << "SPR_LPIDR = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	case SPR_SPRG1:
		std::cout << "SPR_SPRG1 = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	default:
		std::cout << "SPR " << std::dec << spr << " = 0x" << std::hex << hCore->GPR[rD] << std::endl;
		break;
	}

	hCore->SPR[spr] = hCore->GPR[rD];
}

void PPCInterpreter::PPCInterpreter_mfmsr(PPCState* hCore)
{
	X_FORM_rD;
	hCore->GPR[rD] = hCore->MSR.MSR_Hex;
	std::cout << "mfmsr r" << rD << " msr: 0x" << std::hex << hCore->MSR.MSR_Hex << std::endl;
}

void PPCInterpreter::PPCInterpreter_mtmsrd(PPCState* hCore)
{
	X_FORM_rS_L;

	u64 qwMSRMask;
	u64 qwMSRSet;

	if (L == 0)
	{
		qwMSRMask = QMASK(0, 2) | QMASK(4, 50) | QMASK(52, 63);

		qwMSRSet = hCore->GPR[rS] & (QMASK(1, 2) | QMASK(4, 47) | QMASK(49, 50) | QMASK(52, 57) | QMASK(60, 63));

		if (hCore->GPR[rS] & QMASK(0, 1))
		{
			qwMSRSet |= QMASK(0, 0);
		}

		if (hCore->GPR[rS] & QMASK(48, 49))
		{
			qwMSRSet |= QMASK(48, 48);
		}

		if (hCore->GPR[rS] & (QMASK(49, 49) | QMASK(58, 58)))
		{
			qwMSRSet |= QMASK(58, 58);
		}

		if (hCore->GPR[rS] & (QMASK(49, 49) | QMASK(59, 59)))
		{
			qwMSRSet |= QMASK(59, 59);
		}
	}
	else // L==1 
	{
		qwMSRMask = QMASK(48, 48) | QMASK(62, 62);
		qwMSRSet = hCore->GPR[rS] & qwMSRMask;
	}

	/* Highly deficient */
	hCore->MSR.MSR_Hex = qwMSRSet | (hCore->MSR.MSR_Hex & ~qwMSRMask);
}
