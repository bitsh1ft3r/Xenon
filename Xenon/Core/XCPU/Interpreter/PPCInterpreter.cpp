// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"
#include "PPCInterpreter.h"

#include <unordered_map>

using namespace PPCInterpreter;

// Forward Declaration
XENON_CONTEXT* PPCInterpreter::intXCPUContext = nullptr;
RootBus* PPCInterpreter::sysBus = nullptr;

// Define a type alias for function pointers
using instructionHandler = void(*)(PPU_STATE*);

// Define a lookup table for instruction handlers.
std::unordered_map<PPCInstruction, instructionHandler> instructionHandlers = {
  { PPCInstruction::addex, PPCInterpreter_addex },
  { PPCInstruction::addi, PPCInterpreter_addi },
  { PPCInstruction::addic, PPCInterpreter_addic },
  { PPCInstruction::addicx, PPCInterpreter_addic_rc },
  { PPCInstruction::addis, PPCInterpreter_addis },
  { PPCInstruction::addx, PPCInterpreter_addx },
  { PPCInstruction::addzex, PPCInterpreter_addzex },
  { PPCInstruction::andcx, PPCInterpreter_andc },
  { PPCInstruction::andix, PPCInterpreter_andi },
  { PPCInstruction::andisx, PPCInterpreter_andis },
  { PPCInstruction::andx, PPCInterpreter_and },
  { PPCInstruction::bcctrx, PPCInterpreter_bcctr },
  { PPCInstruction::bclrx, PPCInterpreter_bclr },
  { PPCInstruction::bcx, PPCInterpreter_bc },
  { PPCInstruction::bx, PPCInterpreter_b },
  { PPCInstruction::cmp, PPCInterpreter_cmp },
  { PPCInstruction::cmpi, PPCInterpreter_cmpi },
  { PPCInstruction::cmpl, PPCInterpreter_cmpl },
  { PPCInstruction::cmpli, PPCInterpreter_cmpli },
  { PPCInstruction::cntlzdx, PPCInterpreter_cntlzd },
  { PPCInstruction::cntlzwx, PPCInterpreter_cntlzw },
  { PPCInstruction::crand, PPCInterpreter_crand },
  { PPCInstruction::crandc, PPCInterpreter_crandc },
  { PPCInstruction::creqv, PPCInterpreter_creqv },
  { PPCInstruction::crnand, PPCInterpreter_crnand },
  { PPCInstruction::crnor, PPCInterpreter_crnor },
  { PPCInstruction::cror, PPCInterpreter_cror },
  { PPCInstruction::crorc, PPCInterpreter_crorc },
  { PPCInstruction::crxor, PPCInterpreter_crxor },
  { PPCInstruction::dcbf, PPCInterpreter_dcbf },
  { PPCInstruction::dcbi, PPCInterpreter_dcbi },
  { PPCInstruction::dcbst, PPCInterpreter_dcbst },
  { PPCInstruction::dcbt, PPCInterpreter_dcbt },
  { PPCInstruction::dcbtst, PPCInterpreter_dcbtst },
  { PPCInstruction::dcbz, PPCInterpreter_dcbz },
  { PPCInstruction::divdux, PPCInterpreter_divdu },
  { PPCInstruction::divdx, PPCInterpreter_divd },
  { PPCInstruction::divwux, PPCInterpreter_divwux },
  { PPCInstruction::divwx, PPCInterpreter_divwx },
  { PPCInstruction::eieio, PPCInterpreter_eieio },
  { PPCInstruction::extsbx, PPCInterpreter_extsbx },
  { PPCInstruction::extshx, PPCInterpreter_extshx },
  { PPCInstruction::extswx, PPCInterpreter_extswx },
  { PPCInstruction::icbi, PPCInterpreter_icbi },
  { PPCInstruction::isync, PPCInterpreter_isync },
  { PPCInstruction::lbz, PPCInterpreter_lbz },
  { PPCInstruction::lbzu, PPCInterpreter_lbzu },
  { PPCInstruction::lbzux, PPCInterpreter_lbzux },
  { PPCInstruction::lbzx, PPCInterpreter_lbzx },
  { PPCInstruction::ld, PPCInterpreter_ld },
  { PPCInstruction::ldarx, PPCInterpreter_ldarx },
  { PPCInstruction::ldu, PPCInterpreter_ldu },
  { PPCInstruction::ldux, PPCInterpreter_ldux },
  { PPCInstruction::ldx, PPCInterpreter_ldx },
  { PPCInstruction::lfd, PPCInterpreter_lfd },
  { PPCInstruction::lfs, PPCInterpreter_lfs },
  { PPCInstruction::lha, PPCInterpreter_lha },
  { PPCInstruction::lhau, PPCInterpreter_lhau },
  { PPCInstruction::lhax, PPCInterpreter_lhax },
  { PPCInstruction::lhbrx, PPCInterpreter_lhbrx },
  { PPCInstruction::lhz, PPCInterpreter_lhz },
  { PPCInstruction::lhzu, PPCInterpreter_lhzu },
  { PPCInstruction::lhzux, PPCInterpreter_lhzux },
  { PPCInstruction::lhzx, PPCInterpreter_lhzx },
  { PPCInstruction::lmw, PPCInterpreter_lmw },
  { PPCInstruction::lswi, PPCInterpreter_lswi },
  { PPCInstruction::lwa, PPCInterpreter_lwa },
  { PPCInstruction::lwarx, PPCInterpreter_lwarx },
  { PPCInstruction::lwax, PPCInterpreter_lwax },
  { PPCInstruction::lwbrx, PPCInterpreter_lwbrx },
  { PPCInstruction::lwz, PPCInterpreter_lwz },
  { PPCInstruction::lwzu, PPCInterpreter_lwzu },
  { PPCInstruction::lwzux, PPCInterpreter_lwzux },
  { PPCInstruction::lwzx, PPCInterpreter_lwzx },
  { PPCInstruction::mcrf, PPCInterpreter_mcrf },
  { PPCInstruction::mfcr, PPCInterpreter_mfcr },
  { PPCInstruction::mffsx, PPCInterpreter_mffsx },
  { PPCInstruction::mfmsr, PPCInterpreter_mfmsr },
  { PPCInstruction::mfspr, PPCInterpreter_mfspr },
  { PPCInstruction::mftb, PPCInterpreter_mftb },
  { PPCInstruction::mtcrf, PPCInterpreter_mtcrf },
  { PPCInstruction::mtfsfx, PPCInterpreter_mtfsfx },
  { PPCInstruction::mtmsr, PPCInterpreter_mtmsr },
  { PPCInstruction::mtmsrd, PPCInterpreter_mtmsrd },
  { PPCInstruction::mtspr, PPCInterpreter_mtspr },
  { PPCInstruction::mulhdux, PPCInterpreter_mulhdux },
  { PPCInstruction::mulhwux, PPCInterpreter_mulhwux },
  { PPCInstruction::mulldx, PPCInterpreter_mulldx },
  { PPCInstruction::mulli, PPCInterpreter_mulli },
  { PPCInstruction::mullwx, PPCInterpreter_mullw },
  { PPCInstruction::nandx, PPCInterpreter_nandx },
  { PPCInstruction::negx, PPCInterpreter_negx },
  { PPCInstruction::norx, PPCInterpreter_norx },
  { PPCInstruction::orcx, PPCInterpreter_orcx },
  { PPCInstruction::ori, PPCInterpreter_ori },
  { PPCInstruction::oris, PPCInterpreter_oris },
  { PPCInstruction::orx, PPCInterpreter_orx },
  { PPCInstruction::rfid, PPCInterpreter_rfid },
  { PPCInstruction::rldcrx, PPCInterpreter_rldcrx },
  { PPCInstruction::rldiclx, PPCInterpreter_rldiclx },
  { PPCInstruction::rldicrx, PPCInterpreter_rldicrx },
  { PPCInstruction::rldicx, PPCInterpreter_rldicx },
  { PPCInstruction::rldimix, PPCInterpreter_rldimix },
  { PPCInstruction::rlwimix, PPCInterpreter_rlwimix },
  { PPCInstruction::rlwinmx, PPCInterpreter_rlwinmx },
  { PPCInstruction::rlwnmx, PPCInterpreter_rlwnmx },
  { PPCInstruction::sc, PPCInterpreter_sc },
  { PPCInstruction::slbia, PPCInterpreter_slbia },
  { PPCInstruction::slbie, PPCInterpreter_slbie },
  { PPCInstruction::slbmte, PPCInterpreter_slbmte },
  { PPCInstruction::sldx, PPCInterpreter_sldx },
  { PPCInstruction::slwx, PPCInterpreter_slwx },
  { PPCInstruction::sradix, PPCInterpreter_sradix },
  { PPCInstruction::sradx, PPCInterpreter_sradx },
  { PPCInstruction::srawix, PPCInterpreter_srawix },
  { PPCInstruction::srawx, PPCInterpreter_srawx },
  { PPCInstruction::srdx, PPCInterpreter_srdx },
  { PPCInstruction::srwx, PPCInterpreter_srwx },
  { PPCInstruction::stb, PPCInterpreter_stb },
  { PPCInstruction::stbu, PPCInterpreter_stbu },
  { PPCInstruction::stbux, PPCInterpreter_stbux },
  { PPCInstruction::stbx, PPCInterpreter_stbx },
  { PPCInstruction::std, PPCInterpreter_std },
  { PPCInstruction::stdcx, PPCInterpreter_stdcx },
  { PPCInstruction::stdu, PPCInterpreter_stdu },
  { PPCInstruction::stdux, PPCInterpreter_stdux },
  { PPCInstruction::stdx, PPCInterpreter_stdx },
  { PPCInstruction::stfd, PPCInterpreter_stfd },
  { PPCInstruction::sth, PPCInterpreter_sth },
  { PPCInstruction::sthbrx, PPCInterpreter_sthbrx },
  { PPCInstruction::sthu, PPCInterpreter_sthu },
  { PPCInstruction::sthux, PPCInterpreter_sthux },
  { PPCInstruction::sthx, PPCInterpreter_sthx },
  { PPCInstruction::stmw, PPCInterpreter_stmw },
  { PPCInstruction::stswi, PPCInterpreter_stswi },
  { PPCInstruction::stw, PPCInterpreter_stw },
  { PPCInstruction::stwbrx, PPCInterpreter_stwbrx },
  { PPCInstruction::stwcx, PPCInterpreter_stwcx },
  { PPCInstruction::stwu, PPCInterpreter_stwu },
  { PPCInstruction::stwux, PPCInterpreter_stwux },
  { PPCInstruction::stwx, PPCInterpreter_stwx },
  { PPCInstruction::subfcx, PPCInterpreter_subfcx },
  { PPCInstruction::subfex, PPCInterpreter_subfex },
  { PPCInstruction::subfic, PPCInterpreter_subfic },
  { PPCInstruction::subfx, PPCInterpreter_subfx },
  { PPCInstruction::sync, PPCInterpreter_sync },
  { PPCInstruction::tdi, PPCInterpreter_tdi },
  { PPCInstruction::tlbie, PPCInterpreter_tlbie },
  { PPCInstruction::tlbiel, PPCInterpreter_tlbiel },
  { PPCInstruction::tlbsync, PPCInterpreter_tlbsync },
  { PPCInstruction::tw, PPCInterpreter_tw },
  { PPCInstruction::twi, PPCInterpreter_twi },
  { PPCInstruction::xori, PPCInterpreter_xori },
  { PPCInstruction::xoris, PPCInterpreter_xoris },
  { PPCInstruction::xorx, PPCInterpreter_xorx}
};

// Interpreter Single Instruction Processing.
void PPCInterpreter::ppcExecuteSingleInstruction(PPU_STATE* hCore) {
  PPCInstruction currentInstr =
    getOpcode(hCore->ppuThread[hCore->currentThread].CI);

  // RGH 2 for CB_A 9188 in a JRunner XDKBuild.
  if (hCore->ppuThread[hCore->currentThread].CIA == 0x000000000200c870) {
    //hCore->ppuThread[hCore->currentThread].GPR[0x5] = 0;
  }

  // RGH 2 for CB_A 9188 in a JRunner Normal Build.
  if (hCore->ppuThread[hCore->currentThread].CIA == 0x000000000200c820) {
    hCore->ppuThread[hCore->currentThread].GPR[0x3] = 0;
  }

  // RGH 2 17489 in a JRunner Corona XDKBuild.
  if (hCore->ppuThread[hCore->currentThread].CIA == 0x200c7f0) {
    hCore->ppuThread[hCore->currentThread].GPR[0x3] = 0;
  }

  // 3BL Check Bypass Devkit 2.0.1838.1
  if (hCore->ppuThread[hCore->currentThread].CIA == 0x0000000003004994) {
    // hCore->ppuThread[hCore->currentThread].GPR[0x3] = 1;
  }

  // 4BL Check Bypass Devkit 2.0.1838.1
  if (hCore->ppuThread[hCore->currentThread].CIA == 0x0000000003004bf0) {
    // hCore->ppuThread[hCore->currentThread].GPR[0x3] = 1;
  }

  // 3BL Signature Check Bypass Devkit 2.0.2853.0
  if (hCore->ppuThread[hCore->currentThread].CIA == 0x0000000003006488) {
    // hCore->ppuThread[hCore->currentThread].GPR[0x3] = 0;
  }

  // XDK 17.489.0 AudioChipCorder Device Detect bypass. This is not needed for
  // older console revisions.
  if ((u32)hCore->ppuThread[hCore->currentThread].CIA == 0x801AF580) {
    return;
  }

  // This is just to set a PC breakpoint in any PPU/Thread.
  if ((u32)hCore->ppuThread[hCore->currentThread].CIA == 0x8009ce40) {
    u8 a = 0;
  }

  // This is to set a PPU0[Thread0] breakpoint.
  if (hCore->ppuThread[hCore->currentThread].SPR.PIR == 0) {
    hCore->ppuThread[hCore->currentThread].lastRegValue =
        hCore->ppuThread[hCore->currentThread].GPR[11];
  }

  // Execute the instruction using the lookup table.
  auto it = instructionHandlers.find(currentInstr);
  if (it != instructionHandlers.end()) {
    it->second(hCore);
  }
  else {
    LOG_CRITICAL(Xenon, "PPC Interpreter: Unknown or unimplemented instruction found: data: {:#x }, address: {:#x }, OpCode: {}.",
        hCore->ppuThread[hCore->currentThread].CI,
        hCore->ppuThread[hCore->currentThread].CIA,
        getOpcodeName(hCore->ppuThread[hCore->currentThread].CI));
  }
}

//
// Exception definitions.
//

/* Exception name(Reset Vector) */

// System reset Exception (0x100)
void PPCInterpreter::ppcResetException(PPU_STATE* hCore) {
  LOG_INFO(Xenon, "[{}](Thrd{:#d}): Reset exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
    hCore->ppuThread[hCore->currentThread].NIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0x100;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}
// Data Storage Exception (0x300)
void PPCInterpreter::ppcDataStorageException(PPU_STATE* hCore) {
  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Data Storage exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
    hCore->ppuThread[hCore->currentThread].CIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0x300;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}
// Data Segment Exception (0x380)
void PPCInterpreter::ppcDataSegmentException(PPU_STATE* hCore) {
  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Data Segment exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
    hCore->ppuThread[hCore->currentThread].CIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0x380;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}
// Instruction Storage Exception (0x400)
void PPCInterpreter::ppcInstStorageException(PPU_STATE* hCore) {
  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Instruction Storage exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
    hCore->ppuThread[hCore->currentThread].CIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 |= QMASK(33, 33);
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0x400;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}
// Instruction Segment Exception (0x480)
void PPCInterpreter::ppcInstSegmentException(PPU_STATE* hCore) {
  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Instruction Segment exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
    hCore->ppuThread[hCore->currentThread].CIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
     hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0x480;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}
// External Exception (0x500)
void PPCInterpreter::ppcExternalException(PPU_STATE* hCore) {
  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): External exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
    hCore->ppuThread[hCore->currentThread].NIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0x500;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}
// Program Exception (0x700)
void PPCInterpreter::ppcProgramException(PPU_STATE* hCore) {
  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Program exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
      hCore->ppuThread[hCore->currentThread].CIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  BSET(hCore->ppuThread[hCore->currentThread].SPR.SRR1, 64,
    hCore->ppuThread[hCore->currentThread].exceptTrapType);
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex |
    (QMASK(0, 0) | QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0x700;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}

void PPCInterpreter::ppcDecrementerException(PPU_STATE* hCore) {
  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): Decrementer exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
    hCore->ppuThread[hCore->currentThread].NIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex | QMASK(0, 0) |
    (QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0x900;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}

// System Call Exception (0xC00)
void PPCInterpreter::ppcSystemCallException(PPU_STATE* hCore) {
  LOG_TRACE(Xenon, "[{}](Thrd{:#d}): System Call exception.", hCore->ppuName, (s8)hCore->currentThread);
  hCore->ppuThread[hCore->currentThread].SPR.SRR0 =
    hCore->ppuThread[hCore->currentThread].NIA;
  hCore->ppuThread[hCore->currentThread].SPR.SRR1 =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex &
    ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
  hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex =
    hCore->ppuThread[hCore->currentThread].SPR.MSR.MSR_Hex | QMASK(0, 0) |
    (hCore->ppuThread[hCore->currentThread].exceptHVSysCall ? 0
      : QMASK(3, 3));
  hCore->ppuThread[hCore->currentThread].NIA = hCore->SPR.HRMOR + 0xc00;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.DR = 0;
  hCore->ppuThread[hCore->currentThread].SPR.MSR.IR = 0;
}

void PPCInterpreter::ppcInterpreterTrap(PPU_STATE* hCore, u32 trapNumber) {
  // DbgPrint, r3 = PCSTR stringAddress, r4 = int String Size.
  if (trapNumber == 0x14) {
    std::string dbgString;
    dbgString.resize(hCore->ppuThread[hCore->currentThread].GPR[0x4]);
    size_t strSize = (size_t)hCore->ppuThread[hCore->currentThread].GPR[0x4];
    for (int idx = 0; idx < strSize; idx++) {
      dbgString[idx] = MMURead8(
        hCore, hCore->ppuThread[hCore->currentThread].GPR[0x3] + idx);
    }
    LOG_XBOX(DebugPrint, "> {}", dbgString);
  }

  if (trapNumber == 0x17) {
      // DebugLoadImageSymbols, type signature:
      // PUBLIC VOID DebugLoadImageSymbols(IN PSTRING ModuleName == $r3,
      //                                   IN PKD_SYMBOLS_INFO Info == $r4)
      ppcDebugLoadImageSymbols(hCore,
        hCore->ppuThread[hCore->currentThread].GPR[0x3],
        hCore->ppuThread[hCore->currentThread].GPR[4]);
  }
  if (trapNumber == 24) {
      // DebugUnloadImageSymbols, type signature:
      // PUBLIC VOID DebugUnloadImageSymbols(IN PSTRING ModuleName == $r3,
      //                                   IN PKD_SYMBOLS_INFO Info == $r4)
      ppcDebugUnloadImageSymbols(hCore,
        hCore->ppuThread[hCore->currentThread].GPR[0x3],
        hCore->ppuThread[hCore->currentThread].GPR[4]);
  }
  hCore->ppuThread[hCore->currentThread].exceptReg |= PPU_EX_PROG;
  hCore->ppuThread[hCore->currentThread].exceptTrapType =
    TRAP_TYPE_SRR1_TRAP_TRAP;
}
