#include <iostream>

#include "PowerPC_Instruction.h"
#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

PPCInstruction PPCInterpreter::getOpcode(u32 instrData)
{
    u32 OPCD = ExtractBits(instrData, 0, 5);

    u32 XO_20to30 = ExtractBits(instrData, 20, 30);
    u32 XO_27to29 = ExtractBits(instrData, 27, 29);
    u32 XO_27to30 = ExtractBits(instrData, 27, 30);
    u32 XO_21to30 = ExtractBits(instrData, 21, 30);
    u32 XO_22to30 = ExtractBits(instrData, 22, 30);
    u32 XO_21to29 = ExtractBits(instrData, 21, 29);
    u32 XO_30to31 = ExtractBits(instrData, 30, 31);

    switch (OPCD)
    {
    case 2: // tdi
        return PPCInstruction::tdi;
        break;
    case 3: // twi
        return PPCInstruction::twi;
        break;
    case 7: // mulli
        return PPCInstruction::mulli;
        break;
    case 8: // subfic
        return PPCInstruction::subfic;
        break;
    case 10: // cmpli
        return PPCInstruction::cmpli;
        break;
    case 11: // cmpi
        return PPCInstruction::cmpi;
        break;
    case 12: // addic
        return PPCInstruction::addic;
        break;
    case 13: // addic.
        return PPCInstruction::addicx;
        break;
    case 14: // addi
        return PPCInstruction::addi;
        break;
    case 15: // addis
        return PPCInstruction::addis;
        break;
    case 16: // bcx
        return PPCInstruction::bcx;
        break;
    case 17: // sc
        return PPCInstruction::sc;
        break;
    case 18: // bx
        return PPCInstruction::bx;
        break;
    case 19: /* Subgroup 19 */
        switch (XO_20to30)
        {
        case 0: // mcrf
            return PPCInstruction::mcrf;
            break;
        case 16: // bclrx
            return PPCInstruction::bclrx;
            break;
        case 18: // rfid
            return PPCInstruction::rfid;
            break;
        case 33: // crnor
            return PPCInstruction::crnor;
            break;
        case 129: // crandc
            return PPCInstruction::crandc;
            break;
        case 150: // isync
            return PPCInstruction::isync;
            break;
        case 193: // crxor
            return PPCInstruction::crxor;
            break;
        case 225: // crnand
            return PPCInstruction::crnand;
            break;
        case 257: // crand
            return PPCInstruction::crand;
            break;
        case 289: // creqv
            return PPCInstruction::creqv;
            break;
        case 417: // crorc
            return PPCInstruction::crorc;
            break;
        case 449: // cror
            return PPCInstruction::cror;
            break;
        case 528: // bcctrx
            return PPCInstruction::bcctrx;
            break;
        }
        std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 19!" << std::endl;
        return PPCInstruction::invalidInstruction;
        break;
    case 20: // rlwimix
        return PPCInstruction::rlwimix;
        break;
    case 21: // rlwinmx
        return PPCInstruction::rlwinmx;
        break;
    case 23: // rlwnmx
        return PPCInstruction::rlwnmx;
        break;
    case 24: // ori
        return PPCInstruction::ori;
        break;
    case 25: // oris
        return PPCInstruction::oris;
        break;
    case 26: // xori
        return PPCInstruction::xori;
        break;
    case 27: // xoris
        return PPCInstruction::xoris;
        break;
    case 28: // andi.
        return PPCInstruction::andix;
        break;
    case 29: // andis.
        return PPCInstruction::andisx;
        break;
    case 30: /* Subgroup 30 */
        switch (XO_27to29)
        {
        case 0: // rldiclx
            return PPCInstruction::rldiclx;
            break;
        case 1: // rldicrx
            return PPCInstruction::rldicrx;
            break;
        case 2: // rldicx
            return PPCInstruction::rldicx;
            break;
        case 3: // rldimix
            return PPCInstruction::rldimix;
            break;
        }
        switch (XO_27to30)
        {
        case 8: // rldclx
            return PPCInstruction::rldclx;
            break;
        case 9: // rldcrx
            return PPCInstruction::rldcrx;
            break;
        }
        std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 30!" << std::endl;
        return PPCInstruction::invalidInstruction;
        break;
    case 31: /* Subgroup 31 */
        switch (XO_20to30)
        {
        case 278: // dcbt
            return PPCInstruction::dcbt;
            break;
        }
        switch (XO_21to30)
        {
        case 0: // cmp
            return PPCInstruction::cmp;
            break;
        case 4: // tw
            return PPCInstruction::tw;
            break;
        case 19: // mfcr
            return PPCInstruction::mfcr;
            break;
        case 20: // lwarx
            return PPCInstruction::lwarx;
            break;
        case 21: // ldx
            return PPCInstruction::ldx;
            break;
        case 23: // lwzx
            return PPCInstruction::lwzx;
            break;
        case 24: // slwx
            return PPCInstruction::slwx;
            break;
        case 26: // cntlzwx
            return PPCInstruction::cntlzwx;
            break;
        case 27: // sldx
            return PPCInstruction::sldx;
            break;
        case 28: // andx
            return PPCInstruction::andx;
            break;
        case 32: // cmpl
            return PPCInstruction::cmpl;
            break;
        case 53: // ldux
            return PPCInstruction::ldux;
            break;
        case 54: // dcbst
            return PPCInstruction::dcbst;
            break;
        case 55: // lwzux
            return PPCInstruction::lwzux;
            break;
        case 58: // cntlzdx
            return PPCInstruction::cntlzdx;
            break;
        case 60: // andcx
            return PPCInstruction::andcx;
            break;
        case 68: // td
            return PPCInstruction::td;
            break;
        case 83: // mfmsr
            return PPCInstruction::mfmsr;
            break;
        case 84: // ldarx
            return PPCInstruction::ldarx;
            break;
        case 86: // dcbf
            return PPCInstruction::dcbf;
            break;
        case 87: // lbzx
            return PPCInstruction::lbzx;
            break;
        case 119: // lbzux
            return PPCInstruction::lbzux;
            break;
        case 124: // norx
            return PPCInstruction::norx;
            break;
        case 144: // mtcrf
            return PPCInstruction::mtcrf;
            break;
        case 146: // mtmsr
            return PPCInstruction::mtmsr;
            break;
        case 149: // stdx
            return PPCInstruction::stdx;
            break;
        case 150: // stwcx.
            return PPCInstruction::stwcx;
            break;
        case 151: // stwx
            return PPCInstruction::stwx;
            break;
        case 178: // mtmsrd
            return PPCInstruction::mtmsrd;
            break;
        case 181: // stdux
            return PPCInstruction::stdux;
            break;
        case 183: // stwux
            return PPCInstruction::stwux;
            break;
        case 210: // mtsr
            return PPCInstruction::mtsr;
            break;
        case 214: // stdcx.
            return PPCInstruction::stdcx;
            break;
        case 215: // stbx
            return PPCInstruction::stbx;
            break;
        case 242: // mtsrin
            return PPCInstruction::mtsrin;
            break;
        case 247: // stbux
            return PPCInstruction::stbux;
            break;
        case 246: // dcbt
            return PPCInstruction::dcbt;
            break;
        case 279: // lhzx
            return PPCInstruction::lhzx;
            break;
        case 284: // eqvx
            return PPCInstruction::eqvx;
            break;
        case 274: // tlbiel
            return PPCInstruction::tlbiel;
            break;
        case 306: // tlbie
            return PPCInstruction::tlbie;
            break;
        case 310: // eciwx
            return PPCInstruction::eciwx;
            break;
        case 311: // lhzux
            return PPCInstruction::lhzux;
            break;
        case 316: // xorx
            return PPCInstruction::xorx;
            break;
        case 339: // mfspr
            return PPCInstruction::mfspr;
            break;
        case 341: // lwax
            return PPCInstruction::lwax;
            break;
        case 343: // lhax
            return PPCInstruction::lhax;
            break;
        case 370: // tlbia
            return PPCInstruction::tlbia;
            break;
        case 371: // mftb
            return PPCInstruction::mftb;
            break;
        case 373: // lwaux
            return PPCInstruction::lwaux;
            break;
        case 375: // lhaux
            return PPCInstruction::lhaux;
            break;
        case 407: // sthx
            return PPCInstruction::sthx;
            break;
        case 412: // orcx
            return PPCInstruction::orcx;
            break;
        case 434: // slbie
            return PPCInstruction::slbie;
            break;
        case 438: // ecowx
            return PPCInstruction::ecowx;
            break;
        case 439: // sthux
            return PPCInstruction::sthux;
            break;
        case 444: // orx
            return PPCInstruction::orx;
            break;
        case 467: // mtspr
            return PPCInstruction::mtspr;
            break;
        case 476: // nandx
            return PPCInstruction::nandx;
            break;
        case 498: // slbia
            return PPCInstruction::slbia;
            break;
        case 533: // lswx
            return PPCInstruction::lswx;
            break;
        case 534: // lwbrx
            return PPCInstruction::lwbrx;
            break;
        case 535: // lfsx
            return PPCInstruction::lfsx;
            break;
        case 536: // srwx
            return PPCInstruction::srwx;
            break;
        case 539: // srdx
            return PPCInstruction::srdx;
            break;
        case 566: // tlbsync
            return PPCInstruction::tlbsync;
            break;
        case 567: // lfsux
            return PPCInstruction::lfsux;
            break;
        case 595: // mfsr
            return PPCInstruction::mfsr;
            break;
        case 597: // lswi
            return PPCInstruction::lswi;
            break;
        case 598: // sync
            return PPCInstruction::sync;
            break;
        case 599: // lfdx
            return PPCInstruction::lfdx;
            break;
        case 631: // lfdux
            return PPCInstruction::lfdux;
            break;
        case 569: // mfsrin
            return PPCInstruction::mfsrin;
            break;
        case 915: // slbmfee
            return PPCInstruction::slbmfee;
            break;
        case 851: // slbmfev
            return PPCInstruction::slbmfev;
            break;
        case 402: // slbmte
            return PPCInstruction::slbmte;
            break;
        case 661: // stswx
            return PPCInstruction::stswx;
            break;
        case 662: // stwbrx
            return PPCInstruction::stwbrx;
            break;
        case 663: // stfsx
            return PPCInstruction::stfsx;
            break;
        case 695: // stfsux
            return PPCInstruction::stfsux;
            break;
        case 725: // stswi
            return PPCInstruction::stswi;
            break;
        case 727: // stfdx
            return PPCInstruction::stfdx;
            break;
        case 759: // stfdux
            return PPCInstruction::stfdux;
            break;
        case 790: // lhbrx
            return PPCInstruction::lhbrx;
            break;
        case 762: // srawx
            return PPCInstruction::srawx;
            break;
        case 794: // sradx
            return PPCInstruction::sradx;
            break;
        case 824: // srawix
            return PPCInstruction::srawix;
            break;
        case 854: // eieio
            return PPCInstruction::eieio;
            break;
        case 918: // sthbrx
            return PPCInstruction::sthbrx;
            break;
        case 922: // extshx
            return PPCInstruction::extshx;
            break;
        case 954: // extsbx
            return PPCInstruction::extsbx;
            break;
        case 982: // icbi
            return PPCInstruction::icbi;
            break;
        case 983: // stfiwx
            return PPCInstruction::stfiwx;
            break;
        case 986: // extswx
            return PPCInstruction::extswx;
            break;
        case 1014: // dcbz
            return PPCInstruction::dcbz;
            break;
        }
        switch (XO_22to30)
        {
        case 8: // subfcx
            return PPCInstruction::subfcx;
            break;
        case 9: // mulhdux
            return PPCInstruction::mulhdux;
            break;
        case 10: // addcx
            return PPCInstruction::addcx;
            break;
        case 11: // mulhwux
            return PPCInstruction::mulhwux;
            break;
        case 40: // subfx
            return PPCInstruction::subfx;
            break;
        case 73: // mulhdx
            return PPCInstruction::mulhdx;
            break;
        case 75: // mulhwx
            return PPCInstruction::mulhwx;
            break;
        case 104: // negx
            return PPCInstruction::negx;
            break;
        case 136: // subfex
            return PPCInstruction::subfex;
            break;
        case 138: // addex
            return PPCInstruction::addex;
            break;
        case 200: // subfzex
            return PPCInstruction::subfzex;
            break;
        case 202: // addzex
            return PPCInstruction::addzex;
            break;
        case 232: // subfmex
            return PPCInstruction::subfmex;
            break;
        case 233: // mulldx
            return PPCInstruction::mulldx;
            break;
        case 234: // addmex
            return PPCInstruction::addmex;
            break;
        case 235: // mullwx
            return PPCInstruction::mullwx;
            break;
        case 266: // addx
            return PPCInstruction::addx;
            break;
        case 457: // divdux
            return PPCInstruction::divdux;
            break;
        case 459: // divwux
            return PPCInstruction::divwux;
            break;
        case 489: // divdx
            return PPCInstruction::divdx;
            break;
        case 491: // divwx
            return PPCInstruction::divwx;
            break;
        }
        switch (XO_21to29)
        {
        case 413: // sradix
            return PPCInstruction::sradix;
            break;
        }
        std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 31!" << std::endl;
        return PPCInstruction::invalidInstruction;
        break;
    case 32: // lwz
        return PPCInstruction::lwz;
        break;
    case 33: // lwzu
        return PPCInstruction::lwzu;
        break;
    case 34: // lbz
        return PPCInstruction::lbz;
        break;
    case 35: // lbzu
        return PPCInstruction::lbzu;
        break;
    case 36: // stw
        return PPCInstruction::stw;
        break;
    case 37: // stwu
        return PPCInstruction::stwu;
        break;
    case 38: // stb
        return PPCInstruction::stb;
        break;
    case 39: // stbu
        return PPCInstruction::stbu;
        break;
    case 40: // lhz
        return PPCInstruction::lhz;
        break;
    case 41: // lhzu
        return PPCInstruction::lhzu;
        break;
    case 42: // lha
        return PPCInstruction::lha;
        break;
    case 43: // lhau
        return PPCInstruction::lhau;
        break;
    case 44: // sth
        return PPCInstruction::sth;
        break;
    case 45: // sthu
        return PPCInstruction::sthu;
        break;
    case 58:
        switch (XO_30to31)
        {
        case 0: // ld
            return PPCInstruction::ld;
            break;
        case 1: // ldu
            return PPCInstruction::ldu;
            break;
        case 2: // lwa
            return PPCInstruction::lwa;
            break;
        default:
            std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 58!" << std::endl;
            return PPCInstruction::invalidInstruction;
            break;
        }
    case 62:
        switch (XO_30to31)
        {
        case 0: // std
            return PPCInstruction::std;
            break;
        case 1: // stdu
            return PPCInstruction::stdu;
            break;
        default:
            std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 62!" << std::endl;
            return PPCInstruction::invalidInstruction;
            break;
        }
    default:
        std::cout << "PPC Decoder: Unimplemented instruction found, data 0x" << std::hex << instrData << std::endl;
        return PPCInstruction::invalidInstruction;
        break;
    }
}

std::string PPCInterpreter::getOpcodeName(u32 instrData)
{
    u32 OPCD = ExtractBits(instrData, 0, 5);

    u32 XO_20to30 = ExtractBits(instrData, 20, 30);
    u32 XO_27to29 = ExtractBits(instrData, 27, 29);
    u32 XO_27to30 = ExtractBits(instrData, 27, 30);
    u32 XO_21to30 = ExtractBits(instrData, 21, 30);
    u32 XO_22to30 = ExtractBits(instrData, 22, 30);
    u32 XO_21to29 = ExtractBits(instrData, 21, 29);
    u32 XO_30to31 = ExtractBits(instrData, 30, 31);

    switch (OPCD)
    {
    case 2: // tdi
        return "tdi";
        break;
    case 3: // twi
        return "twi";
        break;
    case 7: // mulli
        return "mulli";
        break;
    case 8: // subfic
        return "subfic";
        break;
    case 10: // cmpli
        return "cmpli";
        break;
    case 11: // cmpi
        return "cmpi";
        break;
    case 12: // addic
        return "addic";
        break;
    case 13: // addic.
        return "addicx";
        break;
    case 14: // addi
        return "addi";
        break;
    case 15: // addis
        return "addis";
        break;
    case 16: // bcx
        return "bcx";
        break;
    case 17: // sc
        return "sc";
        break;
    case 18: // bx
        return "bx";
        break;
    case 19: /* Subgroup 19 */
        switch (XO_20to30)
        {
        case 0: // mcrf
            return "mcrf";
            break;
        case 16: // bclrx
            return "bclrx";
            break;
        case 18: // rfid
            return "rfid";
            break;
        case 33: // crnor
            return "crnor";
            break;
        case 129: // crandc
            return "crandc";
            break;
        case 150: // isync
            return "isync";
            break;
        case 193: // crxor
            return "crxor";
            break;
        case 225: // crnand
            return "crnand";
            break;
        case 257: // crand
            return "crand";
            break;
        case 289: // creqv
            return "creqv";
            break;
        case 417: // crorc
            return "crorc";
            break;
        case 449: // cror
            return "cror";
            break;
        case 528: // bcctrx
            return "bcctrx";
            break;
        }
        std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 19!" << std::endl;
        return "INVALID";
        break;
    case 20: // rlwimix
        return "rlwimix";
        break;
    case 21: // rlwinmx
        return "rlwinmx";
        break;
    case 23: // rlwnmx
        return "rlwnmx";
        break;
    case 24: // ori
        return "ori";
        break;
    case 25: // oris
        return "oris";
        break;
    case 26: // xori
        return "xori";
        break;
    case 27: // xoris
        return "xoris";
        break;
    case 28: // andi.
        return "andix";
        break;
    case 29: // andis.
        return "andisx";
        break;
    case 30: /* Subgroup 30 */
        switch (XO_27to29)
        {
        case 0: // rldiclx
            return "rldiclx";
            break;
        case 1: // rldicrx
            return "rldicrx";
            break;
        case 2: // rldicx
            return "rldicx";
            break;
        case 3: // rldimix
            return "rldimix";
            break;
        }
        switch (XO_27to30)
        {
        case 8: // rldclx
            return "rldclx";
            break;
        case 9: // rldcrx
            return "rldcrx";
            break;
        }
        std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 30!" << std::endl;
        return "INVALID";
        break;
    case 31: /* Subgroup 31 */
        switch (XO_20to30)
        {
        case 278: // dcbt
            return "dcbt";
            break;
        }
        switch (XO_21to30)
        {
        case 0: // cmp
            return "cmp";
            break;
        case 4: // tw
            return "tw";
            break;
        case 19: // mfcr
            return "mfcr";
            break;
        case 20: // lwarx
            return "lwarx";
            break;
        case 21: // ldx
            return "ldx";
            break;
        case 23: // lwzx
            return "lwzx";
            break;
        case 24: // slwx
            return "slwx";
            break;
        case 26: // cntlzwx
            return "cntlzwx";
            break;
        case 27: // sldx
            return "sldx";
            break;
        case 28: // andx
            return "andx";
            break;
        case 32: // cmpl
            return "cmpl";
            break;
        case 53: // ldux
            return "ldux";
            break;
        case 54: // dcbst
            return "dcbst";
            break;
        case 55: // lwzux
            return "lwzux";
            break;
        case 58: // cntlzdx
            return "cntlzdx";
            break;
        case 60: // andcx
            return "andcx";
            break;
        case 68: // td
            return "td";
            break;
        case 83: // mfmsr
            return "mfmsr";
            break;
        case 84: // ldarx
            return "ldarx";
            break;
        case 86: // dcbf
            return "dcbf";
            break;
        case 87: // lbzx
            return "lbzx";
            break;
        case 119: // lbzux
            return "lbzux";
            break;
        case 124: // norx
            return "norx";
            break;
        case 144: // mtcrf
            return "mtcrf";
            break;
        case 146: // mtmsr
            return "mtmsr";
            break;
        case 149: // stdx
            return "stdx";
            break;
        case 150: // stwcx.
            return "stwcx";
            break;
        case 151: // stwx
            return "stwx";
            break;
        case 178: // mtmsrd
            return "mtmsrd";
            break;
        case 181: // stdux
            return "stdux";
            break;
        case 183: // stwux
            return "stwux";
            break;
        case 210: // mtsr
            return "mtsr";
            break;
        case 214: // stdcx.
            return "stdcx";
            break;
        case 215: // stbx
            return "stbx";
            break;
        case 242: // mtsrin
            return "mtsrin";
            break;
        case 247: // stbux
            return "stbux";
            break;
        case 246: // dcbt
            return "dcbt";
            break;
        case 279: // lhzx
            return "lhzx";
            break;
        case 284: // eqvx
            return "eqvx";
            break;
        case 274: // tlbiel
            return "tlbiel";
            break;
        case 306: // tlbie
            return "tlbie";
            break;
        case 310: // eciwx
            return "eciwx";
            break;
        case 311: // lhzux
            return "lhzux";
            break;
        case 316: // xorx
            return "xorx";
            break;
        case 339: // mfspr
            return "mfspr";
            break;
        case 341: // lwax
            return "lwax";
            break;
        case 343: // lhax
            return "lhax";
            break;
        case 370: // tlbia
            return "tlbia";
            break;
        case 371: // mftb
            return "mftb";
            break;
        case 373: // lwaux
            return "lwaux";
            break;
        case 375: // lhaux
            return "lhaux";
            break;
        case 407: // sthx
            return "sthx";
            break;
        case 412: // orcx
            return "orcx";
            break;
        case 434: // slbie
            return "slbie";
            break;
        case 438: // ecowx
            return "ecowx";
            break;
        case 439: // sthux
            return "sthux";
            break;
        case 444: // orx
            return "orx";
            break;
        case 467: // mtspr
            return "mtspr";
            break;
        case 476: // nandx
            return "nandx";
            break;
        case 498: // slbia
            return "slbia";
            break;
        case 533: // lswx
            return "lswx";
            break;
        case 534: // lwbrx
            return "lwbrx";
            break;
        case 535: // lfsx
            return "lfsx";
            break;
        case 536: // srwx
            return "srwx";
            break;
        case 539: // srdx
            return "srdx";
            break;
        case 566: // tlbsync
            return "tlbsync";
            break;
        case 567: // lfsux
            return "lfsux";
            break;
        case 595: // mfsr
            return "mfsr";
            break;
        case 597: // lswi
            return "lswi";
            break;
        case 598: // sync
            return "sync";
            break;
        case 599: // lfdx
            return "lfdx";
            break;
        case 631: // lfdux
            return "lfdux";
            break;
        case 569: // mfsrin
            return "mfsrin";
            break;
        case 915: // slbmfee
            return "slbmfee";
            break;
        case 851: // slbmfev
            return "slbmfev";
            break;
        case 402: // slbmte
            return "slbmte";
            break;
        case 661: // stswx
            return "stswx";
            break;
        case 662: // stwbrx
            return "stwbrx";
            break;
        case 663: // stfsx
            return "stfsx";
            break;
        case 695: // stfsux
            return "stfsux";
            break;
        case 725: // stswi
            return "stswi";
            break;
        case 727: // stfdx
            return "stfdx";
            break;
        case 759: // stfdux
            return "stfdux";
            break;
        case 790: // lhbrx
            return "lhbrx";
            break;
        case 762: // srawx
            return "srawx";
            break;
        case 794: // sradx
            return "sradx";
            break;
        case 824: // srawix
            return "srawix";
            break;
        case 854: // eieio
            return "eieio";
            break;
        case 918: // sthbrx
            return "sthbrx";
            break;
        case 922: // extshx
            return "extshx";
            break;
        case 954: // extsbx
            return "extsbx";
            break;
        case 982: // icbi
            return "icbi";
            break;
        case 983: // stfiwx
            return "stfiwx";
            break;
        case 986: // extswx
            return "extswx";
            break;
        case 1014: // dcbz
            return "dcbz";
            break;
        }
        switch (XO_22to30)
        {
        case 8: // subfcx
            return "subfcx";
            break;
        case 9: // mulhdux
            return "mulhdux";
            break;
        case 10: // addcx
            return "addcx";
            break;
        case 11: // mulhwux
            return "mulhwux";
            break;
        case 40: // subfx
            return "subfx";
            break;
        case 73: // mulhdx
            return "mulhdx";
            break;
        case 75: // mulhwx
            return "mulhwx";
            break;
        case 104: // negx
            return "negx";
            break;
        case 136: // subfex
            return "subfex";
            break;
        case 138: // addex
            return "addex";
            break;
        case 200: // subfzex
            return "subfzex";
            break;
        case 202: // addzex
            return "addzex";
            break;
        case 232: // subfmex
            return "subfmex";
            break;
        case 233: // mulldx
            return "mulldx";
            break;
        case 234: // addmex
            return "addmex";
            break;
        case 235: // mullwx
            return "mullwx";
            break;
        case 266: // addx
            return "addx";
            break;
        case 457: // divdux
            return "divdux";
            break;
        case 459: // divwux
            return "divwux";
            break;
        case 489: // divdx
            return "divdx";
            break;
        case 491: // divwx
            return "divwx";
            break;
        }
        switch (XO_21to29)
        {
        case 413: // sradix
            return "sradix";
            break;
        }
        std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 31!" << std::endl;
        return "INVALID";
        break;
    case 32: // lwz
        return "lwz";
        break;
    case 33: // lwzu
        return "lwzu";
        break;
    case 34: // lbz
        return "lbz";
        break;
    case 35: // lbzu
        return "lbzu";
        break;
    case 36: // stw
        return "stw";
        break;
    case 37: // stwu
        return "stwu";
        break;
    case 38: // stb
        return "stb";
        break;
    case 39: // stbu
        return "stbu";
        break;
    case 40: // lhz
        return "lhz";
        break;
    case 41: // lhzu
        return "lhzu";
        break;
    case 44: // sth
        return "sth";
        break;
    case 58:
        switch (XO_30to31)
        {
        case 0: // ld
            return "ld";
            break;
        case 1: // ldu
            return "ldu";
            break;
        default:
            std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 58!" << std::endl;
            return "INVALID";
            break;
        }
    case 62:
        switch (XO_30to31)
        {
        case 0: // std
            return "std";
            break;
        case 1: // stdu
            return "stdu";
            break;
        default:
            std::cout << "PPC Decoder: Unimplemented instruction found, SUBGROUP 62!" << std::endl;
            return "INVALID";
            break;
        }

    default:
        std::cout << "PPC Decoder: Unimplemented instruction found, data 0x" << std::hex << instrData << std::endl;
        return "INVALID";
        break;
    }
}