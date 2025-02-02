// Copyright 2025 Xenon Emulator Project

#pragma once

#define CORE_DUMP
#ifdef CORE_DUMP 
//#define DUMP_SPRS
//#define DUMP_FPU 
#define DUMP_MSR
//#define DUMP_VPU
#define DUMP_GPRS
#define DUMP_CRS
//#define DUMP_SLBS
//#define DUMP_EXCPR
#endif

#include "PPCInternal.h"
#include "PPC_Instruction.h"

#include "Core/RootBus/RootBus.h"
#include "Core/XCPU/PPU/PowerPC.h"

namespace PPCInterpreter {
#ifdef CORE_DUMP
extern bool startCoredump;
#endif
extern RootBus *sysBus;
extern XENON_CONTEXT *intXCPUContext;

//
//	Basic Block Loading, debug symbols and stuff.
//
struct KD_SYMBOLS_INFO {
  u32 BaseOfDll;
  u32 ProcessId;
  u32 CheckSum;
  u32 SizeOfImage;
};

void ppcDebugLoadImageSymbols(PPU_STATE *hCore, u64 moduleNameAddress,
                              u64 moduleInfoAddress);
void ppcDebugUnloadImageSymbols(PPU_STATE *hCore, u64 moduleNameAddress,
                                u64 moduleInfoAddress);

// Interpreter helpers
u64 ppcAddCarrying(PPU_STATE *hCore, u64 op1, u64 op2, u64 carryBit);
void ppcMul64(u64 operand0, u64 operand1, u64 *u64High, u64 *u64Low);
void ppcMul64Signed(u64 operand0, u64 operand1, u64 *u64High, u64 *u64Low);

//
// Condition Register
//

// Compare Unsigned
u32 CRCompU(PPU_STATE *hCore, u64 num1, u64 num2);
// Compare Signed 32 bits
u32 CRCompS32(PPU_STATE *hCore, u32 num1, u32 num2);
// Compare Signed 64 bits
u32 CRCompS64(PPU_STATE *hCore, u64 num1, u64 num2);
// Compare Unsigned
u32 CRCompS(PPU_STATE *hCore, u64 num1, u64 num2);
// Condition register Update
void ppcUpdateCR(PPU_STATE *hCore, s8 crNum, u32 crValue);

// Single instruction execution
void ppcExecuteSingleInstruction(PPU_STATE *hCore);

//
// Exceptions
//

#define TRAP_TYPE_SRR1_TRAP_FPU 43
#define TRAP_TYPE_SRR1_TRAP_ILL 44
#define TRAP_TYPE_SRR1_TRAP_PRIV 45
#define TRAP_TYPE_SRR1_TRAP_TRAP 46

void ppcResetException(PPU_STATE *hCore);
void ppcInterpreterTrap(PPU_STATE *hCore, u32 trapNumber);
void ppcInstStorageException(PPU_STATE *hCore);
void ppcDataStorageException(PPU_STATE *hCore);
void ppcDataSegmentException(PPU_STATE *hCore);
void ppcInstSegmentException(PPU_STATE *hCore);
void ppcSystemCallException(PPU_STATE *hCore);
void ppcDecrementerException(PPU_STATE *hCore);
void ppcProgramException(PPU_STATE *hCore);
void ppcExternalException(PPU_STATE *hCore);

//
// MMU
//

bool MMUTranslateAddress(u64 *EA, PPU_STATE *hCoreState, bool memWrite);
u8 mmuGetPageSize(PPU_STATE *hCore, bool L, u8 LP);
void mmuAddTlbEntry(PPU_STATE *hCore);
bool mmuSearchTlbEntry(PPU_STATE *hCore, u64 *RPN, u64 VA, u64 VPN, u8 p,
                       bool LP);
void mmuReadString(PPU_STATE *hCore, u64 stringAddress, char *string,
                   u32 maxLenght);

// Security Engine Related
SECENG_ADDRESS_INFO mmuGetSecEngInfoFromAddress(u64 inputAddress);
u64 mmuContructEndAddressFromSecEngAddr(u64 inputAddress, bool *socAccess);

// Main R/W Routines.
u64 MMURead(XENON_CONTEXT *cpuContext, PPU_STATE *ppuState, u64 EA,
            s8 byteCount);
void MMUWrite(XENON_CONTEXT *cpuContext, PPU_STATE *ppuState, u64 data, u64 EA,
              s8 byteCount, bool cacheStore = false);

// Helper Read Routines.
u8 MMURead8(PPU_STATE *ppuState, u64 EA);
u16 MMURead16(PPU_STATE *ppuState, u64 EA);
u32 MMURead32(PPU_STATE *ppuState, u64 EA);
u64 MMURead64(PPU_STATE *ppuState, u64 EA);
// Helper Write Routines.
void MMUWrite8(PPU_STATE *ppuState, u64 EA, u8 data);
void MMUWrite16(PPU_STATE *ppuState, u64 EA, u16 data);
void MMUWrite32(PPU_STATE *ppuState, u64 EA, u32 data);
void MMUWrite64(PPU_STATE *ppuState, u64 EA, u64 data);

//
// Instruction definitions, only implemented instructions as of now.
//

// ALU
void PPCInterpreter_addx(PPU_STATE *hCore);
void PPCInterpreter_addex(PPU_STATE *hCore);
void PPCInterpreter_addi(PPU_STATE *hCore);
void PPCInterpreter_addic(PPU_STATE *hCore);
void PPCInterpreter_addic_rc(PPU_STATE *hCore);
void PPCInterpreter_addis(PPU_STATE *hCore);
void PPCInterpreter_addzex(PPU_STATE *hCore);
void PPCInterpreter_and(PPU_STATE *hCore);
void PPCInterpreter_andc(PPU_STATE *hCore);
void PPCInterpreter_andi(PPU_STATE *hCore);
void PPCInterpreter_andis(PPU_STATE *hCore);
void PPCInterpreter_cmp(PPU_STATE *hCore);
void PPCInterpreter_cmpi(PPU_STATE *hCore);
void PPCInterpreter_cmpl(PPU_STATE *hCore);
void PPCInterpreter_cmpli(PPU_STATE *hCore);
void PPCInterpreter_cntlzd(PPU_STATE *hCore);
void PPCInterpreter_cntlzw(PPU_STATE *hCore);
void PPCInterpreter_crand(PPU_STATE *hCore);
void PPCInterpreter_crandc(PPU_STATE *hCore);
void PPCInterpreter_creqv(PPU_STATE *hCore);
void PPCInterpreter_crnand(PPU_STATE *hCore);
void PPCInterpreter_crnor(PPU_STATE *hCore);
void PPCInterpreter_cror(PPU_STATE *hCore);
void PPCInterpreter_crorc(PPU_STATE *hCore);
void PPCInterpreter_crxor(PPU_STATE *hCore);
void PPCInterpreter_divd(PPU_STATE *hCore);
void PPCInterpreter_divdu(PPU_STATE *hCore);
void PPCInterpreter_divwx(PPU_STATE *hCore);
void PPCInterpreter_divwux(PPU_STATE *hCore);
void PPCInterpreter_isync(PPU_STATE *hCore);
void PPCInterpreter_extsbx(PPU_STATE *hCore);
void PPCInterpreter_extshx(PPU_STATE *hCore);
void PPCInterpreter_extswx(PPU_STATE *hCore);
void PPCInterpreter_mcrf(PPU_STATE *hCore);
void PPCInterpreter_mfcr(PPU_STATE *hCore);
void PPCInterpreter_mftb(PPU_STATE *hCore);
void PPCInterpreter_mtcrf(PPU_STATE *hCore);
void PPCInterpreter_mulli(PPU_STATE *hCore);
void PPCInterpreter_mulldx(PPU_STATE *hCore);
void PPCInterpreter_mullw(PPU_STATE *hCore);
void PPCInterpreter_mulhwux(PPU_STATE *hCore);
void PPCInterpreter_mulhdux(PPU_STATE *hCore);
void PPCInterpreter_nandx(PPU_STATE *hCore);
void PPCInterpreter_negx(PPU_STATE *hCore);
void PPCInterpreter_norx(PPU_STATE *hCore);
void PPCInterpreter_ori(PPU_STATE *hCore);
void PPCInterpreter_oris(PPU_STATE *hCore);
void PPCInterpreter_orx(PPU_STATE *hCore);
void PPCInterpreter_rldicx(PPU_STATE *hCore);
void PPCInterpreter_rldcrx(PPU_STATE *hCore);
void PPCInterpreter_rldiclx(PPU_STATE *hCore);
void PPCInterpreter_rldicrx(PPU_STATE *hCore);
void PPCInterpreter_rldimix(PPU_STATE *hCore);
void PPCInterpreter_rlwimix(PPU_STATE *hCore);
void PPCInterpreter_rlwnmx(PPU_STATE *hCore);
void PPCInterpreter_rlwinmx(PPU_STATE *hCore);
void PPCInterpreter_sldx(PPU_STATE *hCore);
void PPCInterpreter_slwx(PPU_STATE *hCore);
void PPCInterpreter_sradx(PPU_STATE *hCore);
void PPCInterpreter_sradix(PPU_STATE *hCore);
void PPCInterpreter_srawx(PPU_STATE *hCore);
void PPCInterpreter_srawix(PPU_STATE *hCore);
void PPCInterpreter_srdx(PPU_STATE *hCore);
void PPCInterpreter_srwx(PPU_STATE *hCore);
void PPCInterpreter_subfcx(PPU_STATE *hCore);
void PPCInterpreter_subfx(PPU_STATE *hCore);
void PPCInterpreter_subfex(PPU_STATE *hCore);
void PPCInterpreter_subfic(PPU_STATE *hCore);
void PPCInterpreter_xori(PPU_STATE *hCore);
void PPCInterpreter_xoris(PPU_STATE *hCore);
void PPCInterpreter_xorx(PPU_STATE *hCore);

// Program control
void PPCInterpreter_b(PPU_STATE *hCore);
void PPCInterpreter_bc(PPU_STATE *hCore);
void PPCInterpreter_bcctr(PPU_STATE *hCore);
void PPCInterpreter_bclr(PPU_STATE *hCore);

// System instructions
void PPCInterpreter_sc(PPU_STATE *hCore);
void PPCInterpreter_slbia(PPU_STATE *hCore);
void PPCInterpreter_slbie(PPU_STATE *hCore);
void PPCInterpreter_slbmte(PPU_STATE *hCore);
void PPCInterpreter_rfid(PPU_STATE *hCore);
void PPCInterpreter_tw(PPU_STATE *hCore);
void PPCInterpreter_twi(PPU_STATE *hCore);
void PPCInterpreter_tdi(PPU_STATE *hCore);
void PPCInterpreter_tlbiel(PPU_STATE *hCore);
void PPCInterpreter_mfspr(PPU_STATE *hCore);
void PPCInterpreter_mtspr(PPU_STATE *hCore);
void PPCInterpreter_mfmsr(PPU_STATE *hCore);
void PPCInterpreter_mtmsr(PPU_STATE *hCore);
void PPCInterpreter_mtmsrd(PPU_STATE *hCore);

// Cache Management
void PPCInterpreter_dcbst(PPU_STATE *hCore);
void PPCInterpreter_dcbz(PPU_STATE *hCore);

//
// Load/Store
//

// Store Byte
void PPCInterpreter_stb(PPU_STATE *hCore);
void PPCInterpreter_stbu(PPU_STATE *hCore);
void PPCInterpreter_stbux(PPU_STATE *hCore);
void PPCInterpreter_stbx(PPU_STATE *hCore);

// Store Halfword
void PPCInterpreter_sth(PPU_STATE *hCore);
void PPCInterpreter_sthbrx(PPU_STATE *hCore);
void PPCInterpreter_sthu(PPU_STATE *hCore);
void PPCInterpreter_sthux(PPU_STATE *hCore);
void PPCInterpreter_sthx(PPU_STATE *hCore);

// Store String Word
void PPCInterpreter_stswi(PPU_STATE *hCore);

// Store Word
void PPCInterpreter_stw(PPU_STATE *hCore);
void PPCInterpreter_stwbrx(PPU_STATE *hCore);
void PPCInterpreter_stwcx(PPU_STATE *hCore);
void PPCInterpreter_stwu(PPU_STATE *hCore);
void PPCInterpreter_stwux(PPU_STATE *hCore);
void PPCInterpreter_stwx(PPU_STATE *hCore);

// Store Doubleword
void PPCInterpreter_std(PPU_STATE *hCore);
void PPCInterpreter_stdcx(PPU_STATE *hCore);
void PPCInterpreter_stdu(PPU_STATE *hCore);
void PPCInterpreter_stdux(PPU_STATE *hCore);
void PPCInterpreter_stdx(PPU_STATE *hCore);

// Store Floating
void PPCInterpreter_stfd(PPU_STATE *hCore);

// Load Byte
void PPCInterpreter_lbz(PPU_STATE *hCore);
void PPCInterpreter_lbzu(PPU_STATE *hCore);
void PPCInterpreter_lbzux(PPU_STATE *hCore);
void PPCInterpreter_lbzx(PPU_STATE *hCore);

// Load Halfword
void PPCInterpreter_lha(PPU_STATE *hCore);
void PPCInterpreter_lhau(PPU_STATE *hCore);
void PPCInterpreter_lhax(PPU_STATE *hCore);
void PPCInterpreter_lhbrx(PPU_STATE *hCore);
void PPCInterpreter_lhz(PPU_STATE *hCore);
void PPCInterpreter_lhzu(PPU_STATE *hCore);
void PPCInterpreter_lhzux(PPU_STATE *hCore);
void PPCInterpreter_lhzx(PPU_STATE *hCore);

// Load String Word
void PPCInterpreter_lswi(PPU_STATE *hCore);

// Load Word
void PPCInterpreter_lwa(PPU_STATE *hCore);
void PPCInterpreter_lwax(PPU_STATE *hCore);
void PPCInterpreter_lwarx(PPU_STATE *hCore);
void PPCInterpreter_lwbrx(PPU_STATE *hCore);
void PPCInterpreter_lwz(PPU_STATE *hCore);
void PPCInterpreter_lwzu(PPU_STATE *hCore);
void PPCInterpreter_lwzux(PPU_STATE *hCore);
void PPCInterpreter_lwzx(PPU_STATE *hCore);

// Load Doubleword
void PPCInterpreter_ld(PPU_STATE *hCore);
void PPCInterpreter_ldarx(PPU_STATE *hCore);
void PPCInterpreter_ldbrx(PPU_STATE *hCore);
void PPCInterpreter_ldu(PPU_STATE *hCore);
void PPCInterpreter_ldux(PPU_STATE *hCore);
void PPCInterpreter_ldx(PPU_STATE *hCore);

//
// Load Floating
//

void PPCInterpreter_lfd(PPU_STATE *hCore);
void PPCInterpreter_lfs(PPU_STATE *hCore);
} // namespace PPCInterpreter
