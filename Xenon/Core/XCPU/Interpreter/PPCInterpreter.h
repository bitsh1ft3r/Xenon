#pragma once

#include "Xenon/Core/XCPU/XCPUContext.h"
#include "Xenon/Core/XCPU/Interpreter/PowerPCInternal.h"
#include "Xenon/Core/XCPU/Interpreter/PowerPC_Instruction.h"

namespace PPCInterpreter
{
	extern XCPUContext* intXCPUContext;

	// Interpreter helpers
	u64 ppcAddCarrying(PPCState* hCore, u64 op1, u64 op2, u64 carryBit);
	void ppcMul64(u64 operand0, u64 operand1, u64* u64High, u64* u64Low);
	void ppcMul64Signed(u64 operand0, u64 operand1, u64* u64High, u64* u64Low);
	
	//
	// Condition Register
	//
	
	// Compare Unsigned
	u32 CRCompU(PPCState* hCore, u64 num1, u64 num2);
	// Compare Signed 32 bits
	u32 CRCompS32(PPCState* hCore, u32 num1, u32 num2);
	// Compare Signed 64 bits
	u32 CRCompS64(PPCState* hCore, u64 num1, u64 num2);
	// Compare Unsigned
	u32 CRCompS(PPCState* hCore, u64 num1, u64 num2);
	// Condition register Update
	void ppcUpdateCR(PPCState* hCore, s8 crNum, u32 crValue);

	// Entry point
	void ppcInterpreterExecute(XCPUContext* cpuContext);

	// Single instruction execution
	void ppcExecuteSingleInstruction(PPCState* hCore);

	//
	// Exceptions
	//

	void ppcDataSegmentException(PPCState* hCore, u64 EA);
	void ppcInstSegmentException(PPCState* hCore);
	void ppcISIException(PPCState* hCore, u64 SSR1);
	void ppcDSIException(PPCState* hCore, u64 EA, u32 ISR);

	//
	// MMU
	//

	bool MMUTranslateAddress(u64* EA, PPCState* hCoreState);
	u8 mmuGetPageSize(PPCState* hCore, bool L, u8 LP);
	void mmuAddTlbEntry(PPCState* hCore);
	bool mmuSearchTlbEntry(PPCState* hCore, u64* RPN, u64 VPN, u8 p, bool LP);

	u64 MMURead(XCPUContext* cpuContext, u64 EA, s8 byteCount);
	void MMUWrite(XCPUContext* cpuContext, u64 data, u64 EA, s8 byteCount, bool cacheStore = false);

	u8	MMURead8(u64 EA);
	u16 MMURead16(u64 EA);
	u32 MMURead32(u64 EA);
	u64 MMURead64(u64 EA);

	void MMUWrite8(u64 EA, u8 data);
	void MMUWrite16(u64 EA, u16 data);
	void MMUWrite32(u64 EA, u32 data);
	void MMUWrite64(u64 EA, u64 data);

	//
	// Instruction definitions
	//

	// ALU
	void PPCInterpreter_addx(PPCState* hCore);
	void PPCInterpreter_addc(PPCState* hCore);
	void PPCInterpreter_adde(PPCState* hCore);
	void PPCInterpreter_addi(PPCState* hCore);
	void PPCInterpreter_addic(PPCState* hCore);
	void PPCInterpreter_addic_rc(PPCState* hCore);
	void PPCInterpreter_addis(PPCState* hCore);
	void PPCInterpreter_addme(PPCState* hCore, u32 instrData);
	void PPCInterpreter_addze(PPCState* hCore, u32 instrData);
	void PPCInterpreter_and(PPCState* hCore);
	void PPCInterpreter_andc(PPCState* hCore);
	void PPCInterpreter_andi(PPCState* hCore);
	void PPCInterpreter_andis(PPCState* hCore);
	void PPCInterpreter_cmp(PPCState* hCore);
	void PPCInterpreter_cmpi(PPCState* hCore);
	void PPCInterpreter_cmpl(PPCState* hCore);
	void PPCInterpreter_cmpli(PPCState* hCore);
	void PPCInterpreter_cntlzd(PPCState* hCore, u32 instrData);
	void PPCInterpreter_cntlzw(PPCState* hCore);
	void PPCInterpreter_crand(PPCState* hCore, u32 instrData);
	void PPCInterpreter_crandc(PPCState* hCore, u32 instrData);
	void PPCInterpreter_creqv(PPCState* hCore, u32 instrData);
	void PPCInterpreter_crnand(PPCState* hCore, u32 instrData);
	void PPCInterpreter_crnor(PPCState* hCore, u32 instrData);
	void PPCInterpreter_cror(PPCState* hCore, u32 instrData);
	void PPCInterpreter_crorc(PPCState* hCore, u32 instrData);
	void PPCInterpreter_crxor(PPCState* hCore, u32 instrData);
	void PPCInterpreter_divd(PPCState* hCore, u32 instrData);
	void PPCInterpreter_divdu(PPCState* hCore);
	void PPCInterpreter_divw(PPCState* hCore, u32 instrData);
	void PPCInterpreter_divwu(PPCState* hCore, u32 instrData);
	void PPCInterpreter_isync(PPCState* hCore);
	void PPCInterpreter_extsbx(PPCState* hCore);
	void PPCInterpreter_extshx(PPCState* hCore);
	void PPCInterpreter_extswx(PPCState* hCore);
	void PPCInterpreter_mfcr(PPCState* hCore);
	void PPCInterpreter_mftb(PPCState* hCore);
	void PPCInterpreter_mtcrf(PPCState* hCore);
	void PPCInterpreter_mulli(PPCState* hCore);
	void PPCInterpreter_mulldx(PPCState* hCore);
	void PPCInterpreter_mullw(PPCState* hCore);
	void PPCInterpreter_mulhdux(PPCState* hCore);
	void PPCInterpreter_negx(PPCState* hCore);
	void PPCInterpreter_norx(PPCState* hCore);
	void PPCInterpreter_ori(PPCState* hCore);
	void PPCInterpreter_oris(PPCState* hCore);
	void PPCInterpreter_orx(PPCState* hCore);
	void PPCInterpreter_rldicx(PPCState* hCore);
	void PPCInterpreter_rldcrx(PPCState* hCore);
	void PPCInterpreter_rldiclx(PPCState* hCore);
	void PPCInterpreter_rldicrx(PPCState* hCore);
	void PPCInterpreter_rldimix(PPCState* hCore);
	void PPCInterpreter_rlwimix(PPCState* hCore);
	void PPCInterpreter_rlwnmx(PPCState* hCore);
	void PPCInterpreter_rlwinmx(PPCState* hCore);
	void PPCInterpreter_slbia(PPCState* hCore);
	void PPCInterpreter_sldx(PPCState* hCore);
	void PPCInterpreter_slwx(PPCState* hCore);
	void PPCInterpreter_srawix(PPCState* hCore);
	void PPCInterpreter_srdx(PPCState* hCore);
	void PPCInterpreter_srwx(PPCState* hCore);
	void PPCInterpreter_subfcx(PPCState* hCore);
	void PPCInterpreter_subfx(PPCState* hCore);
	void PPCInterpreter_subfex(PPCState* hCore);
	void PPCInterpreter_subfic(PPCState* hCore);
	void PPCInterpreter_xori(PPCState* hCore);
	void PPCInterpreter_xorx(PPCState* hCore);

	// Program control
	void PPCInterpreter_b(PPCState* hCore);
	void PPCInterpreter_bc(PPCState* hCore);
	void PPCInterpreter_bcctr(PPCState* hCore);
	void PPCInterpreter_bclr(PPCState* hCore);

	// System instructions
	void PPCInterpreter_slbmte(PPCState* hCore);
	void PPCInterpreter_rfid(PPCState* hCore);
	void PPCInterpreter_tlbiel(PPCState* hCore);
	void PPCInterpreter_mfspr(PPCState* hCore);
	void PPCInterpreter_mtspr(PPCState* hCore);
	void PPCInterpreter_mfmsr(PPCState* hCore);
	void PPCInterpreter_mtmsrd(PPCState* hCore);

	// Cache Management
	void PPCInterpreter_dcbf(PPCState* hCore, u32 instrData);
	void PPCInterpreter_dcbi(PPCState* hCore, u32 instrData);
	void PPCInterpreter_dcbst(PPCState* hCore);
	void PPCInterpreter_dcbt(PPCState* hCore, u32 instrData);
	void PPCInterpreter_dcbtst(PPCState* hCore, u32 instrData);
	void PPCInterpreter_dcbz(PPCState* hCore);

	//
	// Load/Store
	//

	// Store Byte
	void PPCInterpreter_stb(PPCState* hCore);
	void PPCInterpreter_stbu(PPCState* hCore);
	void PPCInterpreter_stbux(PPCState* hCore);
	void PPCInterpreter_stbx(PPCState* hCore);

	// Store Halfword
	void PPCInterpreter_sth(PPCState* hCore);
	void PPCInterpreter_sthbrx(PPCState* hCore);
	void PPCInterpreter_sthu(PPCState* hCore);
	void PPCInterpreter_sthux(PPCState* hCore);
	void PPCInterpreter_sthx(PPCState* hCore);

	// Store Word
	void PPCInterpreter_stmw(PPCState* hCore);
	void PPCInterpreter_stswi(PPCState* hCore);
	void PPCInterpreter_stswx(PPCState* hCore);
	void PPCInterpreter_stw(PPCState* hCore);
	void PPCInterpreter_stwbrx(PPCState* hCore);
	void PPCInterpreter_stwcx(PPCState* hCore);
	void PPCInterpreter_stwu(PPCState* hCore);
	void PPCInterpreter_stwux(PPCState* hCore);
	void PPCInterpreter_stwx(PPCState* hCore);

	// Store Doubleword
	void PPCInterpreter_std(PPCState* hCore);
	void PPCInterpreter_stdcx(PPCState* hCore);
	void PPCInterpreter_stdu(PPCState* hCore);
	void PPCInterpreter_stdux(PPCState* hCore);
	void PPCInterpreter_stdx(PPCState* hCore);

	// Load Byte
	void PPCInterpreter_lbz(PPCState* hCore);
	void PPCInterpreter_lbzu(PPCState* hCore);
	void PPCInterpreter_lbzux(PPCState* hCore);
	void PPCInterpreter_lbzx(PPCState* hCore);

	// Load Halfword
	void PPCInterpreter_lha(PPCState* hCore);
	void PPCInterpreter_lhax(PPCState* hCore);
	void PPCInterpreter_lhbrx(PPCState* hCore);
	void PPCInterpreter_lhz(PPCState* hCore);
	void PPCInterpreter_lhzu(PPCState* hCore);
	void PPCInterpreter_lhzux(PPCState* hCore);
	void PPCInterpreter_lhzx(PPCState* hCore);

	// Load Word
	void PPCInterpreter_lwbrx(PPCState* hCore);
	void PPCInterpreter_lwz(PPCState* hCore);
	void PPCInterpreter_lwzu(PPCState* hCore);
	void PPCInterpreter_lwzux(PPCState* hCore);
	void PPCInterpreter_lwzx(PPCState* hCore);

	// Load Doubleword
	void PPCInterpreter_ld(PPCState* hCore);
	void PPCInterpreter_ldarx(PPCState* hCore);
	void PPCInterpreter_ldbrx(PPCState* hCore);
	void PPCInterpreter_ldu(PPCState* hCore);
	void PPCInterpreter_ldux(PPCState* hCore);
	void PPCInterpreter_ldx(PPCState* hCore);
}