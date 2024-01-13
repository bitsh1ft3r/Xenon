#pragma once

#include <vector>

#include "Xenon/Base/TypeDefs.h"

namespace Xe
{
	namespace Core
	{
		namespace XCPU
		{

#pragma region Register Definitions
			//
			// PowerPC Register definitions
			//

			/*
			General-Purpose Registers (GPRs)
			*/
			typedef u64 GPR;

			/*
			Floating-Point Registers (FPRs)
			*/
			typedef double FPR;

			/*
			Condition Register

			The CR fields can be set in one of the following ways:
			• Specified fields of the CR can be set from a GPR by using the mtcrf and mtocrf instruction.
			• The contents of the XER[0–3] can be moved to another CR field by using the mcrf instruction.
			• A specified field of the XER can be copied to a specified field of the CR by using the mcrxr instruction.
			• A specified field of the FPSCR can be copied to a specified field of the CR by using the mcrfs instruction.
			• Logical instructions of the condition register can be used to perform logical operations on specified bits in
			the condition register.
			• CR0 can be the implicit result of an integer instruction.
			• CR1 can be the implicit result of a floating-point instruction.
			• A specified CR field can indicate the result of either an integer or floating-point compare instruction
			*/
			union CR
			{
				u32 CR_Hex;
				struct
				{
					u32 CR7 : 4;
					u32 CR6 : 4;
					u32 CR5 : 4;
					u32 CR4 : 4;
					u32 CR3 : 4;
					u32 CR2 : 4;
					u32 CR1 : 4;
					u32 CR0 : 4;
				};
			};

			/*
			Floating-Point Status and Control Register (FPSCR)
			*/
			union FPSCR
			{
				u32 FPSCR_Hex;
				struct
				{
					u32 RN : 2;
					u32 NI : 1;
					u32 XE : 1;
					u32 ZE : 1;
					u32 UE : 1;
					u32 OE : 1;
					u32 VE : 1;
					u32 VXCVI : 1;
					u32 VXSQRT : 1;
					u32 VXSOFT : 1;
					u32 R0 : 1;
					u32 FPRF : 5;
					u32 FI : 1;
					u32 FR : 1;
					u32 VXVC : 1;
					u32 VXIMZ : 1;
					u32 VXZDZ : 1;
					u32 VXIDI : 1;
					u32 VXISI : 1;
					u32 VXSNAN : 1;
					u32 XX : 1;
					u32 ZX : 1;
					u32 UX : 1;
					u32 OX : 1;
					u32 VX : 1;
					u32 FEX : 1;
					u32 FX : 1;
				};
			};

			/*
			 XER Register (XER)
			*/
			union XER
			{
				u64 XER_Hex;
				struct
				{
					u64 ByteCount : 7;
					u64 R0 : 21;
					u64 CA : 1;
					u64 OV : 1;
					u64 SO : 1;
					u64 R1 : 32;
				};
			};

			/*
			 Link Register (LR)
			*/
			typedef u64 LR;

			/*
			 Count Register (CTR)
			*/
			typedef u64 CTR;

			/*
			Time Base (TB)
			*/
			union TB
			{
				u64 TB_Hex;
				struct
				{
					u64 TBL : 32;
					u64 TBU : 32;
				};
			};

			/*
			Machine State Register (MSR)
			*/
			union MSR
			{
				u64 MSR_Hex;
				struct
				{
					u64 LE : 1;
					u64 RI : 1;
					u64 PMM : 1;
					u64 R0 : 1;
					u64 DR : 1;
					u64 IR : 1;
					u64 R1 : 2;
					u64 FE1 : 1;
					u64 BE : 1;
					u64 SE : 1;
					u64 FE0 : 1;
					u64 ME : 1;
					u64 FP : 1;
					u64 PR : 1;
					u64 EE : 1;
					u64 ILE : 1;
					u64 R2 : 1;
					u64 POW : 1;
					u64 R3 : 44;
					u64 SF : 1;
				};
			};

			/*
			Processor Version Register (PVR)
			*/
			union PVR
			{
				u64 PVR_Hex;
				struct
				{
					u64 Revision : 32;
					u64 Version : 32;
				};
			};

			/*
			SDR1

			SDR1 Bit Settings—64-Bit Implementations
			Bits Name | Description
			0–1 — Reserved
			2-45 HTABORG Physical base address of page table
			46–58 — Reserved
			59–63 HTABSIZE Encoded size of page table (used to generate mask)
			*/
			union SDR1
			{
				u64 SDR1_Hex;
				struct
				{
					u64 HTABSIZE : 5;
					u64 R0 : 12;
					u64 HTABORG : 44;
					u64 R1 : 2;
				};
			};

			// Segment Lookaside Buffer Entry
			struct SLBEntry
			{
				u8 C;
				u8 L;
				u8 N;
				u8 Kp;
				u8 Ks;
				u64 VSID;
				u8 V;
				u64 ESID;
			}; 

#pragma endregion

#pragma region Instruction Definitions
			//
			// PowerPC Instruction definitions
			//

			enum class PPC_Instruction
			{
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

#pragma endregion

			union PowerPCInstr
			{
				// 32 Bit instruction data.
				u32 instrData;

				// I Instruction Form
				struct
				{
					u32 LK : 1;
					u32 AA : 1;
					u32 LI : 24;
					u32 OPCD : 6;
				}I;

				// B Instruction Form
				struct
				{
					u32 LK : 1;
					u32 AA : 1;
					u32 BD : 14;
					u32 BI : 5;
					u32 BO : 5;
					u32 OPCD : 6;
				}B;

				// SC Instruction Form
				struct
				{
					u32 : 1;
					u32 _1 :1;
					u32 : 15;
					u32 : 5;
					u32 : 5;
					u32 OPCD : 6;
				}SC;

				// D Instruction Form
				struct
				{
					u32 d_SIMM_UIMM : 16;
					u32 rA : 5;
					u32 rD_rS_crfD_TO : 5;
					u32 OPCD : 6;
				}D;

				// DS Instruction Form
				struct
				{
					u32 XO : 2;
					u32 ds : 14;
					u32 A : 5;
					u32 rD_rS : 5;
					u32 OPCD : 6;
				}DS;

				// X Instruction Form
				struct
				{
					u32 Rc_1 : 1;
					u32 XO : 10;
					u32 rB_NB_SH_IMM : 5;
					u32 rA_SR_crfS : 5;
					u32 rD_rS_crfD_L_TO_crbD : 5;
					u32 OPCD : 6;
				}X;

				// XL Instruction Form
				struct
				{
					u32 LK : 1;
					u32 XO : 10;
					u32 crbB : 5;
					u32 BI_crbA_crfS : 5;
					u32 BO_crbD_crfD : 5;
					u32 OPCD : 6;
				}XL;

				// XFX Instruction Form
				struct
				{
					u32 : 1;
					u32 XO : 10;
					u32 spr_CRM_tbr : 10;
					u32 rD_rS : 5;
					u32 OPCD : 6;
				}XFX;

				// XFL Instruction Form
				struct
				{
					u32 Rc : 1;
					u32 XO : 10;
					u32 B : 5;
					u32 : 1;
					u32 FM : 8;
					u32 : 1;
					u32 OPCD : 6;
				}XFL;

				// XS Instruction Form
				struct
				{
					u32 Rc : 1;
					u32 sh1 : 1;
					u32 XO : 9;
					u32 sh : 5;
					u32 rA : 5;
					u32 rS : 5;
					u32 OPCD : 6;
				}XS;

				// XO Instruction Form
				struct
				{
					u32 Rc : 1;
					u32 XO : 9;
					u32 OE : 1;
					u32 rB : 5;
					u32 rA : 5;
					u32 rD : 5;
					u32 OPCD : 6;
				}XO;

				// A Instruction Form
				struct
				{
					u32 Rc : 1;
					u32 XO : 5;
					u32 C : 5;
					u32 rB : 5;
					u32 rA : 5;
					u32 rD : 5;
					u32 OPCD : 6;
				}A;

				// M Instruction Form
				struct
				{
					u32 Rc : 1;
					u32 ME : 5;
					u32 MB : 5;
					u32 SH_rB : 5;
					u32 rA : 5;
					u32 rS : 5;
					u32 OPCD : 6;
				}M;

				// MD Instruction Form
				struct
				{
					u32 Rc : 1;
					u32 sh5 : 1;
					u32 XO : 3;
					u32 mb_me_5 : 1; // Split mb_me field onto two for easier use.
					u32 mb_me : 5;
					u32 sh : 5;
					u32 rA : 5;
					u32 rS : 5;
					u32 OPCD : 6;
				}MD;

				// MDS Instruction Form
				struct
				{
					u32 Rc : 1;
					u32 XO : 4;
					u32 mb_me_1 : 1;
					u32 mb_me_5 : 5;
					u32 rB : 5;
					u32 rA : 5;
					u32 rS : 5;
					u32 OPCD : 6;
				}MDS;

			};

			// Memory Management Unit Context
			struct MMUContext
			{
				u64 sprTlb_Index;
				u64 sprTlb_RPN;
				u64 sprTlb_VPN;
			};

			//
			// Exception list
			//
			enum Exceptions
			{
				systemResetEx = 0x00000001,
				machineCheckEx = 0x0000002,
				dsiEx = 0x00000004,
				dataSegmentEx = 0x00000008,
				isiEx = 0x00000010,
				instructionSegmentEx = 0x00000020,
				extIntEx = 0x00000040,
				alignmentEx = 0x00000080,
				programEx = 0x00000100,
				fpuEx = 0x00000200,
				decrementerEx = 0x00000400,
				systemCallEx = 0x00000800,
				traceEx = 0x00001000,
				perfMonEx = 0x00002000
			};


			//
			// PowerPC State definition
			//

			struct PowerPCState
			{
				// Current Instruction Address
				u64 CIA;
				// Next Instruction Address
				u64 NIA;
				// Current instruciton data
				PowerPCInstr CI = { 0 };

				// General-Purpose Registers (32)
				GPR GPR[32] = { 0 };
				// Floating-Point Registers (32)
				FPR FPR[32] = { 0 };
				// Condition Register
				CR CR;
				// Floating-Point Status Control Register
				FPSCR FPSCR;
				// Link Register
				LR LR;
				// Machine-State Register
				MSR MSR;
				// Counter
				CTR CTR;
				// Fixed Point Exception Register (XER)
				XER XER;
				// Time Base Register
				TB TB;
				// SDR1
				SDR1 SDR1;
				// Processor Version Register
				PVR PVR;
				// Segment Lookaside Buffer
				u8 oldSLB[16] = { 0 };

				SLBEntry SLB[16] = { 0 };

				// Trap Efective Address
				u32 trapEA = 0;

				// Special Purpose Registers (1024)
				u64 SPR[1024] = { 0 };

				// Address Traslation Flag
				bool traslationInProgress = false;

				// Exception Flag
				u8 exceptionFlag = 0;

				// Instruction fetch flag
				bool iFetch = false;

				// Xbox 360 special registers
				u64 CPUFuses[12] = { 0 };

				// MMU Context
				MMUContext mmuContext;
			};
		}
	}
}