// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"
#include "PPCInterpreter.h"

// Forward Declaration
XENON_CONTEXT *PPCInterpreter::intXCPUContext = nullptr;
RootBus *PPCInterpreter::sysBus = nullptr;

// interpreter Single Instruction Processing.
void PPCInterpreter::ppcExecuteSingleInstruction(PPU_STATE *hCore) {

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


#ifdef CORE_DUMP
  if (PPCInterpreter::startCoredump) {
    auto thread = hCore->ppuThread[hCore->currentThread];
    static std::ofstream f;
    if (!f.is_open()) {
       f.open("log/coredump.txt");
    }

    f << fmt::format(
#ifdef DUMP_SPRS
#ifdef DUMP_FPU
      "{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
#else
      "{:#x}"
#endif // DUMP_FPU
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
#ifdef DUMP_MSR
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
      "|{:#x}"
#endif // DUMP_MSR
      "|{:#x}"
      "|{:#x}"
#else
      "{:#x}"
#endif
      "|{:#x}"
      "|{:#x}",
#ifdef DUMP_SPRS
#ifdef DUMP_FPU
      (u64)thread.SPR.XER.XER_Hex,
      (u64)thread.SPR.XER.ByteCount,
      (u64)thread.SPR.XER.R0,
      (u64)thread.SPR.XER.CA,
      (u64)thread.SPR.XER.OV,
      (u64)thread.SPR.XER.SO,
#endif // DUMP_FPU
      (u64)thread.SPR.LR,
      (u64)thread.SPR.CTR,
      (u64)thread.SPR.CFAR,
      (u64)thread.SPR.VRSAVE,
      (u64)thread.SPR.DSISR,
      (u64)thread.SPR.DAR,
      (u64)thread.SPR.SRR0,
      (u64)thread.SPR.SRR1,
      (u64)thread.SPR.ACCR,
      (u64)thread.SPR.SPRG0,
      (u64)thread.SPR.SPRG1,
      (u64)thread.SPR.SPRG2,
      (u64)thread.SPR.SPRG3,
      (u64)thread.SPR.HSPRG0,
      (u64)thread.SPR.HSPRG1,
      (u64)thread.SPR.HSRR0,
      (u64)thread.SPR.HSRR1,
      (u64)thread.SPR.TSRL,
      (u64)thread.SPR.TSSR,
      (u64)thread.SPR.PPE_TLB_Index_Hint,
      (u64)thread.SPR.DABR,
      (u64)thread.SPR.DABRX,
#ifdef DUMP_MSR
      (u64)thread.SPR.MSR.MSR_Hex,
      (u64)thread.SPR.MSR.LE,
      (u64)thread.SPR.MSR.RI,
      (u64)thread.SPR.MSR.PMM,
      (u64)thread.SPR.MSR.DR,
      (u64)thread.SPR.MSR.IR,
      (u64)thread.SPR.MSR.FE1,
      (u64)thread.SPR.MSR.BE,
      (u64)thread.SPR.MSR.SE,
      (u64)thread.SPR.MSR.FE0,
      (u64)thread.SPR.MSR.ME,
      (u64)thread.SPR.MSR.FP,
      (u64)thread.SPR.MSR.PR,
      (u64)thread.SPR.MSR.EE,
      (u64)thread.SPR.MSR.ILE,
      (u64)thread.SPR.MSR.VXU,
      (u64)thread.SPR.MSR.HV,
      (u64)thread.SPR.MSR.TA,
      (u64)thread.SPR.MSR.SF,
#endif // DUMP_MSR
      (u64)thread.SPR.PIR,
#endif // DUMP_SPRS
      (u64)thread.CIA,
      (u64)thread.NIA,
      (u64)thread.CI);

    f << "|" << (int)thread.iFetch
#ifdef DUMP_GPRS
    << "|GPR:";
    for (int i{}; i != 32; ++i) {
      f << "|0x" << std::hex << thread.GPR[i];
    }
#if !defined(DUMP_EXCPR) && !defined(DUMP_SLBS) && !defined(DUMP_FPU) && !defined(DUMP_CRS)
    f << std::endl;
#endif // ifndef DUMP_EXCPR && ifndef DUMP_SLBS && ifndef DUMP_FPU && ifndef DUMP_CRS
#endif // DUMP_GPRS

#if !defined(DUMP_EXCPR) && !defined(DUMP_SLBS) && !defined(DUMP_FPU) && !defined(DUMP_CRS) && !defined(DUMP_GPRS)
    << std::endl;
#else
    ;
#endif // ifndef DUMP_EXCPR && ifndef DUMP_SLBS && ifndef DUMP_FPU && ifndef DUMP_CRS && ifndef DUMP_GPRS

#ifdef DUMP_FPU
    f << "|FPR:";
    for (int i{}; i != 32; ++i) {
      f << "|" << thread.FPR[i].valueAsDouble << "(0x" << std::hex << thread.FPR[i].valueAsU64 << ")";
    }
#endif

#ifdef DUMP_CRS
    f << "|CR:"
    << "0x" << std::hex << thread.CR.CR_Hex
    << "|0x" << std::hex << thread.CR.CR0
    << "|0x" << std::hex << thread.CR.CR1
    << "|0x" << std::hex << thread.CR.CR2
    << "|0x" << std::hex << thread.CR.CR3
    << "|0x" << std::hex << thread.CR.CR4
    << "|0x" << std::hex << thread.CR.CR5
    << "|0x" << std::hex << thread.CR.CR6
    << "|0x" << std::hex << thread.CR.CR7
#if !defined(DUMP_EXCPR) && !defined(DUMP_SLBS) && !defined(DUMP_FPU)
    << std::endl;
#else
    ;
#endif // ifndef DUMP_EXCPR && ifndef DUMP_SLBS && ifndef DUMP_FPU
#endif // DUMP_CRS

#ifdef DUMP_FPU
    f << "|FPSCR:"
    << "|0x" << std::hex << thread.FPSCR.FPSCR_Hex
    << "|0x" << std::hex << thread.FPSCR.RN
    << "|0x" << std::hex << thread.FPSCR.NI
    << "|0x" << std::hex << thread.FPSCR.XE
    << "|0x" << std::hex << thread.FPSCR.ZE
    << "|0x" << std::hex << thread.FPSCR.UE
    << "|0x" << std::hex << thread.FPSCR.OE
    << "|0x" << std::hex << thread.FPSCR.VE
    << "|0x" << std::hex << thread.FPSCR.VXCVI
    << "|0x" << std::hex << thread.FPSCR.VXSQRT
    << "|0x" << std::hex << thread.FPSCR.VXSOFT
    << "|0x" << std::hex << thread.FPSCR.R0
    << "|0x" << std::hex << thread.FPSCR.FPRF
    << "|0x" << std::hex << thread.FPSCR.FI
    << "|0x" << std::hex << thread.FPSCR.FR
    << "|0x" << std::hex << thread.FPSCR.VXVC
    << "|0x" << std::hex << thread.FPSCR.VXIMZ
    << "|0x" << std::hex << thread.FPSCR.VXZDZ
    << "|0x" << std::hex << thread.FPSCR.VXIDI
    << "|0x" << std::hex << thread.FPSCR.VXISI
    << "|0x" << std::hex << thread.FPSCR.VXSNAN
    << "|0x" << std::hex << thread.FPSCR.XX
    << "|0x" << std::hex << thread.FPSCR.ZX
    << "|0x" << std::hex << thread.FPSCR.UX
    << "|0x" << std::hex << thread.FPSCR.OX
    << "|0x" << std::hex << thread.FPSCR.VX
    << "|0x" << std::hex << thread.FPSCR.FEX
    << "|0x" << std::hex << thread.FPSCR.FX
    #if !defined(DUMP_EXCPR) && !defined(DUMP_SLBS)
    << std::endl;
    #else
    ;
    #endif
#endif

#ifdef DUMP_SLBS
    f << "|SLB:";
    for (int i{}; i != 64; ++i) {
      f
      << "|0x" << std::hex << (u32)thread.SLB[i].V
      << "|0x" << std::hex << (u32)thread.SLB[i].LP
      << "|0x" << std::hex << (u32)thread.SLB[i].C
      << "|0x" << std::hex << (u32)thread.SLB[i].L
      << "|0x" << std::hex << (u32)thread.SLB[i].N
      << "|0x" << std::hex << (u32)thread.SLB[i].Kp
      << "|0x" << std::hex << (u32)thread.SLB[i].Ks
      << "|0x" << std::hex << thread.SLB[i].VSID
      << "|0x" << std::hex << thread.SLB[i].ESID
      << "|0x" << std::hex << thread.SLB[i].vsidReg
      << "|0x" << std::hex << thread.SLB[i].esidReg;
    }
    #ifndef DUMP_EXCPR
    f << std::endl;
    #endif
#endif

#ifdef DUMP_EXCPR
    f << "|0x" << std::hex << thread.exceptReg;
    f << "|" << (int)thread.exceptionTaken;
    f << "|0x" << std::hex << thread.exceptEA;
    f << "|0x" << std::hex << thread.exceptTrapType;
    f << "|" << (int)thread.exceptHVSysCall;
    f << "|0x" << std::hex << thread.intEA;
    f << "|0x" << std::hex << thread.lastWriteAddress;
    f << "|0x" << std::hex << thread.lastRegValue
  << std::endl;
#endif
  }
#endif // CORE_DUMP

  // This is to set a PPU0[Thread0] breakpoint.
  if (hCore->ppuThread[hCore->currentThread].SPR.PIR == 0) {
    hCore->ppuThread[hCore->currentThread].lastRegValue =
        hCore->ppuThread[hCore->currentThread].GPR[11];
  }

  switch (currentInstr) {
    /*
case PPCInstruction::addcx:
    break;
    */
  case PPCInstruction::addex:
    PPCInterpreter_addex(hCore);
    break;
  case PPCInstruction::addi:
    PPCInterpreter_addi(hCore);
    break;
  case PPCInstruction::addic:
    PPCInterpreter_addic(hCore);
    break;
  case PPCInstruction::addicx:
    PPCInterpreter_addic_rc(hCore);
    break;
  case PPCInstruction::addis:
    PPCInterpreter_addis(hCore);
    break;
    /*
case PPCInstruction::addmex:
    break;
    */
  case PPCInstruction::addx:
    PPCInterpreter_addx(hCore);
    break;
  case PPCInstruction::addzex:
    PPCInterpreter_addzex(hCore);
    break;
  case PPCInstruction::andcx:
    PPCInterpreter_andc(hCore);
    break;
  case PPCInstruction::andix:
    PPCInterpreter_andi(hCore);
    break;
  case PPCInstruction::andisx:
    PPCInterpreter_andis(hCore);
    break;
  case PPCInstruction::andx:
    PPCInterpreter_and(hCore);
    break;
  case PPCInstruction::bcctrx:
    PPCInterpreter_bcctr(hCore);
    break;
  case PPCInstruction::bclrx:
    PPCInterpreter_bclr(hCore);
    break;
  case PPCInstruction::bcx:
    PPCInterpreter_bc(hCore);
    break;
  case PPCInstruction::bx:
    PPCInterpreter_b(hCore);
    break;
  case PPCInstruction::cmp:
    PPCInterpreter_cmp(hCore);
    break;
  case PPCInstruction::cmpi:
    PPCInterpreter_cmpi(hCore);
    break;
  case PPCInstruction::cmpl:
    PPCInterpreter_cmpl(hCore);
    break;
  case PPCInstruction::cmpli:
    PPCInterpreter_cmpli(hCore);
    break;
  case PPCInstruction::cntlzdx:
    PPCInterpreter_cntlzd(hCore);
    break;
  case PPCInstruction::cntlzwx:
    PPCInterpreter_cntlzw(hCore);
    break;
  case PPCInstruction::crand:
    PPCInterpreter_crand(hCore);
    break;
  case PPCInstruction::crandc:
    PPCInterpreter_crandc(hCore);
    break;
  case PPCInstruction::creqv:
    PPCInterpreter_creqv(hCore);
    break;
  case PPCInstruction::crnand:
    PPCInterpreter_crnand(hCore);
    break;
  case PPCInstruction::crnor:
    PPCInterpreter_crnor(hCore);
    break;
  case PPCInstruction::cror:
    PPCInterpreter_cror(hCore);
    break;
  case PPCInstruction::crorc:
    PPCInterpreter_crorc(hCore);
    break;
  case PPCInstruction::crxor:
    PPCInterpreter_crxor(hCore);
    break;
  case PPCInstruction::dcbf:
    break;
  case PPCInstruction::dcbi:
    break;
  case PPCInstruction::dcbst:
    PPCInterpreter_dcbst(hCore);
    break;
  case PPCInstruction::dcbt:
    break;
  case PPCInstruction::dcbtst:
    break;
  case PPCInstruction::dcbz:
    PPCInterpreter_dcbz(hCore);
    break;
  case PPCInstruction::divdux:
    PPCInterpreter_divdu(hCore);
    break;
  case PPCInstruction::divdx:
    PPCInterpreter_divd(hCore);
    break;
  case PPCInstruction::divwux:
    PPCInterpreter_divwux(hCore);
    break;
  case PPCInstruction::divwx:
    PPCInterpreter_divwx(hCore);
    break;
    /*
case PPCInstruction::eciwx:
    break;
case PPCInstruction::ecowx:
    break;
    */
  case PPCInstruction::eieio:
    break;
    /*
case PPCInstruction::eqvx:
    break;
    */
  case PPCInstruction::extsbx:
    PPCInterpreter_extsbx(hCore);
    break;
  case PPCInstruction::extshx:
    PPCInterpreter_extshx(hCore);
    break;
  case PPCInstruction::extswx:
    PPCInterpreter_extswx(hCore);
    break;
    /*
case PPCInstruction::fabsx:
    break;
case PPCInstruction::faddsx:
    break;
case PPCInstruction::faddx:
    break;
case PPCInstruction::fcfidx:
    break;
case PPCInstruction::fcmpo:
    break;
case PPCInstruction::fcmpu:
    break;
case PPCInstruction::fctidx:
    break;
case PPCInstruction::fctidzx:
    break;
case PPCInstruction::fctiwx:
    break;
case PPCInstruction::fctiwzx:
    break;
case PPCInstruction::fdivsx:
    break;
case PPCInstruction::fdivx:
    break;
case PPCInstruction::fmaddsx:
    break;
case PPCInstruction::fmaddx:
    break;
case PPCInstruction::fmrx:
    break;
case PPCInstruction::fmsubsx:
    break;
case PPCInstruction::fmsubx:
    break;
case PPCInstruction::fmulsx:
    break;
case PPCInstruction::fmulx:
    break;
case PPCInstruction::fnabsx:
    break;
case PPCInstruction::fnegx:
    break;
case PPCInstruction::fnmaddsx:
    break;
case PPCInstruction::fnmaddx:
    break;
case PPCInstruction::fnmsubsx:
    break;
case PPCInstruction::fnmsubx:
    break;
case PPCInstruction::fresx:
    break;
case PPCInstruction::frspx:
    break;
case PPCInstruction::frsqrtex:
    break;
case PPCInstruction::fselx:
    break;
case PPCInstruction::fsqrtsx:
    break;
case PPCInstruction::fsqrtx:
    break;
case PPCInstruction::fsubsx:
    break;
case PPCInstruction::fsubx:
    break;
    */
  case PPCInstruction::icbi:
    break;
  case PPCInstruction::isync:
    PPCInterpreter_isync(hCore);
    break;
  case PPCInstruction::lbz:
    PPCInterpreter_lbz(hCore);
    break;
  case PPCInstruction::lbzu:
    PPCInterpreter_lbzu(hCore);
    break;
  case PPCInstruction::lbzux:
    PPCInterpreter_lbzux(hCore);
    break;
  case PPCInstruction::lbzx:
    PPCInterpreter_lbzx(hCore);
    break;
  case PPCInstruction::ld:
    PPCInterpreter_ld(hCore);
    break;
  case PPCInstruction::ldarx:
    PPCInterpreter_ldarx(hCore);
    break;
    /*
case PPCInstruction::ldbrx:
    break;
    */
  case PPCInstruction::ldu:
    PPCInterpreter_ldu(hCore);
    break;
  case PPCInstruction::ldux:
    PPCInterpreter_ldux(hCore);
    break;
  case PPCInstruction::ldx:
    PPCInterpreter_ldx(hCore);
    break;
  case PPCInstruction::lfd:
    PPCInterpreter_lfd(hCore);
    break;
    /*
case PPCInstruction::lfdu:
    break;
case PPCInstruction::lfdux:
    break;
case PPCInstruction::lfdx:
    break;
    */
  case PPCInstruction::lfs:
    PPCInterpreter_lfs(hCore);
    break;
    /*
case PPCInstruction::lfsu:
    break;
case PPCInstruction::lfsux:
    break;
case PPCInstruction::lfsx:
    break;
    */
  case PPCInstruction::lha:
    PPCInterpreter_lha(hCore);
    break;

  case PPCInstruction::lhau:
    PPCInterpreter_lhau(hCore);
    break;
    /*
case PPCInstruction::lhaux:
    break;
    */
  case PPCInstruction::lhax:
    PPCInterpreter_lhax(hCore);
    break;
  case PPCInstruction::lhbrx:
    PPCInterpreter_lhbrx(hCore);
    break;
  case PPCInstruction::lhz:
    PPCInterpreter_lhz(hCore);
    break;
  case PPCInstruction::lhzu:
    PPCInterpreter_lhzu(hCore);
    break;
  case PPCInstruction::lhzux:
    PPCInterpreter_lhzux(hCore);
    break;
  case PPCInstruction::lhzx:
    PPCInterpreter_lhzx(hCore);
    break;
    /*
case PPCInstruction::lmw:
    break;
    */
  case PPCInstruction::lswi:
    PPCInterpreter_lswi(hCore);
    break;
    /*
case PPCInstruction::lswx:
    break;
    */
  case PPCInstruction::lwa:
    PPCInterpreter_lwa(hCore);
    break;
  case PPCInstruction::lwarx:
    PPCInterpreter_lwarx(hCore);
    break;
    /*
case PPCInstruction::lwaux:
    break;
    */
  case PPCInstruction::lwax:
    PPCInterpreter_lwax(hCore);
    break;
  case PPCInstruction::lwbrx:
    PPCInterpreter_lwbrx(hCore);
    break;
  case PPCInstruction::lwz:
    PPCInterpreter_lwz(hCore);
    break;
  case PPCInstruction::lwzu:
    PPCInterpreter_lwzu(hCore);
    break;
  case PPCInstruction::lwzux:
    PPCInterpreter_lwzux(hCore);
    break;
  case PPCInstruction::lwzx:
    PPCInterpreter_lwzx(hCore);
    break;
  case PPCInstruction::mcrf:
    PPCInterpreter_mcrf(hCore);
    break;
    /*
case PPCInstruction::mcrfs:
    break;
case PPCInstruction::mcrxr:
    break;
    */
  case PPCInstruction::mfcr:
    PPCInterpreter_mfcr(hCore);
    break;
    /*
case PPCInstruction::mffsx:
    break;
    */
  case PPCInstruction::mfmsr:
    PPCInterpreter_mfmsr(hCore);
    break;
    /*
case PPCInstruction::mfocrf:
    break;
    */
  case PPCInstruction::mfspr:
    PPCInterpreter_mfspr(hCore);
    break;
    /*
case PPCInstruction::mfsr:
    break;
case PPCInstruction::mfsrin:
    break;
    */
  case PPCInstruction::mftb:
    PPCInterpreter_mftb(hCore);
    break;
  case PPCInstruction::mtcrf:
    PPCInterpreter_mtcrf(hCore);
    break;
    /*
case PPCInstruction::mtfsb0x:
    break;
case PPCInstruction::mtfsb1x:
    break;
case PPCInstruction::mtfsfix:
    break;
case PPCInstruction::mtfsfx:
    break;
    */
  case PPCInstruction::mtmsr:
    PPCInterpreter_mtmsr(hCore);
    break;
  case PPCInstruction::mtmsrd:
    PPCInterpreter_mtmsrd(hCore);
    break;
    /*
case PPCInstruction::mtocrf:
    break;
    */
  case PPCInstruction::mtspr:
    PPCInterpreter_mtspr(hCore);
    break;
  /*
  case PPCInstruction::mtsr:
          break;
  case PPCInstruction::mtsrin:
          break;
          */
  case PPCInstruction::mulhdux:
    PPCInterpreter_mulhdux(hCore);
    break;
    /*
case PPCInstruction::mulhdx:
    break;
    */
  case PPCInstruction::mulhwux:
    PPCInterpreter_mulhwux(hCore);
    break;
    /*
case PPCInstruction::mulhwx:
    break;
    */
  case PPCInstruction::mulldx:
    PPCInterpreter_mulldx(hCore);
    break;
  case PPCInstruction::mulli:
    PPCInterpreter_mulli(hCore);
    break;
  case PPCInstruction::mullwx:
    PPCInterpreter_mullw(hCore);
    break;
  case PPCInstruction::nandx:
    PPCInterpreter_nandx(hCore);
    break;
  case PPCInstruction::negx:
    PPCInterpreter_negx(hCore);
    break;
  case PPCInstruction::norx:
    PPCInterpreter_norx(hCore);
    break;
case PPCInstruction::orcx:
    PPCInterpreter_orcx(hCore);
    break;
  case PPCInstruction::ori:
    PPCInterpreter_ori(hCore);
    break;
  case PPCInstruction::oris:
    PPCInterpreter_oris(hCore);
    break;
  case PPCInstruction::orx:
    PPCInterpreter_orx(hCore);
    break;
  case PPCInstruction::rfid:
    PPCInterpreter_rfid(hCore);
    break;
    /*
case PPCInstruction::rldclx:
    break;
    */
  case PPCInstruction::rldcrx:
    PPCInterpreter_rldcrx(hCore);
    break;
  case PPCInstruction::rldiclx:
    PPCInterpreter_rldiclx(hCore);
    break;
  case PPCInstruction::rldicrx:
    PPCInterpreter_rldicrx(hCore);
    break;
  case PPCInstruction::rldicx:
    PPCInterpreter_rldicx(hCore);
    break;
  case PPCInstruction::rldimix:
    PPCInterpreter_rldimix(hCore);
    break;
  case PPCInstruction::rlwimix:
    PPCInterpreter_rlwimix(hCore);
    break;
  case PPCInstruction::rlwinmx:
    PPCInterpreter_rlwinmx(hCore);
    break;
  case PPCInstruction::rlwnmx:
    PPCInterpreter_rlwnmx(hCore);
    break;
  case PPCInstruction::sc:
    PPCInterpreter_sc(hCore);
    break;
  case PPCInstruction::slbia:
    PPCInterpreter_slbia(hCore);
    break;
  case PPCInstruction::slbie:
    PPCInterpreter_slbie(hCore);
    break;
    /*
case PPCInstruction::slbmfee:
    break;
case PPCInstruction::slbmfev:
    break;
    */
  case PPCInstruction::slbmte:
    PPCInterpreter_slbmte(hCore);
    break;
  case PPCInstruction::sldx:
    PPCInterpreter_sldx(hCore);
    break;
  case PPCInstruction::slwx:
    PPCInterpreter_slwx(hCore);
    break;
  case PPCInstruction::sradix:
    PPCInterpreter_sradix(hCore);
    break;
  case PPCInstruction::sradx:
    PPCInterpreter_sradx(hCore);
    break;
  case PPCInstruction::srawix:
    PPCInterpreter_srawix(hCore);
    break;
  case PPCInstruction::srawx:
    PPCInterpreter_srawx(hCore);
    break;
  case PPCInstruction::srdx:
    PPCInterpreter_srdx(hCore);
    break;
  case PPCInstruction::srwx:
    PPCInterpreter_srwx(hCore);
    break;
  case PPCInstruction::stb:
    PPCInterpreter_stb(hCore);
    break;
  case PPCInstruction::stbu:
    PPCInterpreter_stbu(hCore);
    break;
  case PPCInstruction::stbux:
    PPCInterpreter_stbux(hCore);
    break;
  case PPCInstruction::stbx:
    PPCInterpreter_stbx(hCore);
    break;
  case PPCInstruction::std:
    PPCInterpreter_std(hCore);
    break;
  case PPCInstruction::stdcx:
    PPCInterpreter_stdcx(hCore);
    break;
  case PPCInstruction::stdu:
    PPCInterpreter_stdu(hCore);
    break;
  case PPCInstruction::stdux:
    PPCInterpreter_stdux(hCore);
    break;
  case PPCInstruction::stdx:
    PPCInterpreter_stdx(hCore);
    break;
  case PPCInstruction::stfd:
    PPCInterpreter_stfd(hCore);
    break;
    /*
case PPCInstruction::stdbrx:
    break;
case PPCInstruction::stfdu:
    break;
case PPCInstruction::stfdux:
    break;
case PPCInstruction::stfdx:
    break;
case PPCInstruction::stfiwx:
    break;
case PPCInstruction::stfs:
    break;
case PPCInstruction::stfsu:
    break;
case PPCInstruction::stfsux:
    break;
case PPCInstruction::stfsx:
    break;
    */
  case PPCInstruction::sth:
    PPCInterpreter_sth(hCore);
    break;
  case PPCInstruction::sthbrx:
    PPCInterpreter_sthbrx(hCore);
    break;
  case PPCInstruction::sthu:
    PPCInterpreter_sthu(hCore);
    break;
  case PPCInstruction::sthux:
    PPCInterpreter_sthux(hCore);
    break;
  case PPCInstruction::sthx:
    PPCInterpreter_sthx(hCore);
    break;
    /*
case PPCInstruction::stmw:
    break;
    */
  case PPCInstruction::stswi:
    PPCInterpreter_stswi(hCore);
    break;
    /*
case PPCInstruction::stswx:
    break;
    */
  case PPCInstruction::stw:
    PPCInterpreter_stw(hCore);
    break;
  case PPCInstruction::stwbrx:
    PPCInterpreter_stwbrx(hCore);
    break;
  case PPCInstruction::stwcx:
    PPCInterpreter_stwcx(hCore);
    break;
  case PPCInstruction::stwu:
    PPCInterpreter_stwu(hCore);
    break;
  case PPCInstruction::stwux:
    PPCInterpreter_stwux(hCore);
    break;
  case PPCInstruction::stwx:
    PPCInterpreter_stwx(hCore);
    break;
  case PPCInstruction::subfcx:
    PPCInterpreter_subfcx(hCore);
    break;
  case PPCInstruction::subfex:
    PPCInterpreter_subfex(hCore);
    break;
  case PPCInstruction::subfic:
    PPCInterpreter_subfic(hCore);
    break;
    /*
case PPCInstruction::subfmex:
    break;
    */
  case PPCInstruction::subfx:
    PPCInterpreter_subfx(hCore);
    break;
    /*
case PPCInstruction::subfzex:
    break;
    */
  case PPCInstruction::sync:
    break;
    /*
case PPCInstruction::td:
    break;
    */
  case PPCInstruction::tdi:
    PPCInterpreter_tdi(hCore);
    break;
    /*
    case PPCInstruction::tlbia:
    break;
    */
  case PPCInstruction::tlbie:
    LOG_INFO(Xenon, "Interpreter: tlbie executed.");
    break;
  case PPCInstruction::tlbiel:
    PPCInterpreter_tlbiel(hCore);
    break;
  case PPCInstruction::tlbsync:
    break;
  case PPCInstruction::tw:
    PPCInterpreter_tw(hCore);
    break;
  case PPCInstruction::twi:
    PPCInterpreter_twi(hCore);
    break;
  case PPCInstruction::xori:
    PPCInterpreter_xori(hCore);
    break;
  case PPCInstruction::xoris:
    PPCInterpreter_xoris(hCore);
    break;
  case PPCInstruction::xorx:
    PPCInterpreter_xorx(hCore);
    break;
  default:
      LOG_CRITICAL(Xenon, "PPC Interpreter: Unknown or unimplemented instruction found: "
          "data: {:#x}, address: {:#x}, OpCode: {}.", hCore->ppuThread[hCore->currentThread].CI,
          hCore->ppuThread[hCore->currentThread].CIA, getOpcodeName(hCore->ppuThread[hCore->currentThread].CI));
    break;
  }
}

//
// Exception definitions.
//

/* Exception name(Reset Vector) */

// System reset Exception (0x100)
void PPCInterpreter::ppcResetException(PPU_STATE *hCore) {
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
void PPCInterpreter::ppcDataStorageException(PPU_STATE *hCore) {
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
void PPCInterpreter::ppcDataSegmentException(PPU_STATE *hCore) {
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
void PPCInterpreter::ppcInstStorageException(PPU_STATE *hCore) {
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
void PPCInterpreter::ppcInstSegmentException(PPU_STATE *hCore) {
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
void PPCInterpreter::ppcExternalException(PPU_STATE *hCore) {
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
void PPCInterpreter::ppcProgramException(PPU_STATE *hCore) {
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

void PPCInterpreter::ppcDecrementerException(PPU_STATE *hCore) {
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
void PPCInterpreter::ppcSystemCallException(PPU_STATE *hCore) {
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

void PPCInterpreter::ppcInterpreterTrap(PPU_STATE *hCore, u32 trapNumber) {
  if (trapNumber ==
      0x14) // DbgPrint, r3 = PCSTR stringAddress, r4 = int String Size.
  {
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
