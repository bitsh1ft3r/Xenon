#include <iostream>

#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

void PPCInterpreter::PPCInterpreter_isync(PPU_STATE* hCore)
{
	// Do nothing :)
}

void PPCInterpreter::PPCInterpreter_sc(PPU_STATE* hCore)
{
	SC_FORM_LEV;

	ppcSystemCallException(hCore, LEV & 1);

	u16 syscallNum = (u16)hCore->ppuThread[hCore->currentThread].GPR[0x0];
	std::string syscallName = "";

	switch (syscallNum)
	{
	case 0x0:
		syscallName = "HvxGetVersions";
		break;
	case 0x1:
		syscallName = "HvxStartupProcessors";
		break;
	case 0x2:
		syscallName = "HvxQuiesceProcessor";
		break;
	case 0x3:
		syscallName = "KeFlushCurrentEntireTb";
		break;
	case 0x4:
		syscallName = "HvxFlushSingleTb";
		break;
	case 0x6:
		syscallName = "HvxGetSpecialPurposeRegister";
		break;
	case 0x7:
		syscallName = "HvxSetSpecialPurposeRegister";
		break;
	case 0x8:
		syscallName = "HvxGetSocRegister";
		break;
	case 0x9:
		syscallName = "HvxSetSocRegister";
		break;
	case 0xA:
		syscallName = "HvxSetTimeBaseToZero";
		break;
	case 0xB:
		syscallName = "KeZeroPage";
		break;
	case 0xC:
		syscallName = "HvxFlushDcacheRange";
		break;
	case 0xD: 
		syscallName = "HvxPostOutput";
		break;
	case 0xE:
		syscallName = "KeEnablePPUPerformanceMonitor";
		break;
	case 0xF:
		syscallName = "HvxGetImagePageTableEntry";
		break;
	case 0x10:
		syscallName = "HvxSetImagePageTableEntry";
		break;
	case 0x11:
		syscallName = "HvxCreateImageMapping";
		break;
	case 0x12:
		syscallName = "HvxMapImagePage";
		break;
	case 0x13:
		syscallName = "HvxCompleteImageMapping";
		break;
	case 0x14:
		syscallName = "HvxLoadImageData";
		break;
	case 0x15:
		syscallName = "HvxFinishImageDataLoad";
		break;
	case 0x16:
		syscallName = "HvxStartResolveImports";
		break;
	case 0x17:
		syscallName = "HvxResolveImports";
		break;
	case 0x18:
		syscallName = "HvxFinishImageLoad";
		break;
	case 0x19:
		syscallName = "HvxAbandonImageLoad";
		break;
	case 0x1A:
		syscallName = "HvxUnmapImagePages";
		break;
	case 0x1B:
		syscallName = "HvxUnmapImage";
		break;
	case 0x1C:
		syscallName = "HvxUnmapImageRange";
		break;
	case 0x1D:
		syscallName = "HvxCreateUserMode";
		break;
	case 0x1E:
		syscallName = "HvxDeleteUserMode";
		break;
	case 0x1F:
		syscallName = "HvxFlushUserModeTb";
		break;
	case 0x20:
		syscallName = "HvxSetPowerMode";
		break;
	case 0x21:
		syscallName = "HvxShadowBoot";
		break;
	case 0x22:
		syscallName = "HvxBlowFuses";
		break;
	case 0x23:
		syscallName = "HvxFsbInterrupt";
		break;
	case 0x24:
		syscallName = "HvxLockL2";
		break;
	case 0x25:
		syscallName = "HvxDvdAuthBuildNVPage";
		break;
	case 0x26:
		syscallName = "HvxDvdAuthVerifyNVPage";
		break;
	case 0x27:
		syscallName = "HvxDvdAuthRecordAuthenticationPage";
		break;
	case 0x28:
		syscallName = "HvxDvdAuthRecordXControl";
		break;
	case 0x29:
		syscallName = "HvxDvdAuthGetAuthPage";
		break;
	case 0x2A:
		syscallName = "HvxDvdAuthVerifyAuthPage";
		break;
	case 0x2B:
		syscallName = "HvxDvdAuthGetNextLBAIndex";
		break;
	case 0x2C:
		syscallName = "HvxDvdAuthVerifyLBA";
		break;
	case 0x2D:
		syscallName = "HvxDvdAuthClearDiscAuthInfo";
		break;
	case 0x2E:
		syscallName = "HvxKeysInitialize";
		break;
	case 0x2F:
		syscallName = "HvxKeysGetKeyProperties";
		break;
	case 0x30:
		syscallName = "HvxKeysGetStatus";
		break;
	case 0x31:
		syscallName = "HvxKeysGenerateRandomKey";
		break;
	case 0x32:
		syscallName = "HvxKeysGetFactoryChallenge";
		break;
	case 0x33:
		syscallName = "HvxKeysSetFactoryResponse";
		break;
	case 0x34:
		syscallName = "HvxKeysSaveBootLoader";
		break;
	case 0x35:
		syscallName = "HvxKeysSaveKeyVault";
		break;
	case 0x36:
		syscallName = "HvxKeysSetKey";
		break;
	case 0x37:
		syscallName = "HvxKeysGetKey";
		break;
	case 0x38:
		syscallName = "HvxKeysGetDigest";
		break;
	case 0x39:
		syscallName = "HvxKeysRsaPrvCrypt";
		break;
	case 0x3A:
		syscallName = "HvxKeysHmacSha";
		break;
	case 0x3B:
		syscallName = "HvxKeysAesCbc";
		break;
	case 0x3C:
		syscallName = "HvxKeysDes2Cbc";
		break;
	case 0x3D:
		syscallName = "HvxKeysDesCbc";
		break;
	case 0x3E:
		syscallName = "HvxKeysObscureKey";
		break;
	case 0x3F:
		syscallName = "HvxKeysSaveSystemUpdate";
		break;
	case 0x40:
		syscallName = "HvxKeysExecute";
		break;
	case 0x41:
		syscallName = "HvxDvdAuthTestMode";
		break;
	case 0x42:
		syscallName = "HvxEnableTimebase";
		break;
	case 0x43:
		syscallName = "HvxHdcpCalculateMi";
		break;
	case 0x44:
		syscallName = "HvxHdcpCalculateAKsvSignature";
		break;
	case 0x45:
		syscallName = "HvxHdcpCalculateBKsvSignature";
		break;
	case 0x46:
		syscallName = "HvxSetRevocationList";
		break;
	case 0x47:
		syscallName = "HvxEncryptedAllocationReserve";
		break;
	case 0x48:
		syscallName = "HvxEncryptedAllocationMap";
		break;
	case 0x49:
		syscallName = "HvxEncryptedAllocationUnmap";
		break;
	case 0x4A:
		syscallName = "HvxEncryptedAllocationRelease";
		break;
	case 0x4B:
		syscallName = "HvxEncryptedSweepAddressRange";
		break;
	case 0x4C:
		syscallName = "HvxKeysExCreateKeyVault";
		break;
	case 0x4D:
		syscallName = "HvxKeysExLoadKeyVault";
		break;
	case 0x4E:
		syscallName = "HvxKeysExSaveKeyVault";
		break;
	case 0x4F:
		syscallName = "HvxKeysExSetKey";
		break;
	case 0x50:
		syscallName = "HvxKeysExGetKey";
		break;
	case 0x51:
		syscallName = "HvxGetUpdateSequence";
		break;
	case 0x52:
		syscallName = "HvxSecurityInitialize";
		break;
	case 0x53:
		syscallName = "HvxSecurityLoadSettings";
		break;
	case 0x54:
		syscallName = "HvxSecuritySaveSettings";
		break;
	case 0x55:
		syscallName = "HvxSecuritySetDetected";
		break;
	case 0x56:
		syscallName = "HvxSecurityGetDetected";
		break;
	case 0x57:
		syscallName = "HvxSecuritySetActivated";
		break;
	case 0x58:
		syscallName = "HvxSecurityGetActivated";
		break;
	case 0x59:
		syscallName = "HvxSecuritySetStat";
		break;
	case 0x5A:
		syscallName = "HvxGetProtectedFlags";
		break;
	case 0x5B:
		syscallName = "HvxSetProtectedFlag";
		break;
	case 0x5C:
		syscallName = "HvxDvdAuthGetAuthResults";
		break;
	case 0x5D:
		syscallName = "HvxDvdAuthSetDriveAuthResult";
		break;
	case 0x5E:
		syscallName = "HvxDvdAuthSetDiscAuthResult";
		break;
	case 0x5F:
		syscallName = "HvxImageTransformImageKey";
		break;
	case 0x60:
		syscallName = "HvxImageXexHeader";
		break;
	case 0x61:
		syscallName = "HvxRevokeLoad";
		break;
	case 0x62:
		syscallName = "HvxRevokeSave";
		break;
	case 0x63:
		syscallName = "HvxRevokeUpdate";
		break;
	case 0x64:
		syscallName = "HvxDvdAuthGetMediaId";
		break;
	case 0x65:
		syscallName = "HvxXexActivationGetNonce";
		break;
	case 0x66:
		syscallName = "HvxXexActivationSetLicense";
		break;
	case 0x67:
		syscallName = "HvxXexActivationVerifyOwnership";
		break;
	case 0x68:
		syscallName = "HvxIptvSetBoundaryKey";
		break;
	case 0x69:
		syscallName = "HvxIptvSetSessionKey";
		break;
	case 0x6A:
		syscallName = "HvxIptvVerifyOmac1Signature";
		break;
	case 0x6B:
		syscallName = "HvxIptvGetAesCtrTransform";
		break;
	case 0x6C:
		syscallName = "HvxIptvGetSessionKeyHash";
		break;
	case 0x6D:
		syscallName = "HvxImageDvdEmulationMode";
		break;
	case 0x6F:
		syscallName = "HvxImageShim";
		break;
	case 0x70:
		syscallName = "HvxExpansionInstall";
		break;
	case 0x72:
		syscallName = "HvxDvdAuthFwcr";
		break;
	case 0x73:
		syscallName = "HvxDvdAuthFcrt";
		break;
	case 0x74:
		syscallName = "HvxDvdAuthEx";
		break;
	default:
		syscallName = " *** Unknown System Call! ***";
		break;
	}
	std::cout << "XCPU(" << hCore->ppuName << ") SystemCall: " 
		<< syscallName.c_str() << " ID: 0x" << syscallNum << std::endl;
}

void PPCInterpreter::PPCInterpreter_slbmte(PPU_STATE* hCore)
{
	X_FORM_rS_rB;

	u64 VSID = QGET(hCore->ppuThread[hCore->currentThread].GPR[rS], 0,51);

	u8 Ks = QGET(hCore->ppuThread[hCore->currentThread].GPR[rS],52,52);
	u8 Kp = QGET(hCore->ppuThread[hCore->currentThread].GPR[rS], 53, 53);
	u8 N = QGET(hCore->ppuThread[hCore->currentThread].GPR[rS], 54, 54);
	u8 L = QGET(hCore->ppuThread[hCore->currentThread].GPR[rS], 55, 55);
	u8 C = QGET(hCore->ppuThread[hCore->currentThread].GPR[rS], 56, 56);
	u8 LP = QGET(hCore->ppuThread[hCore->currentThread].GPR[rS], 57, 59);

	u64 ESID = QGET(hCore->ppuThread[hCore->currentThread].GPR[rB], 0, 35);
	bool V = QGET(hCore->ppuThread[hCore->currentThread].GPR[rB], 36, 36);
	u16 Index = QGET(hCore->ppuThread[hCore->currentThread].GPR[rB], 52, 63);
	
	hCore->ppuThread[hCore->currentThread].SLB[Index].ESID = ESID;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         	hCore->ppuThread[hCore->currentThread].SLB[Index].ESID = ESID;
	hCore->ppuThread[hCore->currentThread].SLB[Index].VSID = VSID;
	hCore->ppuThread[hCore->currentThread].SLB[Index].V = V;
	hCore->ppuThread[hCore->currentThread].SLB[Index].Kp = Kp;
	hCore->ppuThread[hCore->currentThread].SLB[Index].Ks = Ks;
	hCore->ppuThread[hCore->currentThread].SLB[Index].N = N;
	hCore->ppuThread[hCore->currentThread].SLB[Index].L = L;
	hCore->ppuThread[hCore->currentThread].SLB[Index].C = C;
	hCore->ppuThread[hCore->currentThread].SLB[Index].LP = LP;
}

void PPCInterpreter::PPCInterpreter_rfid(PPU_STATE* hCore)
{
	u64 srr1, new_msr, diff_msr;
	u32 b3, b, usr;

	// Compose new MSR as per specs
	srr1 = hCore->ppuThread[hCore->currentThread].SPR.SRR1;
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

	b3 = BGET(hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex, 64, 3);

	// MSR.51 = (MSR.3 & SRR1.51) | ((~MSR.3) & MSR.51)
	if ((b3 && BGET(srr1, 64, 51)) ||
		(!b3 && BGET(hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex, 64, 51)))
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
	diff_msr = hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex ^ new_msr;

	// NB: we dont do half-modes
	if (diff_msr & QMASK(58, 59))
	{
		if (usr) {
			hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = true;
			hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = true;
		}
		else if (new_msr & QMASK(58, 59)) {
			hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = true;
			hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = true;
		}
		else {
			hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = false;
			hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = false;
		}
	}

	hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex = new_msr;
	hCore->ppuThread[hCore->currentThread].NIA = hCore->ppuThread[hCore->currentThread].SPR.SRR0 & ~3;
}

void PPCInterpreter::PPCInterpreter_tw(PPU_STATE* hCore)
{
	X_FORM_TO_rA_rB;

	long a = (long)hCore->ppuThread[hCore->currentThread].GPR[rA];
	long b = (long)hCore->ppuThread[hCore->currentThread].GPR[rB];

	if ((a < b && BGET(TO, 5, 0))
		|| (a > b && BGET(TO, 5, 1))
		|| (a == b && BGET(TO, 5, 2))
		|| ((u32)a < (u32)b && BGET(TO, 5, 3))
		|| ((u32)a > (u32)b && BGET(TO, 5, 4)))
	{
		ppcInterpreterTrap(hCore,b);
	}
}

void PPCInterpreter::PPCInterpreter_twi(PPU_STATE* hCore)
{
	D_FORM_TO_rA_SI; 
	SI = EXTS(SI, 16);

	long a = (long)hCore->ppuThread[hCore->currentThread].GPR[rA];

	if ((a < (long)SI && BGET(TO, 5, 0))
		|| (a > (long)SI && BGET(TO, 5, 1))
		|| (a == (long)SI && BGET(TO, 5, 2))
		|| ((u32)a < SI && BGET(TO, 5, 3))
		|| ((u32)a > SI && BGET(TO, 5, 4)))
	{
		ppcInterpreterTrap(hCore, (u32)SI);
	}
}

void PPCInterpreter::PPCInterpreter_tdi(PPU_STATE* hCore)
{
	D_FORM_TO_rA_SI; 
	SI = EXTS(SI, 16);

	s64 rAReg = (s64)hCore->ppuThread[hCore->currentThread].GPR[rA];

	if ((rAReg < (s64)SI && BGET(TO, 5, 0))
		|| (rAReg > (s64)SI && BGET(TO, 5, 1))
		|| (rAReg == (s64)SI && BGET(TO, 5, 2))
		|| ((u64)rAReg < SI && BGET(TO, 5, 3))
		|| ((u64)rAReg > SI && BGET(TO, 5, 4)))
	{
		ppcInterpreterTrap(hCore, (u32)SI);
	}
	
}

void PPCInterpreter::PPCInterpreter_mfspr(PPU_STATE* hCore)
{
	u64 rS, crm = 0;
	PPC_OPC_TEMPL_XFX(hCore->ppuThread[hCore->currentThread].CI, rS, crm);
	u32 sprNum = ExtractBits(hCore->ppuThread[hCore->currentThread].CI, 11, 20);
	sprNum = ((sprNum & 0x1f) << 5) | ((sprNum >> 5) & 0x1F);

	u64 value = 0;

	switch (sprNum)
	{
	case SPR_LR:
		value = hCore->ppuThread[hCore->currentThread].SPR.LR;
		break;
	case SPR_CTR:
		value = hCore->ppuThread[hCore->currentThread].SPR.CTR;
		break;
	case SPR_HRMOR:
		value = hCore->SPR.HRMOR;
		break;
	case SPR_RMOR:
		value = hCore->SPR.RMOR;
		break;
	case SPR_PIR:
		value = hCore->ppuThread[hCore->currentThread].SPR.PIR;
		break;
	case SPR_HID0:
		value = hCore->SPR.HID0;
		break;
	case SPR_HID1:
		value = hCore->SPR.HID1;
		break;
	case SPR_HID4:
		value = hCore->SPR.HID4;
		break;
	case SPR_HID6:
		value = hCore->SPR.HID6;
		break;
	case SPR_LPCR:
		value = hCore->SPR.LPCR;
		break;
	case SPR_PpeTlbIndexHint:
		value = hCore->ppuThread[hCore->currentThread].SPR.PPE_TLB_Index_Hint;
		break;
	case SPR_HSPRG0:
		value = hCore->ppuThread[hCore->currentThread].SPR.HSPRG0;
		break;
	case SPR_HSPRG1:
		value = hCore->ppuThread[hCore->currentThread].SPR.HSPRG1;
		break;
	case SPR_TSCR:
		value = hCore->SPR.TSCR;
		break;
	case SPR_TTR:
		value = hCore->SPR.TTR;
		break;
	case SPR_PVR:
		value = hCore->SPR.PVR.PVR_Hex;
		break;
	case SPR_SPRG0:
		value = hCore->ppuThread[hCore->currentThread].SPR.SPRG0;
		break;
	case SPR_SPRG1:
		value = hCore->ppuThread[hCore->currentThread].SPR.SPRG1;
		break;
	case SPR_SPRG3:
		value = hCore->ppuThread[hCore->currentThread].SPR.SPRG3;
		break;
	case SPR_SRR0:
		value = hCore->ppuThread[hCore->currentThread].SPR.SRR0;
		break;
	case SPR_SRR1:
		value = hCore->ppuThread[hCore->currentThread].SPR.SRR1;
		break;
	case SPR_XER:
		value = hCore->ppuThread[hCore->currentThread].SPR.XER.XER_Hex;
		break;
	case SPR_DSISR:
		value = hCore->ppuThread[hCore->currentThread].SPR.DSISR;
		break;
	case SPR_DAR:
		value = hCore->ppuThread[hCore->currentThread].SPR.DAR;
		break;
	case SPR_TB:
		value = hCore->SPR.TB;
		break;
	case SPR_TBL_RO:
		value = hCore->SPR.TB;
		break;
	case SPR_TBU_RO:
		value = (hCore->SPR.TB & 0xFFFFFFFF00000000);
		break;
	case SPR_DABR:
		value = hCore->ppuThread[hCore->currentThread].SPR.DABR;
		break;
	case SPR_CTRLRD:
		value = hCore->SPR.CTRL;
		break;
	default:
		std::cout << "mfspr: Unknown SPR! " << sprNum << std::endl;
		break;
	}

	hCore->ppuThread[hCore->currentThread].GPR[rS] = value;
}	

void PPCInterpreter::PPCInterpreter_mtspr(PPU_STATE* hCore)
{
	XFX_FORM_rD_spr;
	switch (spr)
	{
	case SPR_DEC:
		hCore->ppuThread[hCore->currentThread].SPR.DEC = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_SDR1:
		hCore->SPR.SDR1 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_DAR:
		hCore->ppuThread[hCore->currentThread].SPR.DAR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_DSISR:
		hCore->ppuThread[hCore->currentThread].SPR.DSISR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_CTR:
		hCore->ppuThread[hCore->currentThread].SPR.CTR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_LR:
		hCore->ppuThread[hCore->currentThread].SPR.LR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_LPCR:
		hCore->SPR.LPCR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_HID0:
		hCore->SPR.HID0 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_HID1:
		hCore->SPR.HID1 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_HID4:
		hCore->SPR.HID4 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_HID6:
		hCore->SPR.HID6 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_SRR0:
		hCore->ppuThread[hCore->currentThread].SPR.SRR0 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_SRR1:
		hCore->ppuThread[hCore->currentThread].SPR.SRR1 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_HRMOR:
		hCore->SPR.HRMOR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_PpeTlbIndex:
		hCore->SPR.PPE_TLB_Index = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_PpeTlbRpn:
		hCore->SPR.PPE_TLB_RPN = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_PpeTlbVpn:
		hCore->SPR.PPE_TLB_VPN = hCore->ppuThread[hCore->currentThread].GPR[rD];
		mmuAddTlbEntry(hCore);
		break;
	case SPR_TTR:
		hCore->SPR.TTR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_TSCR:
		hCore->SPR.TSCR = (u32)hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_HSPRG0:
		hCore->ppuThread[hCore->currentThread].SPR.HSPRG0 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_HSPRG1:
		hCore->ppuThread[hCore->currentThread].SPR.HSPRG1 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_CTRLWR:
		hCore->SPR.CTRL = (u32)hCore->ppuThread[hCore->currentThread].GPR[rD]; // Also do the write on SPR_CTRLRD
		break;
	case SPR_RMOR:
		hCore->SPR.RMOR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_HDEC:
		hCore->SPR.HDEC = (u32)hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_LPIDR:
		hCore->SPR.LPIDR = (u32)hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_SPRG0:
		hCore->ppuThread[hCore->currentThread].SPR.SPRG0 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_SPRG1:
		hCore->ppuThread[hCore->currentThread].SPR.SPRG1 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_SPRG3:
		hCore->ppuThread[hCore->currentThread].SPR.SPRG3 = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_DABR:
		hCore->ppuThread[hCore->currentThread].SPR.DABR = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_DABRX:
		hCore->ppuThread[hCore->currentThread].SPR.DABRX = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_XER:
		hCore->ppuThread[hCore->currentThread].SPR.XER.XER_Hex = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_TBL_WO:
		hCore->SPR.TB = hCore->ppuThread[hCore->currentThread].GPR[rD];
		break;
	case SPR_TBU_WO:
		hCore->SPR.TB = hCore->SPR.TB |= (hCore->ppuThread[hCore->currentThread].GPR[rD] << 32);
		break;
	default:
		std::cout << hCore->ppuName << " SPR " << std::dec << spr << " = 0x" << std::hex << hCore->ppuThread[hCore->currentThread].GPR[rD] << std::endl;
		break;
	}
}

void PPCInterpreter::PPCInterpreter_mfmsr(PPU_STATE* hCore)
{
	X_FORM_rD;
	hCore->ppuThread[hCore->currentThread].GPR[rD] = 
		hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex;
}

void PPCInterpreter::PPCInterpreter_mtmsr(PPU_STATE* hCore)
{
	X_FORM_rS;

	hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
		hCore->ppuThread[hCore->currentThread].GPR[rS];
}

void PPCInterpreter::PPCInterpreter_mtmsrd(PPU_STATE* hCore)
{
	X_FORM_rS_L;

	u64 qwMSRMask;
	u64 qwMSRSet;

	if (L == 0)
	{
		qwMSRMask = QMASK(0, 2) | QMASK(4, 50) | QMASK(52, 63);

		qwMSRSet = hCore->ppuThread[hCore->currentThread].GPR[rS] & (QMASK(1, 2) | QMASK(4, 47) | QMASK(49, 50) | QMASK(52, 57) | QMASK(60, 63));

		if (hCore->ppuThread[hCore->currentThread].GPR[rS] & QMASK(0, 1))
		{
			qwMSRSet |= QMASK(0, 0);
		}

		if (hCore->ppuThread[hCore->currentThread].GPR[rS] & QMASK(48, 49))
		{
			qwMSRSet |= QMASK(48, 48);
		}

		if (hCore->ppuThread[hCore->currentThread].GPR[rS] & (QMASK(49, 49) | QMASK(58, 58)))
		{
			qwMSRSet |= QMASK(58, 58);
		}

		if (hCore->ppuThread[hCore->currentThread].GPR[rS] & (QMASK(49, 49) | QMASK(59, 59)))
		{
			qwMSRSet |= QMASK(59, 59);
		}
	}
	else // L==1 
	{
		qwMSRMask = QMASK(48, 48) | QMASK(62, 62);
		qwMSRSet = hCore->ppuThread[hCore->currentThread].GPR[rS] & qwMSRMask;
	}

	/* Highly deficient */
	hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex = qwMSRSet | (hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex & ~qwMSRMask);
}
