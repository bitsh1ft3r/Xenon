#include <iostream>
#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

// Formward Declaration
XCPUContext* PPCInterpreter::intXCPUContext = nullptr;

void PPCInterpreter::ppcInterpreterExecute(XCPUContext* cpuContext)
{
	intXCPUContext = cpuContext;
	// While this cpu is running
	while (cpuContext->executionRunning)
	{
		// For each cpu core
		for (int currentCore = 0; currentCore < 6; currentCore++) {
			// Update global core ID
			cpuContext->currentCoreID = currentCore;
			// If CPU Core is running
			if (cpuContext->cpuCores[currentCore].coreRunning) {
				// Execute 10000 instructions
				for (int cycleCount = 0; cycleCount <= cpuContext->cpuCores[currentCore].SPR[SPR_TTR]; cycleCount++) {
					// Increase PC
					cpuContext->cpuCores[currentCore].CIA = cpuContext->cpuCores[currentCore].NIA;
					cpuContext->cpuCores[currentCore].NIA += 4;

					// Load next instruction data into cpuState
					cpuContext->cpuCores[currentCore].iFetch = true;
					cpuContext->cpuCores[currentCore].CI = MMURead32(cpuContext->cpuCores[currentCore].CIA);
					cpuContext->cpuCores[currentCore].iFetch = false;

					// Execute current instruction
					ppcExecuteSingleInstruction(&cpuContext->cpuCores[currentCore]);

					// Increase time base for current CPU core
					cpuContext->cpuCores[currentCore].TB.TB_Hex++;
					cpuContext->cpuCores[currentCore].SPR[SPR_TBL]++;

					// Check for pending interrupts & execute them
					if (cpuContext->cpuCores[currentCore].exceptionOcurred)
					{
						// Process and dispatch exceptions in order.
					}
				}
			}
		}
	}
}


void PPCInterpreter::ppcExecuteSingleInstruction(PPCState* hCore) {

	PPCInstruction currentInstr = getOpcode(hCore->CI);

	// RGH 2 for CB 6752
	if (hCore->CIA == 0x000000000200c8d0)
	{
		//hCore->GPR[0x5] = 0;
	}

	// HW_INIT Success CB_6752
	if (hCore->CIA == 0x0000000003003dd0)
	{
		//hCore->GPR[0x5] = 0;
	}
	// RGH 2 for CB_A 9188
	if (hCore->CIA == 0x000000000200c870)
	{
		hCore->GPR[0x5] = 0;
	}

	if (0)
	{
		std::cout << "( 0x" << hCore->CIA << ") " << getOpcodeName(hCore->CI) << std::endl;
	}

	// CPU's online patch for Xell
	if (hCore->CIA == 0x800000001c000c74)
	{
		hCore->GPR[0x3] = 0x3f;
	}

	switch (currentInstr)
	{
		/*
	case PPCInstruction::addcx:
		break;
	case PPCInstruction::addex:
		break;
		*/
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
		/*
	case PPCInstruction::addzex:
		break;
		*/
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
		/*
	case PPCInstruction::cntlzdx:
		break;
		*/
	case PPCInstruction::cntlzwx:
		PPCInterpreter_cntlzw(hCore);
		break;
		/*
	case PPCInstruction::crand:
		break;
	case PPCInstruction::crandc:
		break;
	case PPCInstruction::creqv:
		break;
	case PPCInstruction::crnand:
		break;
	case PPCInstruction::crnor:
		break;
	case PPCInstruction::cror:
		break;
	case PPCInstruction::crorc:
		break;
	case PPCInstruction::crxor:
		break;
		*/
	case PPCInstruction::dcbf:
		std::cout << "dcbf" << std::endl;
		break;
		/*
	case PPCInstruction::dcbi:
		break;
		*/
	case PPCInstruction::dcbst:
		PPCInterpreter_dcbst(hCore);
		break;
		/*
	case PPCInstruction::dcbt:
		break;
	case PPCInstruction::dcbtst:
		break;
		*/
	case PPCInstruction::dcbz:
		PPCInterpreter_dcbz(hCore);
		break;
	case PPCInstruction::divdux:
		PPCInterpreter_divdu(hCore);
		break;
		/*
	case PPCInstruction::divdx:
		break;
	case PPCInstruction::divwux:
		break;
	case PPCInstruction::divwx:
		break;
	case PPCInstruction::eciwx:
		break;
	case PPCInstruction::ecowx:
		break;
		*/
	case PPCInstruction::eieio:
		std::cout << "XCPU: eieio" << std::endl;
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
		/*
	case PPCInstruction::ldarx:
		break;
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
		/*
	case PPCInstruction::lfd:
		break;
	case PPCInstruction::lfdu:
		break;
	case PPCInstruction::lfdux:
		break;
	case PPCInstruction::lfdx:
		break;
	case PPCInstruction::lfs:
		break;
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
	case PPCInstruction::lswi:
		break;
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
		/*
	case PPCInstruction::mcrf:
		break;
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
	case PPCInstruction::mtmsr:
		break;
		*/
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
	case PPCInstruction::mulhwux:
		break;
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
		/*
	case PPCInstruction::nandx:
		break;
		*/
	case PPCInstruction::negx:
		PPCInterpreter_negx(hCore);
		break;
	case PPCInstruction::norx:
		PPCInterpreter_norx(hCore);
		break;
		/*
	case PPCInstruction::orcx:
		break;
		*/
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
		/*
	case PPCInstruction::slbie:
		break;
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
		/*
	case PPCInstruction::srawx:
		break;
		*/
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
		/*
	case PPCInstruction::stdcx:
		break;
		*/
	case PPCInstruction::stdu:
		PPCInterpreter_stdu(hCore);
		break;
	case PPCInstruction::stdux:
		PPCInterpreter_stdux(hCore);
		break;
	case PPCInstruction::stdx:
		PPCInterpreter_stdx(hCore);
		break;
		/*
	case PPCInstruction::stfd:
		break;
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
	case PPCInstruction::stswi:
		break;
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
		std::cout << "tlbie" << std::endl;
		break;
	case PPCInstruction::tlbiel:
		PPCInterpreter_tlbiel(hCore);
		break;
	case PPCInstruction::tlbsync:
		break;
		/*
	case PPCInstruction::tw:
		break;
	case PPCInstruction::twi:
		break;
		*/
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
		std::cout << "PPC Interpreter: Unknown or unimplemented instruction found: data 0x"
			<< hCore->CI << " addr 0x" << hCore->CIA << std::endl;
		break;
	}
}

void PPCInterpreter::ppcInstSegmentException(PPCState* hCore)
{
	hCore->SPR[SPR_SRR0] = hCore->CIA;
	hCore->SPR[SPR_SRR1] = hCore->MSR.MSR_Hex & (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex & ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex | (QMASK(0, 0) | QMASK(3, 3));
	hCore->NIA = hCore->SPR[SPR_HIOR] + 0x480;
	hCore->MSR.DR = 0;
	hCore->MSR.IR = 0;
}

void PPCInterpreter::ppcSystemCallException(PPCState* hCore, bool isHypervisorCall)
{
	hCore->SPR[SPR_SRR0] = hCore->NIA;
	hCore->SPR[SPR_SRR1] = hCore->MSR.MSR_Hex & (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex & ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex | QMASK(0, 0) | (isHypervisorCall ? 0 : QMASK(3, 3));
	hCore->NIA = hCore->SPR[SPR_HIOR] + + 0xc00;
	hCore->MSR.DR = 0;
	hCore->MSR.IR = 0;
}

void PPCInterpreter::ppcInstStorageException(PPCState* hCore, u64 SRR1)
{
	hCore->SPR[SPR_SRR0] = hCore->CIA;
	hCore->SPR[SPR_SRR1] = hCore->MSR.MSR_Hex & (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
	hCore->SPR[SPR_SRR1] |= SRR1;
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex & ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex | (QMASK(0, 0) | QMASK(3, 3));
	hCore->NIA = hCore->SPR[SPR_HIOR] + 0x400;
	hCore->MSR.DR = 0;
	hCore->MSR.IR = 0;
}

void PPCInterpreter::ppcDataStorageException(PPCState* hCore, u64 EA, u64 ISR)
{
	hCore->SPR[SPR_SRR0] = hCore->CIA;
	hCore->SPR[SPR_SRR1] = hCore->MSR.MSR_Hex & (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
	hCore->SPR[SPR_DSISR] = ISR;
	hCore->SPR[SPR_DAR] = EA;
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex & ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex | (QMASK(0, 0) | QMASK(3, 3));
	hCore->NIA = hCore->SPR[SPR_HIOR] + 0x300;
	hCore->MSR.DR = 0;
	hCore->MSR.IR = 0;
}

void PPCInterpreter::ppcDataSegmentException(PPCState* hCore, u64 EA)
{
	hCore->SPR[SPR_SRR0] = hCore->CIA;
	hCore->SPR[SPR_SRR1] = hCore->MSR.MSR_Hex & (QMASK(0, 32) | QMASK(37, 41) | QMASK(48, 63));
	hCore->SPR[SPR_DSISR] = 0; 
	hCore->SPR[SPR_DAR] = EA;
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex & ~(QMASK(48, 50) | QMASK(52, 55) | QMASK(58, 59) | QMASK(61, 63));
	hCore->MSR.MSR_Hex = hCore->MSR.MSR_Hex | (QMASK(0, 0) | QMASK(3, 3));
	hCore->NIA = hCore->SPR[SPR_HRMOR] + 0x380;
	hCore->MSR.DR = 0;
	hCore->MSR.IR = 0;
}
