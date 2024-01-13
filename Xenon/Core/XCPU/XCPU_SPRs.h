	#pragma once

// 
// Xenon Special Purpose Registers
//

// Taken from Xell sourcecode.


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