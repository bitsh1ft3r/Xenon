#include <iostream>

#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

//
// Xbox 360 Memory map, info taken from various sources.
//

// Everything can fit on 32 bits on the 360, so MS uses upper bits of the 64 bit
// EA to manage L2 cache, further research required on this.
// 0x200 00000000 200 apparently is used for NONCACHED memory.
// 0x100 00000000 100 apparently is used for CACHED/HASHED/ENCRYPTED
// 0x100 - 1FF        is used, currently unknown of its effect. Maybe someone
//                    can make this clearer.
                 
// 0x200 00000000 - 0x200 00008000                  32K SROM - 1BL Location.
// 0x200 00010000 - 0x200 00020000                  64K SRAM.
// 0x200 00050000 - 0x200 00056000                  Interrupt controller.
// 0x200 C8000000 - 0x200 C9000000                  NAND Flash 1:1
// 0x200 C9000000 - 0x200 CA000000                  Currently unknown, I 
// suspect that maybe it is additional space for 512 MB NAND Flash images.
// 0x200 EA000000 - 0x200 EA010000                  PCI Bridge
// 0x200 EC800000 - 0x200 EC810000                  GPU

#define MMU_PAGE_SIZE_4KB       12
#define MMU_PAGE_SIZE_64KB      16
#define MMU_PAGE_SIZE_1MB       20
#define MMU_PAGE_SIZE_16MB      24


void PPCInterpreter::PPCInterpreter_slbia(PPCState* hCore)
{
    for (auto slbEntry : hCore->SLB) {
        slbEntry.V = 0;
    }
}

void PPCInterpreter::PPCInterpreter_tlbiel(PPCState* hCore)
{
    X_FORM_L_rB;
    
    // The PPU adds two new fields to this instruction, them being LP abd IS.

    bool LP = (hCore->GPR[rB] & 0x1000) >> 12;
    bool invalSelector = (hCore->GPR[rB] & 0x800) >> 11;
    u8 p = mmuGetPageSize(hCore, L, LP);
    u64 VPN = 0;

    if (invalSelector == 0)
    {
        // The TLB is as selective as possible when invalidating TLB entries.The
        // invalidation match criteria is VPN[38:79 - p], L, LP, and LPID.

        VPN = QGET(hCore->GPR[rB], 22, 63 - p);

        std::cout << "TLB IL: Inval Selector = 0, VPN = 0x" << VPN << " p = " 
            << (u16)p << std::endl;

        for (auto& tlbEntry : hCore->TLB.tlbSet0) {
            if (tlbEntry.V && tlbEntry.VPN == VPN && tlbEntry.p == p)
            {
                tlbEntry.V = 0;
                std::cout << " *** TLB Set 0: Invalidated entry with RPN: 0x"
                    << tlbEntry.RPN << " VPN = 0x" << tlbEntry.RPN << std::endl;
            }
        }
        for (auto& tlbEntry : hCore->TLB.tlbSet1) {
            if (tlbEntry.V && tlbEntry.VPN == VPN && tlbEntry.p == p)
            {
                tlbEntry.V = 0;
                std::cout << " *** TLB Set 1: Invalidated entry with RPN: 0x" 
                    << tlbEntry.RPN << " VPN = 0x" << tlbEntry.RPN << std::endl;
            }
        }
        for (auto& tlbEntry : hCore->TLB.tlbSet2) {
            if (tlbEntry.V && tlbEntry.VPN == VPN && tlbEntry.p == p)
            {
                tlbEntry.V = 0;
                std::cout << " *** TLB Set 2: Invalidated entry with RPN: 0x"
                    << tlbEntry.RPN << " VPN = 0x" << tlbEntry.RPN << std::endl;
            }
        }
        for (auto& tlbEntry : hCore->TLB.tlbSet3) {
            if (tlbEntry.V && tlbEntry.VPN == VPN && tlbEntry.p == p)
            {
                tlbEntry.V = 0;
                std::cout << " *** TLB Set 3: Invalidated entry with RPN: 0x"
                    << tlbEntry.RPN << " VPN = 0x" << tlbEntry.RPN << std::endl;
            }
        }
    }
    else
    {
        // Index to one of the 256 rows of the tlb.
        u8 tlbCongruenceClass = 0;
        u64 rb_44_51 = hCore->GPR[rB] & 0xFF000;

        // 52-55 bits of 80 VA
        u8 bits36_39 = static_cast<u8>(QGET(VPN, 36, 39));
        // 56-59 bits of 80 VA
        u8 bits40_43 = static_cast<u8>(QGET(VPN, 40, 43));
        // 60-63 bits of 80 VA
        u8 bits44_47 = static_cast<u8>(QGET(VPN, 44, 47));
        // 64-67 bits of 80 VA
        u8 bits48_51 = static_cast<u8>(QGET(VPN, 48, 51));
        // 48-55 bits of 80 VA
        u8 bits32_39 = static_cast<u8>(QGET(VPN, 32, 39));

        for (auto& tlbEntry : hCore->TLB.tlbSet0)
        {
            if (tlbEntry.V)
            {
                switch (p)
                {
                case MMU_PAGE_SIZE_64KB:
                    if (((QGET(VPN, 36, 39) / QGET(VPN, 40, 43)) 
                        | QGET(VPN, 44, 47)) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                case MMU_PAGE_SIZE_16MB:
                    if (QGET(tlbEntry.VPN, 32, 39) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                default:
                    // 4Kb page size.
                    if (((QGET(VPN, 36, 39) / QGET(VPN, 44, 47)) 
                        | QGET(VPN, 48, 51)) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                }
            }
        }
        for (auto& tlbEntry : hCore->TLB.tlbSet1)
        {
            if (tlbEntry.V)
            {
                switch (p)
                {
                case MMU_PAGE_SIZE_64KB:
                    if (((QGET(VPN, 36, 39) / QGET(VPN, 40, 43)) 
                        | QGET(VPN, 44, 47)) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                case MMU_PAGE_SIZE_16MB:
                    if (QGET(tlbEntry.VPN, 32, 39) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                default:
                    // 4Kb page size.
                    if (((QGET(VPN, 36, 39) / QGET(VPN, 44, 47)) 
                        | QGET(VPN, 48, 51)) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                }
            }
        }
        for (auto& tlbEntry : hCore->TLB.tlbSet2)
        {
            if (tlbEntry.V)
            {
                switch (p)
                {
                case MMU_PAGE_SIZE_64KB:
                    if (((QGET(VPN, 36, 39) / QGET(VPN, 40, 43)) 
                        | QGET(VPN, 44, 47)) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                case MMU_PAGE_SIZE_16MB:
                    if (QGET(tlbEntry.VPN, 32, 39) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                default:
                    // 4Kb page size.
                    if (((QGET(VPN, 36, 39) / QGET(VPN, 44, 47)) 
                        | QGET(VPN, 48, 51)) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                }
            }
        }
        for (auto& tlbEntry : hCore->TLB.tlbSet3)
        {
            if (tlbEntry.V)
            {
                switch (p)
                {
                case MMU_PAGE_SIZE_64KB:
                    if (((QGET(VPN, 36, 39) / QGET(VPN, 40, 43)) | 
                        QGET(VPN, 44, 47)) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                case MMU_PAGE_SIZE_16MB:
                    if (QGET(tlbEntry.VPN, 32, 39) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                default:
                    // 4Kb page size.
                    if (((QGET(VPN, 36, 39) / QGET(VPN, 44, 47)) | 
                        QGET(VPN, 48, 51)) == rb_44_51)
                        tlbEntry.V = false;
                    break;
                }
            }
        }
    }

}

// Helper function for getting Page Size (p bit).
u8 PPCInterpreter::mmuGetPageSize(PPCState* hCore, bool L, u8 LP)
{

    // Large page selection works the following way:
    // First check if pages are large (L)
    // if (L) the page size can be one of two defined pages. On the XBox 360,
    // MS decided to use two of the three page sizes, 64Kb and 16Mb.
    // Selection between them is made using bits 16 - 19 of HID6 SPR.

    // HID6 16-17 bits select Large Page size 1.
    // HID6 18-19 bits select Large Page size 2.
    u8 LB_16_17 = (u8)QGET(hCore->SPR[SPR_HID6], 16, 17);
    u8 LB_18_19 = (u8)QGET(hCore->SPR[SPR_HID6], 18, 19);

    // Final p size.
    u8  p = 0;
    // Page size in decimal.
    u32 pSize = 0;

    // Large page?
    if (L == 0)
    {
        // If L equals 0, the small page size is used, 4Kb in this case.
        pSize = 4096;
    }
    else
    {
        // Large Page Selector
        if (LP == 0)
        {
            switch (LB_16_17)
            {
            case 0b0000:
                pSize = 16777216;   // 16 Mb page size
                break;
            case 0b0001:
                pSize = 1048576;    // 1 Mb page size
                break;
            case 0b0010:
                pSize = 65536;      // 64 Kb page size
                break;
            }
        }
        else if (LP == 1)
        {
            switch (LB_18_19)
            {
            case 0b0000:
                pSize = 16777216;   // 16 Mb page size
                break;
            case 0b0001:
                pSize = 1048576;    // 1 Mb page size
                break;
            case 0b0010:
                pSize = 65536;      // 16 Mb page size
                break;
            }
        }
    }

    // p size is Log(2) of Page Size.
    p = static_cast<u8>(log2(pSize));
    return p;
}

// This is done when TLB Reload is in software-controlled mode.
void PPCInterpreter::mmuAddTlbEntry(PPCState* hCore)
{
#define PPE_TLB_INDEX_LVPN_MASK         0xE00000000000
#define MMU_GET_TLB_INDEX_TI(x)         ((u16)((x & 0xFF0) >> 4))
#define MMU_GET_TLB_INDEX_TS(x)         ((u16)(x & 0xF))

#define PPE_TLB_VPN_V_MASK              0x1
#define PPE_TLB_VPN_L_MASK              0x4
#define PPE_TLB_VPN_AVPN_MASK           0x1FFFFFFFFFF80
#define PPE_TLB_VPN_AVPN_16MB_MASK      0x1FFFFFFFFFF00

#define PPE_TLB_RPN_ARPN_MASK           0x3FFFFFFE000
#define PPE_TLB_RPN_ARPN_AND_LP_MASK    0x3FFFFFFF000
#define PPE_TLB_RPN_LP_MASK             0x1000

    u64 tlbIndex = hCore->SPR[SPR_PpeTlbIndex];
    u64 tlbVpn = hCore->SPR[SPR_PpeTlbVpn];
    u64 tlbRpn = hCore->SPR[SPR_PpeTlbRpn];

    // Valid entry.
    bool vpnValid = tlbVpn & PPE_TLB_VPN_V_MASK;
    // Large pages.
    bool L = (tlbVpn & PPE_TLB_VPN_L_MASK) >> 2;
    // Large pages selector
    u32 LP = (tlbRpn & PPE_TLB_RPN_LP_MASK) >> 12;

    // Get Page Size
    u8 p = mmuGetPageSize(hCore, L, LP);

    // AVPN
    u64 AVPN = 0;

    // VPN = AVPN concatenated with LVPN
    // The AVPN corresponds to VPN[15:56].
    // When reading a 16 MB TLB entry, bit [56] of the AVPN is undefined.
    if (p == MMU_PAGE_SIZE_16MB) // 16 Mb entry
    {
        AVPN = tlbVpn & PPE_TLB_VPN_AVPN_16MB_MASK;
    }
    else
    {
        AVPN = tlbVpn & PPE_TLB_VPN_AVPN_MASK;
    }

    u64 ARPN = 0;

    if (L)
    {
        ARPN = tlbRpn & PPE_TLB_RPN_ARPN_MASK;
    }
    else
    {
        ARPN = tlbRpn & PPE_TLB_RPN_ARPN_AND_LP_MASK;
    }

    // LVPN[0:2] corresponds to VPN[57:59].
    u64 LVPN = (tlbIndex & PPE_TLB_INDEX_LVPN_MASK) >> 41;

    // Virtual page number
    u64 VPN = AVPN | LVPN;

    // Real Page Number
    // The ARPN corresponds to RPN[22:50].
    // To obtain the full 30 - bit RPN, the ARPN is combined with the LP field.
    // We already did that.
    u64 RPN = ARPN;
   
    // Tlb Index (0 - 255) of current tlb set.
    u16 TI = MMU_GET_TLB_INDEX_TI(tlbIndex);
    u16 TS = MMU_GET_TLB_INDEX_TS(tlbIndex);

    std::cout << "XCPU (MMU): Adding tlb entry:" << std::endl;
    std::cout << " *** RPN:         0x" << RPN << std::endl;
    std::cout << " *** VPN:         0x" << VPN << std::endl;
    std::cout << "  *  Large bit:   " << (L ? "true" : "false") << std::endl;
    std::cout << "  *  LP selector: " << (LP ? "true" : "false") << std::endl;
    std::cout << "  *  TLB Set:     " << std::dec << TS << std::endl;
    std::cout << "  *  TLB Index:   " << TI << std::endl;
    std::cout << "  *  p Size:      " << (u16)p << std::endl;
    std::cout << std::hex << std::endl;

    // TLB set to choose from
    // There are 4 sets of 256 entries each:
    switch (TS)
    {
    case 0b1000:
        hCore->TLB.tlbSet0[TI].V = true;
        hCore->TLB.tlbSet0[TI].RPN = RPN;
        hCore->TLB.tlbSet0[TI].VPN = VPN;
        hCore->TLB.tlbSet0[TI].p = p;
        hCore->TLB.tlbSet0[TI].L = L;
        hCore->TLB.tlbSet0[TI].LP = LP;
        break;
    case 0b0100:
        hCore->TLB.tlbSet1[TI].V = true;
        hCore->TLB.tlbSet1[TI].RPN = RPN;
        hCore->TLB.tlbSet1[TI].VPN = VPN;
        hCore->TLB.tlbSet1[TI].p = p;
        hCore->TLB.tlbSet1[TI].L = L;
        hCore->TLB.tlbSet1[TI].LP = LP;
        break;
    case 0b0010:
        hCore->TLB.tlbSet2[TI].V = true;
        hCore->TLB.tlbSet2[TI].RPN = RPN;
        hCore->TLB.tlbSet2[TI].VPN = VPN;
        hCore->TLB.tlbSet2[TI].p = p;
        hCore->TLB.tlbSet2[TI].L = L;
        hCore->TLB.tlbSet2[TI].LP = LP;
        break;
    case 0b0001:
        hCore->TLB.tlbSet3[TI].V = true;
        hCore->TLB.tlbSet3[TI].RPN = RPN;
        hCore->TLB.tlbSet3[TI].VPN = VPN;
        hCore->TLB.tlbSet3[TI].p = p;
        hCore->TLB.tlbSet3[TI].L = L;
        hCore->TLB.tlbSet3[TI].LP = LP;
        break;
    }
}

// Translation Lookaside Buffer Search
bool PPCInterpreter::mmuSearchTlbEntry(PPCState* hCore, u64* RPN, u64 VPN, 
    u8 p, bool LP)
{
    // Index to choose from the 256 ways of the TLB
    u8 tlbIndex = 0;

    // 4 Kb - (VA[52:55] xor VA[60:63]) || VA[64:67]
    // 64 Kb - (VA[52:55] xor VA[56:59]) || VA[60:63]
    // 16MB - VA[48:55]

    // 52-55 bits of 80 VA
    u8 bits36_39 = static_cast<u8>(QGET(VPN, 36, 39));
    // 56-59 bits of 80 VA
    u8 bits40_43 = static_cast<u8>(QGET(VPN, 40, 43));
    // 60-63 bits of 80 VA
    u8 bits44_47 = static_cast<u8>(QGET(VPN, 44, 47));
    // 64-67 bits of 80 VA
    u8 bits48_51 = static_cast<u8>(QGET(VPN, 48, 51));
    // 48-55 bits of 80 VA
    u8 bits32_39 = static_cast<u8>(QGET(VPN, 32, 39));

    switch (p)
    {
    case MMU_PAGE_SIZE_64KB:
        tlbIndex = ((bits36_39 ^ bits40_43) << 4) | bits44_47;
        break;
    case MMU_PAGE_SIZE_1MB:
        break;
    case MMU_PAGE_SIZE_16MB:
        tlbIndex = bits32_39;
        break;
    default:
        // p = 12 bits, 4KB
        tlbIndex = ((bits36_39 ^ bits40_43) | bits44_47);
        break;
    }

    if (hCore->TLB.tlbSet0[tlbIndex].V)
    {
        // Check to see if its this entry!
        // Need to have a tag
        if (hCore->TLB.tlbSet0[tlbIndex].LP == LP &&
            hCore->TLB.tlbSet0[tlbIndex].p == p &&
            hCore->TLB.tlbSet0[tlbIndex].L)
        {
            // Probably It!
            *RPN = hCore->TLB.tlbSet0[tlbIndex].RPN;
            return true;
        }
    }
    if (hCore->TLB.tlbSet1[tlbIndex].V)
    {
        // Check to see if its this entry!
        // Need to have a tag
        if (hCore->TLB.tlbSet1[tlbIndex].LP == LP &&
            hCore->TLB.tlbSet1[tlbIndex].p == p &&
            hCore->TLB.tlbSet1[tlbIndex].L)
        {
            // Probably It!
            *RPN = hCore->TLB.tlbSet1[tlbIndex].RPN;
            return true;
        }
    }
    if (hCore->TLB.tlbSet2[tlbIndex].V)
    {
        // Check to see if its this entry!
        // Need to have a tag
        if (hCore->TLB.tlbSet2[tlbIndex].LP == LP &&
            hCore->TLB.tlbSet2[tlbIndex].p == p &&
            hCore->TLB.tlbSet2[tlbIndex].L)
        {
            // Probably It!
            *RPN = hCore->TLB.tlbSet2[tlbIndex].RPN;
            return true;
        }
    }
    if (hCore->TLB.tlbSet3[tlbIndex].V)
    {
        // Check to see if its this entry!
        // Need to have a tag
        if (hCore->TLB.tlbSet3[tlbIndex].LP == LP &&
            hCore->TLB.tlbSet3[tlbIndex].p == p &&
            hCore->TLB.tlbSet3[tlbIndex].L)
        {
            // Probably It!
            *RPN = hCore->TLB.tlbSet3[tlbIndex].RPN;
            return true;
        }
    }
    return false;
}

// Main address translation mechanism used on the XCPU.
bool PPCInterpreter::MMUTranslateAddress(u64* EA, PPCState *hCoreState)
{
    // Every time the CPU does a load or store, it goes trough the MMU.
    // The MMU decides based on MSR, and some other regs if address translation
    // for Instr/Data is in Real Mode (EA = RA) or in Virtual Mode (Page 
    // Address Translation).

    // Xbox 360 MMU contains a very similar to the CELL-BE MMU.
    // Has two ERAT's (64 entry, 2 way), one for Instructions (I-ERAT) and Data
    // (D-ERAT), this caches Effective to Physical adress translations done
    // recently.
    // It also contains a 1024 entry 4 * 256 columns TLB array, wich caches
    // recent Page tables. TLB on the Xbox 360 can be Software/Hardware managed.
    // This is controlled via TL bit of the LPCR SPR.

    /* TODO */
    // Implement ERATS and ERAT handling code.
    // Implement L1 per-core data/inst cache and cache handling code.
    
    //
    // Current thread SPR's used in MMU..
    // 
    
    // Machine State Register.
    MSR _msr = hCoreState->MSR;
    // Logical Partition Control Register.
    u64 LPCR = hCoreState->SPR[SPR_LPCR];
    // Hypervisor Real Mode Offset Register.
    u64 HRMOR = hCoreState->SPR[SPR_HRMOR];
    // Real Mode Offset Register.
    u64 RMOR = hCoreState->SPR[SPR_RMOR];

    // Real Adress, this is what we want.
    u64 RA = 0;
    // Holds whether the cpu thread issuing the fetch is running in Real or
    // Virtual mode. It defaults to Real Mode, as this is how the XCPU starts
    // all threads.
    bool realMode = true;
    // If this EA bit is set, then address generated in Real Mode isn't OR'ed
    // with the contents of HRMOR register.
    bool eaZeroBit = ((*EA & 0x8000000000000000) >> 63);
    // LPCR(LPES) bit 1.
    bool lpcrLPESBit1 = ((LPCR & 0x8) >> 3);
    // Software management of the TLB. 0 = Hardware, 1 = Software.
    bool tlbSoftwareManaged = ((LPCR & 0x400) >> 10);


    // Instruction relocate and instruction fetch
    if (_msr.IR && hCoreState->iFetch)
        realMode = false;
    // Data fetch
    else if (_msr.DR)
        realMode = false;

    // Real Addressing Mode
    if (realMode)
    {
        // If running in Hypervisor Offset mode.
        if (_msr.HV)
        {
            if (eaZeroBit)
            {
                // Real adrees is bits 22-63 of Effective Address. 
                // RA = EA[22:63]
                RA = (*EA & 0x3FFFFFFFFFF);
            }
            else
            {
                // RA = (EA[22:43] | HRMOR[22:43]) || EA[44:63]
                RA = (((*EA & 0x3FFFFF00000) | (HRMOR & 0x3FFFFF00000)) 
                    | (*EA & 0xFFFFF));
            }
        }
        // Real Offset Mode
        else
        {
            // RA = (EA[22:43] | RMOR[22:43]) || EA[44:63]
            if (lpcrLPESBit1)
            {
                RA = (((*EA & 0x3FFFFF00000) | (RMOR & 0x3FFFFF00000))
                    | (*EA & 0xFFFFF));
            }
            else
            {
                // Mode Fault. LPAR Interrupt.
                std::cout << "XCPU (MMU): LPAR INTERRUPT!" << std::endl;
                system("PAUSE");
            }
        }
    }
    // Virtual Mode
    else
    {
        // Page size bits
        u8 p = 0;
        // Large pages
        bool L = 0;       
        // Large Page Selector (LP)
        u8 LP = 0;    
        // Efective Segment ID
        u64 ESID = QGET(*EA, 0, 35);
        ESID = ESID << 28;
        // Virtual Segment ID
        u64 VSID = 0;

        /*** Segmentation ***/
        // 64 bit EA -> 65 bit VA
        // ESID -> VSID

        bool slbHit = false;
        // Search the SLB to get the VSID
        for (auto& slbEntry : hCoreState->SLB)
        {
            if (slbEntry.V && (slbEntry.ESID == ESID))
            {
                // Entry valid & SLB->ESID = EA->VSID
                VSID = slbEntry.VSID;
                // VSID is VA 0-52 bit, the remaining 28 bits are adress data
                // so whe shift 28 bits left here.
                VSID = VSID << 28;
                L = slbEntry.L;
                LP = slbEntry.LP;
                slbHit = true;
                continue;
            }
        }

        // Virtual Page Number
        u64 VPN = 0;
        // Real Page Number
        u64 RPN = 0;
        // Page
        u32 Page = 0;
        // Byte offset
        u32 Byte = 0;

        // We hit the SLB, get the VA
        if(slbHit)
        {
            //
            // Virtual Addresss Generation
            // 
            
            // 1. Get the p Size
            p = mmuGetPageSize(hCoreState, L, LP);

            // Get our Virtual Address - 65 bit
            // VSID + 28 bit adress data
            u64 VA = (VSID | (*EA & 0xFFFFFFF));

            // Page 28 - p
            u32 Page = 0;
            Page = QGET(*EA, 36, 63 - p);
            Page = Page << p;

            // Vitual Page Number = VSID + Page 28-p
            VPN = VSID | Page;

            // Byte offset
            Byte = QGET(*EA, 64 - p, 63);

            if (mmuSearchTlbEntry(hCoreState, &RPN, VPN, p, LP))
            {
                // TLB Hit, proceed.
                goto end;
            }
            else
            {
                // TLB miss, if we are in software managed mode, generate an 
                // interrupt, else do page table search.
                if (tlbSoftwareManaged)
                {
                    hCoreState->exceptionOcurred = true;
                    // Page Table lookup TODO.
                    std::cout << "XCPU (MMU) TLB Miss in Softwarre Managed "
                        << "Mode. System Stopped." << std::endl;
                    system("PAUSE");
                }
                else
                {
                    // Page Table lookup TODO.
                    std::cout << "XCPU (MMU) TLB Miss in Hardware Managed "
                        << "Mode. System Stopped." << std::endl;
                    system("PAUSE");
                }
            }
        }
        else
        {
            // SLB Miss
            // Data or Inst Segment Exception
            std::cout << "XCPU (MMU): SLB not hit. System Stopped." 
                << std::endl;
            system("pause");
            return false;
        }

    end:
        RA = (RPN | QGET(*EA, 64 - p, 63));
        // Real Address 0 - 21 bits are not implemented;
        QSET(RA, 0, 21, 0);
    }    

    *EA = RA;
    return true;
}

// MMU Read Routine, used by the CPU
u64 PPCInterpreter::MMURead(XCPUContext* cpuContext, u64 EA, s8 byteCount)
{
    s8 currentCore = cpuContext->currentCoreID;
    u64 data = 0;

    // Exception ocurred?
    if (MMUTranslateAddress(&EA, &cpuContext->cpuCores[currentCore]) == false)
        return 0;

    // Check if there's a cache block with the current address
    if (EA >= 0x10000000000 && EA < 0x20000000000)
    {
        // Completly ignore cache, see what happens.
        EA = static_cast<u32>(EA);
        /*
        // Cached address
        for (auto& cacheBlock : intXCPUContext->l2Cache)
        {
            if (cacheBlock.V)
            {
                if (EA >= cacheBlock.address && EA < (cacheBlock.address + 0x80))
                {
                    u8 offset = static_cast<u8>(EA - cacheBlock.address);
                    memcpy(&data, &cacheBlock.data[offset], byteCount);
                    return data;
                }
            }
        }
        */
    }

    // TODO: Investigate this values FSB_CONFIG_RX_STATE - Needed to Work!
    if (cpuContext->cpuCores[currentCore].CIA == 0x0000000001003598)
    {
        cpuContext->cpuCores[currentCore].GPR[0xB] = 0xE;
    }

    if (cpuContext->cpuCores[currentCore].CIA == 0x0000000001003644)
    {
        cpuContext->cpuCores[currentCore].GPR[0xB] = 0x2;
    }

    // Hack Needed for CB to work, seems like it reads from some SoC this value
    // and checks againts the fuses.
    if (EA == 0x20000061000)
    {
        data = 0x0000000000000020;
        return data;
    }

    // Hack Needed for CB to work, reading ram size to this address, further 
    // research required. Free60.org shows this belongs to BIU address range.
    if (EA == 0x200e1040000)
    {
        data = 0x0000000020000000;
        return data;
    }

    // Hardware register address
    if (EA == 0x20000061008)
    {
        return data;
    }

    // SECENG address, CB compares this to 0.
    // Further research required!
    if (EA == 0x20000026000 || EA == 0x20000026008)
    {
        return data;
    }

    // Read is from SROM area:
    if (EA >= SROM_ADDR && EA <= SROM_ADDR + SROM_SIZE)
    {
        u32 sromAddr = static_cast<u32>(EA) - static_cast<u32>(SROM_ADDR);
        memcpy(&data, &cpuContext->SROM[sromAddr], byteCount);
        return data;
    }

    // Read is from SRAM area
    if (EA >= SRAM_ADDR && EA <= SRAM_ADDR + SRAM_SIZE)
    {
        u32 sramAddr = static_cast<u32>(EA) - static_cast<u32>(SRAM_ADDR);
        memcpy(&data, &cpuContext->SRAM[sramAddr], byteCount);
        return data;
    }

    // Check if reading from eFuses section
    if (EA >= FUSESET_LOC && EA <= (FUSESET_LOC + FUSESET_SIZE))
    {
        switch ((u32)EA)
        {
        case 0x20000:
            data = cpuContext->cpuCores[0].CPUFuses[0];
            break;
        case 0x20200:
            data = cpuContext->cpuCores[0].CPUFuses[1];
            break;
        case 0x20400:
            data = cpuContext->cpuCores[0].CPUFuses[2];
            break;
        case 0x20600:
            data = cpuContext->cpuCores[0].CPUFuses[3];
            break;
        case 0x20800:
            data = cpuContext->cpuCores[0].CPUFuses[4];
            break;
        case 0x20a00:
            data = cpuContext->cpuCores[0].CPUFuses[5];
            break;
        case 0x20c00:
            data = cpuContext->cpuCores[0].CPUFuses[6];
            break;
        case 0x20e00:
            data = cpuContext->cpuCores[0].CPUFuses[7];
            break;
        case 0x21000:
            data = cpuContext->cpuCores[0].CPUFuses[8];
            break;
        case 0x21200:
            data = cpuContext->cpuCores[0].CPUFuses[9];
            break;
        case 0x21400:
            data = cpuContext->cpuCores[0].CPUFuses[10];
            break;
        case 0x21600:
            data = cpuContext->cpuCores[0].CPUFuses[11];
            break;
        default:
            std::cout << "XCPU SECOTP(eFuse): Reading to FUSE at address 0x" 
                << EA << std::endl;
            break;

        }

        return _byteswap_uint64(data);
    }   

    else
    {
        // External Read
        cpuContext->bus->Read(EA, &data, byteCount);
    }

    return data;
}

// MMU Write Routine, used by the CPU
void PPCInterpreter::MMUWrite(XCPUContext* cpuContext, u64 data, u64 EA, 
    s8 byteCount, bool cacheStore)
{
    s8 currentCore = cpuContext->currentCoreID;

    if (MMUTranslateAddress(&EA, &cpuContext->cpuCores[currentCore]) == false)
        return;

    // Is Address Cached?
    if (EA >= 0x10000000000 && EA < 0x20000000000)
    {
        // Completly ignore cache, see what happens.
        EA = static_cast<u32>(EA);
        /*
        // Check if there's a cache block with the current address
        if (!cacheStore)
        {
            for (auto& cacheBlock : intXCPUContext->l2Cache)
            {
                if (cacheBlock.V)
                {
                    if (EA >= cacheBlock.address && EA < (cacheBlock.address + 0x80))
                    {
                        u8 offset = EA - cacheBlock.address;
                        memcpy(&cacheBlock.data[offset], &data, byteCount);
                        return;
                    }
                }
            }
        }
        */
    }

    // Check if writing to bootloader section
    if (EA >= SROM_ADDR && EA <= SROM_ADDR + SROM_SIZE)
    {
        std::cout << "XCPU (MMU): WARNING: Tried to write to XCPU SROM!" 
            << std::endl;
        return;
    }

    // Check if writing to internal SRAM
    if (EA >= SRAM_ADDR && EA <= SRAM_ADDR + SRAM_SIZE)
    {
        u32 sramAddr = (u32)(EA - SRAM_ADDR);
        memcpy(&cpuContext->SRAM[sramAddr], &data, byteCount);
        return;
    }
    else
    {
        // External Write
        cpuContext->bus->Write(EA, data, byteCount);
    }
}

// Reads 1 Byte of memory.
u8 PPCInterpreter::MMURead8(u64 EA)
{
    u8 data = 0;
    data = static_cast<u8>(MMURead(intXCPUContext, EA, 1));
    return data;
}
// Reads 2 Bytes of memory.
u16 PPCInterpreter::MMURead16(u64 EA)
{
    u16 data = 0;
    data = static_cast<u16>(MMURead(intXCPUContext, EA, 2));
    return _byteswap_ushort(data);
}
// Reads 4 Bytes of memory.
u32 PPCInterpreter::MMURead32(u64 EA)
{
    u32 data = 0;
    data = static_cast<u32>(MMURead(intXCPUContext, EA, 4));
    return _byteswap_ulong(data);
}
// Reads 8 Bytes of memory.
u64 PPCInterpreter::MMURead64(u64 EA)
{
    u64 data = 0;
    data = MMURead(intXCPUContext, EA, 8);
    return _byteswap_uint64(data);
}
// Writes 1 Byte to memory.
void PPCInterpreter::MMUWrite8(u64 EA, u8 data)
{
    MMUWrite(intXCPUContext, data, EA, 1);
}
// Writes 2 Bytes to memory.
void PPCInterpreter::MMUWrite16(u64 EA, u16 data)
{
    u16 dataBS = _byteswap_ushort(data);
    MMUWrite(intXCPUContext, dataBS, EA, 2);
}
// Writes 4 Bytes to memory.
void PPCInterpreter::MMUWrite32(u64 EA, u32 data)
{
    u32 dataBS = _byteswap_ulong(data);
    MMUWrite(intXCPUContext, dataBS, EA, 4);
}
// Writes 8 Bytes to memory.
void PPCInterpreter::MMUWrite64(u64 EA, u64 data)
{
    u64 dataBS = _byteswap_uint64(data);
    MMUWrite(intXCPUContext, dataBS, EA, 8);
}