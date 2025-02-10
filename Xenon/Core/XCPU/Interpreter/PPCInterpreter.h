// Copyright 2025 Xenon Emulator Project

#pragma once

#include "PPCInternal.h"
#include "PPC_Instruction.h"
#include "PPCOpcodes.h"

#include "Core/RootBus/RootBus.h"
#include "Core/XCPU/PPU/PowerPC.h"

namespace PPCInterpreter {
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
} // namespace PPCInterpreter
