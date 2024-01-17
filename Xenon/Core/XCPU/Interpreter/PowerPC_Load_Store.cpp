#include <iostream>

#include "Xenon/Core/XCPU/Interpreter/PPCInterpreter.h"

#define DBG_PRINT_LOAD(txt) //std::cout txt
#define DBG_PRINT_STORE(txt) //std::cout txt

void splitAddress(u64 data, u32 *addressHigh, u32* addressLow)
{
    *addressHigh = (data & 0xFFFFFFFF00000000) >> 32;
    *addressLow = (u32)data;
}

//
// Store Byte
//

void PPCInterpreter::PPCInterpreter_dcbst(PPCState* hCore)
{
    X_FORM_rA_rB;
    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    u64 RA = EA;
    u32 addrHigh, addrLow = 0;
    splitAddress(RA, &addrHigh, &addrLow);
    MMUTranslateAddress(&RA, hCore);
    u8* dataBlock = new(u8[128]);
    // Check if address has an existing cache block.
    for (auto& cacheBlock : intXCPUContext->l2Cache)
    {
        if (cacheBlock.V)
        {
            if (cacheBlock.address == RA)
            {
                memcpy(dataBlock, cacheBlock.data, 128);
                for (size_t offset = 0; offset < 128; offset += 8)
                {
                    u64 data = 0;
                    memcpy(&data, &dataBlock[offset], 8);
                    MMUWrite(intXCPUContext, data,EA + offset, 8, true);
                }
                break;
            }
        }
    }
    std::cout << " * dcbst * EA = 0x" << EA 
        << " RA 0x(" << addrHigh << ")" << addrLow << std::endl;
}

void PPCInterpreter::PPCInterpreter_dcbz(PPCState* hCore)
{
    X_FORM_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    EA = EA & ~(128 - 1); // Cache line size
    u64 RA = EA;
    MMUTranslateAddress(&RA, hCore);
    u32 addrHigh, addrLow = 0;
    splitAddress(RA, &addrHigh, &addrLow);
    std::cout << " * dcbz * EA = 0x" << EA 
        << " RA 0x(" << addrHigh << ") " << addrLow << std::endl;
    // As far as i can tell, XCPU does all the crypto, scrambling of
    // data on L2 cache, and DCBZ is used for creating cache blocks
    // and also erasing them.
    
    // Check if address has an existing cache block.
    for (auto& cacheBlock : intXCPUContext->l2Cache)
    {
        if (cacheBlock.V && cacheBlock.address == RA)
        {
            memset(cacheBlock.data, 0, 128);
            return;
        }
    }
    // No cache block exist for the target address.
    for (auto& cacheBlock : intXCPUContext->l2Cache)
    {
        if (cacheBlock.V == false)
        {
            // This is it.
            cacheBlock.V = true;
            cacheBlock.address = RA;
            memset(cacheBlock.data, 0, 128);
            return;
        }
    }
}

void PPCInterpreter::PPCInterpreter_stb(PPCState* hCore)
{
    D_FORM_rS_rA_D; 
    D = EXTS(D, 16);
    u64 EA = (rA ? hCore->GPR[rA] : 0) + D;

    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);

    DBG_PRINT_STORE(<< "stb (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u16)hCore->GPR[rS] << std::endl);

    MMUWrite8(EA, (u8)hCore->GPR[rS]);
}

void PPCInterpreter::PPCInterpreter_stbu(PPCState* hCore)
{
    D_FORM_rS_rA_D;
    D = EXTS(D, 16);

    u64 EA = hCore->GPR[rA] + D;

    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stbu (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u16)hCore->GPR[rS] << std::endl);
    
    MMUWrite8(EA, (u8)hCore->GPR[rS]);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_stbux(PPCState* hCore)
{
    X_FORM_rS_rA_rB;

    u64 EA = hCore->GPR[rA] + hCore->GPR[rB];

    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stbux (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u16)hCore->GPR[rS] << std::endl);
    
    MMUWrite8(EA, (u8)hCore->GPR[rS]);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_stbx(PPCState* hCore)
{
    X_FORM_rS_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stbx (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u8)hCore->GPR[rS] << std::endl);
    
    MMUWrite8(EA, (u8)hCore->GPR[rS]);
}

//
// Store Halfword
//

void PPCInterpreter::PPCInterpreter_sth(PPCState* hCore)
{
    D_FORM_rS_rA_D; 
    D = EXTS(D, 16);

    u64 EA = (rA ? hCore->GPR[rA] : 0) + D;
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "sth (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u16)hCore->GPR[rS] << std::endl);
    
    MMUWrite16(EA, (u16)hCore->GPR[rS]);
}

void PPCInterpreter::PPCInterpreter_sthbrx(PPCState* hCore)
{
    X_FORM_rS_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "sthbrx (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << _byteswap_ushort((u16)hCore->GPR[rS]) << std::endl);
    
    MMUWrite16(EA, _byteswap_ushort((u16)hCore->GPR[rS]));
}

void PPCInterpreter::PPCInterpreter_sthu(PPCState* hCore)
{
    D_FORM_rS_rA_D; 
    D = EXTS(D, 16);

    u64 EA = hCore->GPR[rA] + D;
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "sthu (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u16)hCore->GPR[rS] << std::endl);
    
    MMUWrite16(EA, (u16)hCore->GPR[rS]);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_sthux(PPCState* hCore)
{
    X_FORM_rS_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "sthux (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u16)hCore->GPR[rS] << std::endl);
    
    MMUWrite16(EA, (u16)hCore->GPR[rS]);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_sthx(PPCState* hCore)
{
    X_FORM_rS_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "sthx (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u16)hCore->GPR[rS] << std::endl);
    
    MMUWrite16(EA, (u16)hCore->GPR[rS]);
}

//
// Store Word
//

void PPCInterpreter::PPCInterpreter_stw(PPCState* hCore)
{
    D_FORM_rS_rA_D; 
    D = EXTS(D, 16);
    u64 EA = (rA ? hCore->GPR[rA] : 0) + D;
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stw (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u32)hCore->GPR[rS] << std::endl);
    
    MMUWrite32(EA, (u32)hCore->GPR[rS]);
}

void PPCInterpreter::PPCInterpreter_stwbrx(PPCState* hCore)
{
    X_FORM_rS_rA_rB;
    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);

    DBG_PRINT_STORE(<< "stwbrx (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u32)hCore->GPR[rS] << std::endl);
    
    MMUWrite32(EA, _byteswap_ulong(static_cast<u32>(hCore->GPR[rS])));
}

void PPCInterpreter::PPCInterpreter_stwu(PPCState* hCore)
{
    D_FORM_rS_rA_D;
    D = EXTS(D, 16);
    u64 EA = hCore->GPR[rA] + D;
       
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stwu (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u32)hCore->GPR[rS] << std::endl);
    
    MMUWrite32(EA, (u32)hCore->GPR[rS]);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_stwux(PPCState* hCore)
{
    X_FORM_rS_rA_rB;
    u64 EA = hCore->GPR[rA] + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stwux (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u32)hCore->GPR[rS] << std::endl);
   
    MMUWrite32(EA, (u32)hCore->GPR[rS]);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_stwx(PPCState* hCore)
{
    X_FORM_rD_rA_rB;
    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stwx (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << (u32)hCore->GPR[rD] << std::endl);
    
    
    MMUWrite32(EA, (u32)hCore->GPR[rD]);
}

//
// Store Doubleword
//

void PPCInterpreter::PPCInterpreter_std(PPCState* hCore)
{
    DS_FORM_rS_rA_DS; 
    DS = EXTS(DS, 14) << 2;
    u64 EA = (rA ? hCore->GPR[rA] : 0) + DS;
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "std (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << hCore->GPR[rS] << std::endl);
    
    MMUWrite64(EA, hCore->GPR[rS]);
}

void PPCInterpreter::PPCInterpreter_stdcx(PPCState* hCore)
{
}

void PPCInterpreter::PPCInterpreter_stdu(PPCState* hCore)
{
    DS_FORM_rD_rA_DS; 
    DS = EXTS(DS, 14) << 2;
    u64 EA = hCore->GPR[rA] + DS;
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stdu (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << hCore->GPR[rD] << std::endl);
    
    
    MMUWrite64(EA, hCore->GPR[rD]);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_stdux(PPCState* hCore)
{
    X_FORM_rS_rA_rB;
    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stdux (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << hCore->GPR[rS] << std::endl);
    
    MMUWrite64(EA, hCore->GPR[rS]);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_stdx(PPCState* hCore)
{
    X_FORM_rS_rA_rB;
    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    
    u32 addrHigh, addrLow = 0;
    splitAddress(EA, &addrHigh, &addrLow);
    DBG_PRINT_STORE(<< "stdx (0x" << std::hex << hCore->CIA << "): Addr = 0x("
        << addrHigh << ") " << addrLow << " data =  0x" << hCore->GPR[rS] << std::endl);
    
    MMUWrite64(EA, hCore->GPR[rS]);
}

//
// Load Byte
//

void PPCInterpreter::PPCInterpreter_lbz(PPCState* hCore)
{
    D_FORM_rD_rA_D;
    D = EXTS(D, 16);

    u64 EA = (rA ? hCore->GPR[rA] : 0) + D;
    hCore->GPR[rD] = MMURead8(EA);
    DBG_PRINT_LOAD(<< "lbz: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
}

void PPCInterpreter::PPCInterpreter_lbzu(PPCState* hCore)
{
    D_FORM_rD_rA_D;
    D = EXTS(D, 16);

    u64 EA = hCore->GPR[rA] + D;
    hCore->GPR[rD] = MMURead8(EA);
    hCore->GPR[rA] = EA;
    DBG_PRINT_LOAD(<< "lbzu: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
}

void PPCInterpreter::PPCInterpreter_lbzux(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = hCore->GPR[rA] + hCore->GPR[rB];

    hCore->GPR[rD] = MMURead8(EA);
    hCore->GPR[rA] = EA;
    DBG_PRINT_LOAD(<< "lbzux: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
}

void PPCInterpreter::PPCInterpreter_lbzx(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    hCore->GPR[rD] = MMURead8(EA);
    DBG_PRINT_LOAD(<< "lbzx: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
}

//
// Load Halfword
//

void PPCInterpreter::PPCInterpreter_lhbrx(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];

    u16 data = MMURead16(EA);
    hCore->GPR[rD] = _byteswap_ushort(data);
    DBG_PRINT_LOAD(<< "lhbrx: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
}

void PPCInterpreter::PPCInterpreter_lhz(PPCState* hCore)
{
    D_FORM_rD_rA_D; 
    D = EXTS(D, 16);

    u64 EA = (rA ? hCore->GPR[rA] : 0) + D;
    hCore->GPR[rD] = MMURead16(EA);
    DBG_PRINT_LOAD(<< "lhz: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
}

void PPCInterpreter::PPCInterpreter_lhzu(PPCState* hCore)
{
    D_FORM_rD_rA_D;
    D = EXTS(D, 16);

    u64 EA = hCore->GPR[rA] + D;
    hCore->GPR[rD] = MMURead16(EA);
    DBG_PRINT_LOAD(<< "lhzu: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_lhzux(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = hCore->GPR[rA] + hCore->GPR[rB];

    hCore->GPR[rD] = MMURead16(EA);
    DBG_PRINT_LOAD(<< "lhzux: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_lhzx(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];

    hCore->GPR[rD] = MMURead16(EA);
    DBG_PRINT_LOAD(<< "lhzx: Addr = 0x" << std::hex << EA << " data =  0x" << (u16)hCore->GPR[rD] << std::endl);
}

//
// Load Word
// 

void PPCInterpreter::PPCInterpreter_lwbrx(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    u32 data = MMURead32(EA);
    hCore->GPR[rD] = _byteswap_ulong(data);
    DBG_PRINT_LOAD(<< "lwbrx: Addr = 0x" << std::hex << EA << " data =  0x" << (u32)hCore->GPR[rD] << std::endl);
}

void PPCInterpreter::PPCInterpreter_lwz(PPCState* hCore)
{
    D_FORM_rD_rA_D; 
    D = EXTS(D, 16);

    u64 EA = (rA ? hCore->GPR[rA] : 0) + D;
    hCore->GPR[rD] = MMURead32(EA);
    DBG_PRINT_LOAD(<< "lwz(0x" << std::hex << hCore->CIA << "): Addr = 0x" << EA << " data =  0x" << (u32)hCore->GPR[rD] << std::endl);
}

void PPCInterpreter::PPCInterpreter_lwzu(PPCState* hCore)
{
    D_FORM_rD_rA_D;
    D = EXTS(D, 16);

    u64 EA = hCore->GPR[rA] + D;
    hCore->GPR[rD] = MMURead32(EA);
    DBG_PRINT_LOAD(<< "lwzu: Addr = 0x" << std::hex << EA << " data =  0x" << (u32)hCore->GPR[rD] << std::endl);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_lwzux(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = hCore->GPR[rA] + hCore->GPR[rB];

    hCore->GPR[rD] = MMURead32(EA);
    DBG_PRINT_LOAD(<< "lwzux: Addr = 0x" << std::hex << EA << " data =  0x" << (u32)hCore->GPR[rD] << std::endl);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_lwzx(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];
    hCore->GPR[rD] = MMURead32(EA);
    DBG_PRINT_LOAD(<< "lwzx: Addr = 0x" << std::hex << EA << " data =  0x" << (u32)hCore->GPR[rD] << std::endl);
}

//
// Load Doubleword
//

void PPCInterpreter::PPCInterpreter_ld(PPCState* hCore)
{
    DS_FORM_rD_rA_DS; 
    DS = EXTS(DS, 14) << 2;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + DS;
    hCore->GPR[rD] = MMURead64(EA);
    DBG_PRINT_LOAD(<< "ld: Addr = 0x" << std::hex << EA << " data =  0x" << (u64)hCore->GPR[rD] << std::endl);
}

void PPCInterpreter::PPCInterpreter_ldarx(PPCState* hCore)
{
}

void PPCInterpreter::PPCInterpreter_ldbrx(PPCState* hCore)
{

}

void PPCInterpreter::PPCInterpreter_ldu(PPCState* hCore)
{
    DS_FORM_rD_rA_DS;
    DS = EXTS(DS, 14) << 2;

    u64 EA = hCore->GPR[rA] + DS;
    hCore->GPR[rD] = MMURead64(EA);
    DBG_PRINT_LOAD(<< "ldu: Addr = 0x" << std::hex << EA << " data =  0x" << (u64)hCore->GPR[rD] << std::endl);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_ldux(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = hCore->GPR[rA] + hCore->GPR[rB];

    hCore->GPR[rD] = MMURead64(EA);
    DBG_PRINT_LOAD(<< "ldux: Addr = 0x" << std::hex << EA << " data =  0x" << (u64)hCore->GPR[rD] << std::endl);
    hCore->GPR[rA] = EA;
}

void PPCInterpreter::PPCInterpreter_ldx(PPCState* hCore)
{
    X_FORM_rD_rA_rB;

    u64 EA = (rA ? hCore->GPR[rA] : 0) + hCore->GPR[rB];

    hCore->GPR[rD] = MMURead64(EA);
    DBG_PRINT_LOAD(<< "ldx: Addr = 0x" << std::hex << EA << " data =  0x" << (u64)hCore->GPR[rD] << std::endl);
}
