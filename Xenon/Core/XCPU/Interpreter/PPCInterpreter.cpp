// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"
#include "PPCInterpreter.h"

#include <unordered_map>

using namespace PPCInterpreter;

// Forward Declaration
XENON_CONTEXT* PPCInterpreter::intXCPUContext = nullptr;
RootBus* PPCInterpreter::sysBus = nullptr;

const std::unordered_map<u32, PPCInstruction> PPCInterpreter::opcodeMap{
  { 2, { "tdi", PPCInterpreter_tdi } },
  { 3, { "twi", PPCInterpreter_twi } },
  { 7, { "mulli", PPCInterpreter_mulli } },
  { 8, { "subfic", PPCInterpreter_subfic } },
  { 10, { "cmpli", PPCInterpreter_cmpli } },
  { 11, { "cmpi", PPCInterpreter_cmpi } },
  { 12, { "addic", PPCInterpreter_addic } },
  { 13, { "addic_rc", PPCInterpreter_addic_rc } },
  { 14, { "addi", PPCInterpreter_addi } },
  { 15, { "addis", PPCInterpreter_addis } },
  { 16, { "bc", PPCInterpreter_bc } },
  { 17, { "sc", PPCInterpreter_sc } },
  { 18, { "b", PPCInterpreter_b } },
  { 20, { "rlwimix", PPCInterpreter_rlwimix } },
  { 21, { "rlwinmx", PPCInterpreter_rlwinmx } },
  { 23, { "rlwnmx", PPCInterpreter_rlwnmx } },
  { 24, { "ori", PPCInterpreter_ori } },
  { 25, { "oris", PPCInterpreter_oris } },
  { 26, { "xori", PPCInterpreter_xori } },
  { 27, { "xoris", PPCInterpreter_xoris } },
  { 28, { "andi", PPCInterpreter_andi } },
  { 29, { "andis", PPCInterpreter_andis } },
  { 32, { "lwz", PPCInterpreter_lwz } },
  { 33, { "lwzu", PPCInterpreter_lwzu } },
  { 34, { "lbz", PPCInterpreter_lbz } },
  { 35, { "lbzu", PPCInterpreter_lbzu } },
  { 36, { "stw", PPCInterpreter_stw } },
  { 37, { "stwu", PPCInterpreter_stwu } },
  { 38, { "stb", PPCInterpreter_stb } },
  { 39, { "stbu", PPCInterpreter_stbu } },
  { 40, { "lhz", PPCInterpreter_lhz } },
  { 41, { "lhzu", PPCInterpreter_lhzu } },
  { 42, { "lha", PPCInterpreter_lha } },
  { 43, { "lhau", PPCInterpreter_lhau } },
  { 44, { "sth", PPCInterpreter_sth } },
  { 45, { "sthu", PPCInterpreter_sthu } },
  { 46, { "lmw", PPCInterpreter_lmw } },
  { 47, { "stmw", PPCInterpreter_stmw } },
  { 48, { "lfs", PPCInterpreter_lfs } },
  { 50, { "lfd", PPCInterpreter_lfd } },
  { 52, { "stfs", PPCInterpreter_invalid } }, // stfs
  { 53, { "stfsu", PPCInterpreter_invalid } }, // stfsu
  { 54, { "stfd", PPCInterpreter_stfd } },
};
const std::unordered_map<u32, PPCInstruction> PPCInterpreter::subgroup19Map{
  { 0, { "mcrf", PPCInterpreter_mcrf } },
  { 16, { "bclr", PPCInterpreter_bclr } },
  { 18, { "rfid", PPCInterpreter_rfid } },
  { 33, { "crnor", PPCInterpreter_crnor } },
  { 129, { "crandc", PPCInterpreter_crandc } },
  { 150, { "isync", PPCInterpreter_isync } },
  { 193, { "crxor", PPCInterpreter_crxor } },
  { 225, { "crnand", PPCInterpreter_crnand } },
  { 257, { "crand", PPCInterpreter_crand } },
  { 289, { "creqv", PPCInterpreter_creqv } },
  { 417, { "crorc", PPCInterpreter_crorc } },
  { 449, { "cror", PPCInterpreter_cror } },
  { 528, { "bcctr", PPCInterpreter_bcctr } }
};
const std::unordered_map<u32, PPCInstruction> PPCInterpreter::subgroup30Map{
  { 0, { "rldiclx", PPCInterpreter_rldiclx } },
  { 1, { "rldicrx", PPCInterpreter_rldicrx } },
  { 2, { "rldicx", PPCInterpreter_rldicx } },
  { 3, { "rldimix", PPCInterpreter_rldimix } },
  { 8, { "rldclx", PPCInterpreter_invalid } }, // rldclx
  { 9, { "rldcrx", PPCInterpreter_rldcrx } }
};
const std::unordered_map<u32, PPCInstruction> PPCInterpreter::subgroup31Map{
  { 0, { "cmp", PPCInterpreter_cmp } },
  { 4, { "tw", PPCInterpreter_tw } },
  { 19, { "mfcr", PPCInterpreter_mfcr } },
  { 20, { "lwarx", PPCInterpreter_lwarx } },
  { 21, { "ldx", PPCInterpreter_ldx } },
  { 23, { "lwzx", PPCInterpreter_lwzx } },
  { 24, { "slwx", PPCInterpreter_slwx } },
  { 26, { "cntlzw", PPCInterpreter_cntlzw } },
  { 27, { "sldx", PPCInterpreter_sldx } },
  { 28, { "and", PPCInterpreter_and } },
  { 32, { "cmpl", PPCInterpreter_cmpl } },
  { 53, { "ldux", PPCInterpreter_ldux } },
  { 54, { "dcbst", PPCInterpreter_dcbst } },
  { 55, { "lwzux", PPCInterpreter_lwzux } },
  { 58, { "cntlzd", PPCInterpreter_cntlzd } },
  { 60, { "andc", PPCInterpreter_andc } },
  { 68, { "td", PPCInterpreter_invalid } }, // td
  { 83, { "mfmsr", PPCInterpreter_mfmsr } },
  { 84, { "ldarx", PPCInterpreter_ldarx } },
  { 86, { "dcbf", PPCInterpreter_dcbf } },
  { 87, { "lbzx", PPCInterpreter_lbzx } },
  { 119, { "lbzux", PPCInterpreter_lbzux } },
  { 124, { "norx", PPCInterpreter_norx } },
  { 144, { "mtcrf", PPCInterpreter_mtcrf } },
  { 146, { "mtmsr", PPCInterpreter_mtmsr } },
  { 149, { "stdx", PPCInterpreter_stdx } },
  { 150, { "stwcx", PPCInterpreter_stwcx } },
  { 151, { "stwx", PPCInterpreter_stwx } },
  { 178, { "mtmsrd", PPCInterpreter_mtmsrd } },
  { 181, { "stdux", PPCInterpreter_stdux } },
  { 183, { "stwux", PPCInterpreter_stwux } },
  { 210, { "mtsr", PPCInterpreter_invalid } }, // mtsr
  { 214, { "stdcx", PPCInterpreter_stdcx } },
  { 215, { "stbx", PPCInterpreter_stbx } },
  { 242, { "mtsrin", PPCInterpreter_invalid } }, // mtsrin
  { 247, { "stbux", PPCInterpreter_stbux } },
  { 246, { "dcbt", PPCInterpreter_dcbt } },
  { 278, { "dcbt", PPCInterpreter_dcbt } },
  { 279, { "lhzx", PPCInterpreter_lhzx } },
  { 284, { "eqvx", PPCInterpreter_invalid } }, // eqvx
  { 274, { "tlbiel", PPCInterpreter_tlbiel } },
  { 306, { "tlbie", PPCInterpreter_tlbie } },
  { 310, { "eciwx", PPCInterpreter_invalid } }, // eciwx
  { 311, { "lhzux", PPCInterpreter_lhzux } },
  { 316, { "xorx", PPCInterpreter_xorx } },
  { 339, { "mfspr", PPCInterpreter_mfspr } },
  { 341, { "lwax", PPCInterpreter_lwax } },
  { 343, { "lhax", PPCInterpreter_lhax } },
  { 370, { "tlbia", PPCInterpreter_invalid } }, // tlbia
  { 371, { "mftb", PPCInterpreter_mftb } },
  { 373, { "lwaux", PPCInterpreter_invalid } }, // lwaux
  { 375, { "lhaux", PPCInterpreter_invalid } }, // lhaux
  { 407, { "sthx", PPCInterpreter_sthx } },
  { 412, { "orcx", PPCInterpreter_orcx } },
  { 434, { "slbie", PPCInterpreter_slbie } },
  { 438, { "ecowx", PPCInterpreter_invalid } }, // ecowx
  { 439, { "sthux", PPCInterpreter_sthux } },
  { 444, { "orx", PPCInterpreter_orx } },
  { 467, { "mtspr", PPCInterpreter_mtspr } },
  { 476, { "nandx", PPCInterpreter_nandx } },
  { 498, { "slbia", PPCInterpreter_slbia } },
  { 533, { "lswx", PPCInterpreter_invalid } }, // lswx
  { 534, { "lwbrx", PPCInterpreter_lwbrx } },
  { 535, { "lfsx", PPCInterpreter_invalid } }, // lfsx
  { 536, { "srwx", PPCInterpreter_srwx } },
  { 539, { "srdx", PPCInterpreter_srdx } },
  { 566, { "tlbsync", PPCInterpreter_tlbsync } },
  { 567, { "lfsux", PPCInterpreter_invalid } }, // lfsux
  { 595, { "mfsr", PPCInterpreter_invalid } }, // mfsr
  { 597, { "lswi", PPCInterpreter_lswi } },
  { 598, { "sync", PPCInterpreter_sync } },
  { 599, { "lfdx", PPCInterpreter_invalid } }, // lfdx
  { 631, { "lfdux", PPCInterpreter_invalid } }, // lfdux
  { 569, { "mfsrin", PPCInterpreter_invalid } }, // mfsrin
  { 915, { "slbmfee", PPCInterpreter_invalid } }, // slbmfee
  { 851, { "slbmfev", PPCInterpreter_invalid } }, // slbmfev
  { 402, { "slbmte", PPCInterpreter_slbmte } },
  { 661, { "stswx", PPCInterpreter_invalid } }, // stswx
  { 662, { "stwbrx", PPCInterpreter_stwbrx } },
  { 663, { "stfsx", PPCInterpreter_invalid } }, // stfsx
  { 695, { "stfsux", PPCInterpreter_invalid } }, // stfsux
  { 725, { "stswi", PPCInterpreter_stswi } },
  { 727, { "stfdx", PPCInterpreter_invalid } }, // stfdx
  { 759, { "stfdux", PPCInterpreter_invalid } }, // stfdux
  { 790, { "lhbrx", PPCInterpreter_lhbrx } },
  { 792, { "srawx", PPCInterpreter_srawx } },
  { 794, { "sradx", PPCInterpreter_sradx } },
  { 824, { "srawix", PPCInterpreter_srawix } },
  { 854, { "eieio", PPCInterpreter_eieio } },
  { 918, { "sthbrx", PPCInterpreter_sthbrx } },
  { 922, { "extshx", PPCInterpreter_extshx } },
  { 954, { "extsbx", PPCInterpreter_extsbx } },
  { 982, { "icbi", PPCInterpreter_icbi } },
  { 983, { "stfiwx", PPCInterpreter_invalid } }, // stfiwx
  { 986, { "extswx", PPCInterpreter_extswx } },
  { 1014, { "dcbz", PPCInterpreter_dcbz } },
  { 8, { "subfcx", PPCInterpreter_subfcx } },
  { 9, { "addcx", PPCInterpreter_mulhdux } },
  { 10, { "addcx", PPCInterpreter_invalid } }, // addcx
  { 11, { "mulhwux", PPCInterpreter_mulhwux } },
  { 40, { "subfx", PPCInterpreter_subfx } },
  { 73, { "mulhdx", PPCInterpreter_invalid } }, // mulhdx
  { 75, { "mulhwx", PPCInterpreter_invalid } }, // mulhwx
  { 104, { "negx", PPCInterpreter_negx } },
  { 136, { "subfex", PPCInterpreter_subfex } },
  { 138, { "addex", PPCInterpreter_addex } },
  { 200, { "subfzex", PPCInterpreter_invalid } }, // subfzex
  { 202, { "addzex", PPCInterpreter_addzex } },
  { 232, { "subfmex", PPCInterpreter_invalid } }, // subfmex
  { 233, { "mulldx", PPCInterpreter_mulldx } },
  { 234, { "addmex", PPCInterpreter_invalid } }, // addmex
  { 235, { "mullw", PPCInterpreter_mullw } },
  { 266, { "addx", PPCInterpreter_addx } },
  { 457, { "divdu", PPCInterpreter_divdu } },
  { 459, { "divwux", PPCInterpreter_divwux } },
  { 489, { "divd", PPCInterpreter_divd } },
  { 491, { "divwx", PPCInterpreter_divwx } },
  { 413, { "sradix", PPCInterpreter_sradix } }
};
const std::unordered_map<u32, PPCInstruction> PPCInterpreter::subgroup58Map{
  { 0, { "ld", PPCInterpreter_ld } },
  { 1, { "ldu", PPCInterpreter_ldu } },
  { 2, { "lwa", PPCInterpreter_lwa } }
};
const std::unordered_map<u32, PPCInstruction> PPCInterpreter::subgroup62Map{
  { 0, { "std", PPCInterpreter_std } },
  { 1, { "stdu", PPCInterpreter_stdu } }
};
const std::unordered_map<u32, PPCInstruction> PPCInterpreter::subgroup63Map{
  { 583, { "mffsx", PPCInterpreter_mffsx } },
  { 711, { "mtfsfx", PPCInterpreter_mtfsfx } }
};

void PPCInterpreter_invalid(PPU_STATE* hCore) {
  LOG_CRITICAL(Xenon, "PPC Interpreter: Unknown or unimplemented instruction found: data: {:#x}, address: {:#x}, OpCode: {}.",
    hCore->ppuThread[hCore->currentThread].CI,
    hCore->ppuThread[hCore->currentThread].CIA,
    getInstruction({ hCore->ppuThread[hCore->currentThread].CI }).name);
}

// Interpreter Single Instruction Processing.
void PPCInterpreter::ppcExecuteSingleInstruction(PPU_STATE* hCore) {
  PPCInstruction currentInstr =
    getInstruction({ hCore->ppuThread[hCore->currentThread].CI });

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

  PPU_THREAD_REGISTERS& thread = hCore->ppuThread[hCore->currentThread];
  bool regionOfImportance = thread.CIA >= 0x800000001C000AC4 && thread.CIA <= 0x800000001C000F2C;
  static bool hardDump = false;
  if (thread.CIA == 0x800000001C000E00 || thread.NIA == 0x800000001C000E00)
    hardDump = true;
  u64 cachedGPRs[32]{};
  if (regionOfImportance || hardDump)
    memcpy(cachedGPRs, thread.GPR, sizeof(cachedGPRs));
  CRegister cachedCR{};
  if (regionOfImportance || hardDump)
    memcpy(&cachedCR, &thread.CR, sizeof(cachedCR));
  XERRegister cachedXER{};
  if (regionOfImportance || hardDump)
    memcpy(&cachedXER, &thread.SPR.XER, sizeof(cachedXER));
  if (regionOfImportance || hardDump) {
    LOG_INFO(Xenon, "Ins: {} | CIA: {:#x} | NIA: {:#x} | GPR: 0|{:#x} 1|{:#x} 2|{:#x} 3|{:#x} 4|{:#x} 5|{:#x} 6|{:#x} 7|{:#x} 8|{:#x} 9|{:#x} 29|{:#x} 30|{:#x} 31|{:#x}",
      currentInstr.name,
      thread.CIA,
      thread.NIA,
      thread.GPR[0],
      thread.GPR[1],
      thread.GPR[2],
      thread.GPR[3],
      thread.GPR[4],
      thread.GPR[5],
      thread.GPR[6],
      thread.GPR[7],
      thread.GPR[8],
      thread.GPR[9],
      thread.GPR[29],
      thread.GPR[30],
      thread.GPR[31]
    );
  }

  currentInstr.handler(hCore);

  if (regionOfImportance || hardDump) {
    for (u32 i{}; i != 32; ++i) {
      if (thread.GPR[i] != cachedGPRs[i]) {
        LOG_INFO(Xenon, "GPR {} was modified ({:#x} to {:#x})", i, cachedGPRs[i], thread.GPR[i]);
      }
    }
    if (thread.CR.CR_Hex != cachedCR.CR_Hex) {
      LOG_INFO(Xenon, "CR was modified ({:#x} to {:#x})", cachedCR.CR_Hex, thread.CR.CR_Hex);
    }
    if (thread.SPR.XER.XER_Hex != cachedXER.XER_Hex) {
      LOG_INFO(Xenon, "XER was modified ({:#x} to {:#x})", cachedCR.CR_Hex, thread.SPR.XER.XER_Hex);
    }
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
