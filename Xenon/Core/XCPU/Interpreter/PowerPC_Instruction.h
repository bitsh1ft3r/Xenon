#pragma once

#include "Xenon/Base/TypeDefs.h"

//
// PowerPC Instruction definitions
//

enum class PPCInstruction
{
	invalidInstruction,
	addcx,
	addex,
	addi,
	addic,
	addicx,
	addis,
	addmex,
	addx,
	addzex,
	andcx,
	andix,
	andisx,
	andx,
	bcctrx,
	bclrx,
	bcx,
	bx,
	cmp,
	cmpi,
	cmpl,
	cmpli,
	cntlzdx,
	cntlzwx,
	crand,
	crandc,
	creqv,
	crnand,
	crnor,
	cror,
	crorc,
	crxor,
	dcbf,
	dcbi,
	dcbst,
	dcbt,
	dcbtst,
	dcbz,
	divdux,
	divdx,
	divwux,
	divwx,
	eciwx,
	ecowx,
	eieio,
	eqvx,
	extsbx,
	extshx,
	extswx,
	fabsx,
	faddsx,
	faddx,
	fcfidx,
	fcmpo,
	fcmpu,
	fctidx,
	fctidzx,
	fctiwx,
	fctiwzx,
	fdivsx,
	fdivx,
	fmaddsx,
	fmaddx,
	fmrx,
	fmsubsx,
	fmsubx,
	fmulsx,
	fmulx,
	fnabsx,
	fnegx,
	fnmaddsx,
	fnmaddx,
	fnmsubsx,
	fnmsubx,
	fresx,
	frspx,
	frsqrtex,
	fselx,
	fsqrtsx,
	fsqrtx,
	fsubsx,
	fsubx,
	icbi,
	isync,
	lbz,
	lbzu,
	lbzux,
	lbzx,
	ld,
	ldarx,
	ldbrx,
	ldu,
	ldux,
	ldx,
	lfd,
	lfdu,
	lfdux,
	lfdx,
	lfs,
	lfsu,
	lfsux,
	lfsx,
	lha,
	lhau,
	lhaux,
	lhax,
	lhbrx,
	lhz,
	lhzu,
	lhzux,
	lhzx,
	lmw,
	lswi,
	lswx,
	lwa,
	lwarx,
	lwaux,
	lwax,
	lwbrx,
	lwz,
	lwzu,
	lwzux,
	lwzx,
	mcrf,
	mcrfs,
	mcrxr,
	mfcr,
	mffsx,
	mfmsr,
	mfocrf,
	mfspr,
	mfsr,
	mfsrin,
	mftb,
	mtcrf,
	mtfsb0x,
	mtfsb1x,
	mtfsfix,
	mtfsfx,
	mtmsr,
	mtmsrd,
	mtocrf,
	mtspr,
	mtsr,
	mtsrin,
	mulhdux,
	mulhdx,
	mulhwux,
	mulhwx,
	mulldx,
	mulli,
	mullwx,
	nandx,
	negx,
	norx,
	orcx,
	ori,
	oris,
	orx,
	rfid,
	rldclx,
	rldcrx,
	rldiclx,
	rldicrx,
	rldicx,
	rldimix,
	rlwimix,
	rlwinmx,
	rlwnmx,
	sc,
	slbia,
	slbie,
	slbmfee,
	slbmfev,
	slbmte,
	sldx,
	slwx,
	sradix,
	sradx,
	srawix,
	srawx,
	srdx,
	srwx,
	stb,
	stbu,
	stbux,
	stbx,
	std,
	stdcx,
	stdu,
	stdux,
	stdx,
	stfd,
	stdbrx,
	stfdu,
	stfdux,
	stfdx,
	stfiwx,
	stfs,
	stfsu,
	stfsux,
	stfsx,
	sth,
	sthbrx,
	sthu,
	sthux,
	sthx,
	stmw,
	stswi,
	stswx,
	stw,
	stwbrx,
	stwcx,
	stwu,
	stwux,
	stwx,
	subfcx,
	subfex,
	subfic,
	subfmex,
	subfx,
	subfzex,
	sync,
	td,
	tdi,
	tlbia,
	tlbie,
	tlbiel,
	tlbsync,
	tw,
	twi,
	xori,
	xoris,
	xorx
};

namespace PPCInterpreter {
	PPCInstruction getOpcode(u32 instrData);
	std::string getOpcodeName(u32 instrData);
}