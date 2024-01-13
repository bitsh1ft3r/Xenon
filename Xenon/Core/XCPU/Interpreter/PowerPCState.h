#pragma once

#include "Xenon/Base/TypeDefs.h"

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
		u64 : 1;
		u64 DR : 1;
		u64 IR : 1;
		u64 : 2;
		u64 FE1 : 1;
		u64 BE : 1;
		u64 SE : 1;
		u64 FE0 : 1;
		u64 ME : 1;
		u64 FP : 1;
		u64 PR : 1;
		u64 EE : 1;
		u64 ILE : 1;
		u64 : 8;
		u64 VXU : 1;
		u64 : 34;
		u64 HV : 1;
		u64 : 1;
		u64 TA : 1;
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
	u8 LP; // Large Page selector
	u8 C;
	u8 L;
	u8 N;
	u8 Kp;
	u8 Ks;
	u64 VSID;
	u8 V;
	u64 ESID;
};

// TLB_Index register
union TLB_Index
{
	u64 TLB_Index_Data;
	struct
	{
		u64 TS : 4;
		u64 TI : 8;
		u64 : 25;
		u64 LVPN : 11;
		u64 : 16;
	};
};

union PTE_RPN
{
	u64 hexData;
	struct
	{
		u64 PP : 2;
		u64 N : 1;
		u64 G : 1;
		u64 M : 1;
		u64 I : 1;
		u64 W : 1;
		u64 C : 1;
		u64 R : 1;
		u64 AC : 1;
		u64 : 2;
		u64 LP : 1;
		u64 RPN : 49;
		u64 : 2;
	};
};

union PTE_VPN	
{
	u64 hexData;
	struct
	{
		u64 V : 1;
		u64 H : 1;
		u64 L : 1;
		u64 SW : 4;
		u64 AVPN : 57;
	};
};

// Page Table on the PowerPC CELL BE
struct PTE_UPPER
{
	u64 pteUpperData;
	u64 AVPN;
	u64 SW;
	bool L;
	bool H;
	bool V;
};

struct PTE_LOWER
{
	u64 pteLowerData;
	u64 RPN;
	bool LP;
	bool AC;
	bool R;
	bool C;
	bool W;
	bool I;
	bool M;
	bool G;
	bool N;
	u8 PP;
};

// Traslation lookaside buffer
struct TLBEntry
{
	bool V;		// Entry valid.
	u8 p;		// Page Size
	u64 RPN;	// Real page Number
	u64 VPN;	// Virtual Page Number
	bool L;		// Large pages
	bool LP;	// Large page selector
	u8 tlbIndex;
	u8 LVPN;
	PTE_RPN pteRPN;
	PTE_VPN pteVPN;
};
struct TLB_Reg
{
	TLBEntry tlbSet0[256];
	TLBEntry tlbSet1[256];
	TLBEntry tlbSet2[256];
	TLBEntry tlbSet3[256];
};

//
// PowerPC State definition
//

struct PPCState
{
	// Current Instruction Address
	u64 CIA;
	// Next Instruction Address
	u64 NIA;
	// Current instruction data
	u32 CI;

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

	// L1 Cache 32Kb
	u8* L1;

	// Address Traslation Flag
	bool traslationInProgress = false;

	// Exception Flag
	bool exceptionOcurred = false;

	// Core running?
	bool coreRunning = false;

	// Instruction fetch flag
	bool iFetch = false;

	// Xbox 360 special registers
	u64 CPUFuses[12] = { 0 };

	// TLB
	TLB_Reg TLB;

	// Last Jump Address
	u64 lastJumpAddress = 0;
};

// 
// Xenon Special Purpose Registers
//

#define SPR_XER			1
#define SPR_LR			8
#define SPR_CTR			9
#define SPR_DSISR		18
#define SPR_DAR			19
#define SPR_DEC			22
#define SPR_SDR1		25
#define SPR_SRR0		26
#define SPR_SRR1		27
#define SPR_ESR 		62
#define SPR_IVPR 		63
#define SPR_PID 		48
#define SPR_CTRLRD		136		
#define SPR_CTRLWR		152
#define SPR_SPRG0		272
#define SPR_SPRG1		273
#define SPR_SPRG2		274
#define SPR_TBL			284
#define SPR_TBU			285
#define SPR_PVR     	287
#define SPR_HSPRG0		304
#define SPR_HSPRG1		305
#define SPR_HDSISR		306
#define SPR_HDAR		307
#define SPR_DBCR0		308
#define SPR_DBCR1		309
#define SPR_HDEC		310
#define SPR_HIOR		311
#define SPR_RMOR    	312
#define SPR_HRMOR   	313
#define SPR_HSRR0		314
#define SPR_HSRR1		315
#define SPR_DAC1		316
#define SPR_DAC2		317
#define SPR_LPCR    	318
#define SPR_LPIDR		319
#define SPR_TSR		    336
#define SPR_TCR		    340
#define SPR_SIAR		780
#define SPR_SDAR		781
#define SPR_TSRL		896
#define SPR_TSRR		897
#define SPR_TSCR		921
#define SPR_TTR			922
#define SPR_PpeTlbIndexHint	946
#define SPR_PpeTlbIndex	947
#define SPR_PpeTlbVpn	948
#define SPR_PpeTlbRpn	949
#define SPR_PpeTlbRmt	951
#define SPR_DSR0		952
#define SPR_DRMR0		953
#define SPR_DCIDR0		954
#define SPR_DRSR1		955
#define SPR_DRMR1		956
#define SPR_DCIDR1		957
#define SPR_ISSR0		976
#define SPR_IRMR0		977
#define SPR_ICIDR0		978
#define SPR_IRSR1		979
#define SPR_IRMR1		980
#define SPR_ICIDR1		981
#define SPR_HID0		1008
#define SPR_HID1		1009
#define SPR_IABR    	1010
#define SPR_HID4		1012
#define SPR_DABR		1013
#define SPR_HID5		1014
#define SPR_DABRX		1015
#define SPR_BUSCSR  	1016
#define SPR_HID6    	1017
#define SPR_L2SR    	1018
#define SPR_BPVR		1022        
#define SPR_PIR			1023