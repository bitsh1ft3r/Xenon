#pragma once

#include "PPCInternal.h"
#include "Core/XCPU/PPU/PowerPC.h"

namespace PPCInterpreter {
//
// Instruction definitions
//

// Stubs
extern void PPCInterpreter_invalid(PPU_STATE* hCore);
extern void PPCInterpreter_known_unimplemented(const char* name, PPU_STATE* hCore);

// ALU
extern void PPCInterpreter_addx(PPU_STATE *hCore);
extern void PPCInterpreter_addex(PPU_STATE *hCore);
extern void PPCInterpreter_addi(PPU_STATE *hCore);
extern void PPCInterpreter_addic(PPU_STATE *hCore);
extern void PPCInterpreter_addis(PPU_STATE *hCore);
extern void PPCInterpreter_addzex(PPU_STATE *hCore);
extern void PPCInterpreter_andx(PPU_STATE *hCore);
extern void PPCInterpreter_andcx(PPU_STATE *hCore);
extern void PPCInterpreter_andi(PPU_STATE *hCore);
extern void PPCInterpreter_andis(PPU_STATE* hCore);
extern void PPCInterpreter_cmp(PPU_STATE *hCore);
extern void PPCInterpreter_cmpi(PPU_STATE *hCore);
extern void PPCInterpreter_cmpl(PPU_STATE *hCore);
extern void PPCInterpreter_cmpli(PPU_STATE *hCore);
extern void PPCInterpreter_cntlzdx(PPU_STATE *hCore);
extern void PPCInterpreter_cntlzwx(PPU_STATE *hCore);
extern void PPCInterpreter_crand(PPU_STATE *hCore);
extern void PPCInterpreter_crandc(PPU_STATE *hCore);
extern void PPCInterpreter_creqv(PPU_STATE *hCore);
extern void PPCInterpreter_crnand(PPU_STATE *hCore);
extern void PPCInterpreter_crnor(PPU_STATE *hCore);
extern void PPCInterpreter_cror(PPU_STATE *hCore);
extern void PPCInterpreter_crorc(PPU_STATE *hCore);
extern void PPCInterpreter_crxor(PPU_STATE *hCore);
extern void PPCInterpreter_divdx(PPU_STATE *hCore);
extern void PPCInterpreter_divdux(PPU_STATE *hCore);
extern void PPCInterpreter_divwx(PPU_STATE *hCore);
extern void PPCInterpreter_divwux(PPU_STATE *hCore);
extern void PPCInterpreter_isync(PPU_STATE *hCore);
extern void PPCInterpreter_extsbx(PPU_STATE *hCore);
extern void PPCInterpreter_extshx(PPU_STATE *hCore);
extern void PPCInterpreter_extswx(PPU_STATE *hCore);
extern void PPCInterpreter_mcrf(PPU_STATE *hCore);
extern void PPCInterpreter_mfocrf(PPU_STATE *hCore);
extern void PPCInterpreter_mftb(PPU_STATE *hCore);
extern void PPCInterpreter_mtocrf(PPU_STATE *hCore);
extern void PPCInterpreter_mulli(PPU_STATE *hCore);
extern void PPCInterpreter_mulldx(PPU_STATE *hCore);
extern void PPCInterpreter_mullwx(PPU_STATE *hCore);
extern void PPCInterpreter_mulhwux(PPU_STATE *hCore);
extern void PPCInterpreter_mulhdux(PPU_STATE *hCore);
extern void PPCInterpreter_nandx(PPU_STATE *hCore);
extern void PPCInterpreter_negx(PPU_STATE *hCore);
extern void PPCInterpreter_norx(PPU_STATE *hCore);
extern void PPCInterpreter_orcx(PPU_STATE *hCore);
extern void PPCInterpreter_ori(PPU_STATE *hCore);
extern void PPCInterpreter_oris(PPU_STATE *hCore);
extern void PPCInterpreter_orx(PPU_STATE *hCore);
extern void PPCInterpreter_rldicx(PPU_STATE *hCore);
extern void PPCInterpreter_rldcrx(PPU_STATE *hCore);
extern void PPCInterpreter_rldiclx(PPU_STATE *hCore);
extern void PPCInterpreter_rldicrx(PPU_STATE *hCore);
extern void PPCInterpreter_rldimix(PPU_STATE *hCore);
extern void PPCInterpreter_rlwimix(PPU_STATE *hCore);
extern void PPCInterpreter_rlwnmx(PPU_STATE *hCore);
extern void PPCInterpreter_rlwinmx(PPU_STATE *hCore);
extern void PPCInterpreter_sldx(PPU_STATE *hCore);
extern void PPCInterpreter_slwx(PPU_STATE *hCore);
extern void PPCInterpreter_sradx(PPU_STATE *hCore);
extern void PPCInterpreter_sradix(PPU_STATE *hCore);
extern void PPCInterpreter_srawx(PPU_STATE *hCore);
extern void PPCInterpreter_srawix(PPU_STATE *hCore);
extern void PPCInterpreter_srdx(PPU_STATE *hCore);
extern void PPCInterpreter_srwx(PPU_STATE *hCore);
extern void PPCInterpreter_subfcx(PPU_STATE *hCore);
extern void PPCInterpreter_subfx(PPU_STATE *hCore);
extern void PPCInterpreter_subfex(PPU_STATE *hCore);
extern void PPCInterpreter_subfic(PPU_STATE *hCore);
extern void PPCInterpreter_xori(PPU_STATE *hCore);
extern void PPCInterpreter_xoris(PPU_STATE *hCore);
extern void PPCInterpreter_xorx(PPU_STATE *hCore);

// Program control
extern void PPCInterpreter_b(PPU_STATE *hCore);
extern void PPCInterpreter_bc(PPU_STATE *hCore);
extern void PPCInterpreter_bcctr(PPU_STATE *hCore);
extern void PPCInterpreter_bclr(PPU_STATE *hCore);

// System instructions
extern void PPCInterpreter_eieio(PPU_STATE *hCore);
extern void PPCInterpreter_sc(PPU_STATE *hCore);
extern void PPCInterpreter_slbia(PPU_STATE *hCore);
extern void PPCInterpreter_slbie(PPU_STATE *hCore);
extern void PPCInterpreter_slbmte(PPU_STATE *hCore);
extern void PPCInterpreter_rfid(PPU_STATE *hCore);
extern void PPCInterpreter_tw(PPU_STATE *hCore);
extern void PPCInterpreter_twi(PPU_STATE *hCore);
extern void PPCInterpreter_tdi(PPU_STATE *hCore);
extern void PPCInterpreter_tlbie(PPU_STATE *hCore);
extern void PPCInterpreter_tlbiel(PPU_STATE *hCore);
extern void PPCInterpreter_tlbsync(PPU_STATE *hCore);
extern void PPCInterpreter_mfspr(PPU_STATE *hCore);
extern void PPCInterpreter_mtspr(PPU_STATE *hCore);
extern void PPCInterpreter_mfmsr(PPU_STATE *hCore);
extern void PPCInterpreter_mtmsr(PPU_STATE *hCore);
extern void PPCInterpreter_mtmsrd(PPU_STATE *hCore);
extern void PPCInterpreter_sync(PPU_STATE *hCore);

// Cache Management
extern void PPCInterpreter_dcbf(PPU_STATE *hCore);
extern void PPCInterpreter_dcbi(PPU_STATE* hCore);
extern void PPCInterpreter_dcbt(PPU_STATE* hCore);
extern void PPCInterpreter_dcbtst(PPU_STATE* hCore);
extern void PPCInterpreter_dcbst(PPU_STATE *hCore);
extern void PPCInterpreter_dcbz(PPU_STATE *hCore);
extern void PPCInterpreter_icbi(PPU_STATE *hCore);

//
// FPU
//
extern void PPCInterpreter_mffsx(PPU_STATE* hCore);
extern void PPCInterpreter_mtfsfx(PPU_STATE* hCore);

//
// Load/Store
//

// Store Byte
extern void PPCInterpreter_stb(PPU_STATE *hCore);
extern void PPCInterpreter_stbu(PPU_STATE *hCore);
extern void PPCInterpreter_stbux(PPU_STATE *hCore);
extern void PPCInterpreter_stbx(PPU_STATE *hCore);

// Store Halfword
extern void PPCInterpreter_sth(PPU_STATE *hCore);
extern void PPCInterpreter_sthbrx(PPU_STATE *hCore);
extern void PPCInterpreter_sthu(PPU_STATE *hCore);
extern void PPCInterpreter_sthux(PPU_STATE *hCore);
extern void PPCInterpreter_sthx(PPU_STATE *hCore);

// Store String Word
extern void PPCInterpreter_stswi(PPU_STATE *hCore);

// Store Multiple Word
extern void PPCInterpreter_stmw(PPU_STATE* hCore);

// Store Word
extern void PPCInterpreter_stw(PPU_STATE *hCore);
extern void PPCInterpreter_stwbrx(PPU_STATE *hCore);
extern void PPCInterpreter_stwcx(PPU_STATE *hCore);
extern void PPCInterpreter_stwu(PPU_STATE *hCore);
extern void PPCInterpreter_stwux(PPU_STATE *hCore);
extern void PPCInterpreter_stwx(PPU_STATE *hCore);

// Store Doubleword
extern void PPCInterpreter_std(PPU_STATE *hCore);
extern void PPCInterpreter_stdcx(PPU_STATE *hCore);
extern void PPCInterpreter_stdu(PPU_STATE *hCore);
extern void PPCInterpreter_stdux(PPU_STATE *hCore);
extern void PPCInterpreter_stdx(PPU_STATE *hCore);

// Store Floating
extern void PPCInterpreter_stfd(PPU_STATE *hCore);

// Load Byte
extern void PPCInterpreter_lbz(PPU_STATE *hCore);
extern void PPCInterpreter_lbzu(PPU_STATE *hCore);
extern void PPCInterpreter_lbzux(PPU_STATE *hCore);
extern void PPCInterpreter_lbzx(PPU_STATE *hCore);

// Load Halfword
extern void PPCInterpreter_lha(PPU_STATE *hCore);
extern void PPCInterpreter_lhau(PPU_STATE *hCore);
extern void PPCInterpreter_lhax(PPU_STATE *hCore);
extern void PPCInterpreter_lhbrx(PPU_STATE *hCore);
extern void PPCInterpreter_lhz(PPU_STATE *hCore);
extern void PPCInterpreter_lhzu(PPU_STATE *hCore);
extern void PPCInterpreter_lhzux(PPU_STATE *hCore);
extern void PPCInterpreter_lhzx(PPU_STATE *hCore);

// Load String Word
extern void PPCInterpreter_lswi(PPU_STATE *hCore);

// Load Multiple Word
extern void PPCInterpreter_lmw(PPU_STATE* hCore);

// Load Word
extern void PPCInterpreter_lwa(PPU_STATE *hCore);
extern void PPCInterpreter_lwax(PPU_STATE *hCore);
extern void PPCInterpreter_lwarx(PPU_STATE *hCore);
extern void PPCInterpreter_lwbrx(PPU_STATE *hCore);
extern void PPCInterpreter_lwz(PPU_STATE *hCore);
extern void PPCInterpreter_lwzu(PPU_STATE *hCore);
extern void PPCInterpreter_lwzux(PPU_STATE *hCore);
extern void PPCInterpreter_lwzx(PPU_STATE *hCore);

// Load Doubleword
extern void PPCInterpreter_ld(PPU_STATE *hCore);
extern void PPCInterpreter_ldarx(PPU_STATE *hCore);
extern void PPCInterpreter_ldbrx(PPU_STATE *hCore);
extern void PPCInterpreter_ldu(PPU_STATE *hCore);
extern void PPCInterpreter_ldux(PPU_STATE *hCore);
extern void PPCInterpreter_ldx(PPU_STATE *hCore);

//
// Load Floating
//

extern void PPCInterpreter_lfd(PPU_STATE *hCore);
extern void PPCInterpreter_lfs(PPU_STATE *hCore);

}
