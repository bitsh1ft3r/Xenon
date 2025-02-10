// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"
#include "PPCInterpreter.h"

#include <unordered_map>

using namespace PPCInterpreter;

// Forward Declaration
XENON_CONTEXT* PPCInterpreter::intXCPUContext = nullptr;
RootBus* PPCInterpreter::sysBus = nullptr;

PPCInterpreter::PPCDecoder ppcDecoder{};


// Interpreter Single Instruction Processing.
void PPCInterpreter::ppcExecuteSingleInstruction(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  // RGH 2 for CB_A 9188 in a JRunner XDKBuild.
  if (thread.CIA == 0x000000000200c870) {
    //thread.GPR[0x5] = 0;
  }

  // RGH 2 for CB_A 9188 in a JRunner Normal Build.
  if (thread.CIA == 0x000000000200c820) {
    thread.GPR[0x3] = 0;
  }

  // RGH 2 17489 in a JRunner Corona XDKBuild.
  if (thread.CIA == 0x200c7f0) {
    thread.GPR[0x3] = 0;
  }

  // 3BL Check Bypass Devkit 2.0.1838.1
  if (thread.CIA == 0x0000000003004994) {
    // thread.GPR[0x3] = 1;
  }

  // 4BL Check Bypass Devkit 2.0.1838.1
  if (thread.CIA == 0x0000000003004bf0) {
    // thread.GPR[0x3] = 1;
  }

  // 3BL Signature Check Bypass Devkit 2.0.2853.0
  if (thread.CIA == 0x0000000003006488) {
    // thread.GPR[0x3] = 0;
  }

  // XDK 17.489.0 AudioChipCorder Device Detect bypass. This is not needed for
  // older console revisions.
  if ((u32)thread.CIA == 0x801AF580) {
    return;
  }

  // This is just to set a PC breakpoint in any PPU/Thread.
  if ((u32)thread.CIA == 0x8009ce40) {
    u8 a = 0;
  }

  // This is to set a PPU0[Thread0] breakpoint.
  if (thread.SPR.PIR == 0) {
    thread.lastRegValue =
      thread.GPR[11];
  }

  instructionHandler function =
    ppcDecoder.decode(thread.CI.opcode);

  function(hCore);
}

//
// Exception definitions.
//

/* Exception name(Reset Vector) */

// System reset Exception (0x100)
void PPCInterpreter::ppcResetException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_INFO(Xenon, "[{}](Thrd{:#d}): Reset exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.NIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0x100;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}
// Data Storage Exception (0x300)
void PPCInterpreter::ppcDataStorageException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Data Storage exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.CIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0x300;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}
// Data Segment Exception (0x380)
void PPCInterpreter::ppcDataSegmentException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Data Segment exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.CIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0x380;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}
// Instruction Storage Exception (0x400)
void PPCInterpreter::ppcInstStorageException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Instruction Storage exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.CIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  thread.SPR.SRR1 |= QMASK(33, 33);
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0x400;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}
// Instruction Segment Exception (0x480)
void PPCInterpreter::ppcInstSegmentException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Instruction Segment exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.CIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0x480;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}
// External Exception (0x500)
void PPCInterpreter::ppcExternalException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): External exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.NIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0x500;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}
// Program Exception (0x700)
void PPCInterpreter::ppcProgramException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Program exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.CIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  BSET(thread.SPR.SRR1, 64,
    thread.exceptTrapType);
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0x700;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}

void PPCInterpreter::ppcDecrementerException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Decrementer exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.NIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex | QMASK(0, 0) |
    (QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0x900;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}

// System Call Exception (0xC00)
void PPCInterpreter::ppcSystemCallException(PPU_STATE* hCore) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): System Call exception.", hCore->ppuName, (s8)hCore->currentThread);
  thread.SPR.SRR0 =
    thread.NIA;
  thread.SPR.SRR1 =
    thread.SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  thread.SPR.MSR.MSR_Hex =
    thread.SPR.MSR.MSR_Hex | QMASK(0, 0) |
    (thread.exceptHVSysCall ? 0
      : QMASK(3, 3));
  thread.NIA = hCore->SPR.HRMOR + 0xc00;
  thread.SPR.MSR.DR = 0;
  thread.SPR.MSR.IR = 0;
}

void PPCInterpreter::ppcInterpreterTrap(PPU_STATE* hCore, u32 trapNumber) {
  PPU_THREAD_REGISTERS& thread =
    hCore->ppuThread[hCore->currentThread];

  if (trapNumber ==
    0x14) // DbgPrint, r3 = PCSTR stringAddress, r4 = int String Size.
  {
    std::string dbgString;
    dbgString.resize(thread.GPR[0x4]);
    size_t strSize = (size_t)thread.GPR[0x4];
    for (int idx = 0; idx < strSize; idx++) {
      dbgString[idx] = MMURead8(
        hCore, thread.GPR[0x3] + idx);
    }
    LOG_XBOX(DebugPrint, "> {}", dbgString);
  }

  if (trapNumber == 0x17) {
    // DebugLoadImageSymbols, type signature:
    // PUBLIC VOID DebugLoadImageSymbols(IN PSTRING ModuleName == $r3,
    //                   IN PKD_SYMBOLS_INFO Info == $r4)

    ppcDebugLoadImageSymbols(hCore,
      thread.GPR[0x3],
      thread.GPR[4]);
  }
  if (trapNumber == 24) {
    // DebugUnloadImageSymbols, type signature:
    // PUBLIC VOID DebugUnloadImageSymbols(IN PSTRING ModuleName == $r3,
    //                   IN PKD_SYMBOLS_INFO Info == $r4)

    ppcDebugUnloadImageSymbols(hCore,
      thread.GPR[0x3],
      thread.GPR[4]);
  }

  thread.exceptReg |= PPU_EX_PROG;
  thread.exceptTrapType =
    TRAP_TYPE_SRR1_TRAP_TRAP;
}
