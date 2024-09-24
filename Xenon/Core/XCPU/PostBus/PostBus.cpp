#include <iostream>

#include "PostBus.h"

// Text colored output (for now)
HANDLE hConsole;

void Xe::XCPU::POSTBUS::POST(u64 postCode)
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 3);
    std::cout << "POST: ";
    SetConsoleTextAttribute(hConsole, 2);

    /* 1BL */
    if (postCode >= 0x10 && postCode <= 0x1e)
    {
        std::cout << "1BL > ";
        SetConsoleTextAttribute(hConsole, 15);

        switch (postCode)
        {
        case 0x10:
            std::cout << "1BL started" << std::endl;
            break;
        case 0x11:
            std::cout << "FSB_CONFIG_PHY_CONTROL - Execute FSB function1" << std::endl;
            break;
        case 0x12:
            std::cout << "FSB_CONFIG_RX_STATE - Execute FSB function2" << std::endl;
            break;
        case 0x13:
            std::cout << "FSB_CONFIG_TX_STATE - Execute FSB function3" << std::endl;
            break;
        case 0x14:
            std::cout << "FSB_CONFIG_TX_CREDITS - Execute FSB function4" << std::endl;
            break;
        case 0x15:
            std::cout << "FETCH_OFFSET - Verify CB offset" << std::endl;
            break;
        case 0x16:
            std::cout << "FETCH_HEADER - Copy CB header from NAND" << std::endl;
            break;
        case 0x17:
            std::cout << "VERIFY_HEADER - Verify CB header" << std::endl;
            break;
        case 0x18:
            std::cout << "FETCH_CONTENTS - Copy CB into protected SRAM" << std::endl;
            break;
        case 0x19:
            std::cout << "HMACSHA_COMPUTE - Generate CB HMAC key" << std::endl;
            break;
        case 0x1a:
            std::cout << "RC4_INITIALIZE - Initialize CB RC4 decryption key" << std::endl;
            break;
        case 0x1b:
            std::cout << "RC4_DECRYPT - RC4 decrypt CB" << std::endl;
            break;
        case 0x1c:
            std::cout << "SHA_COMPUTE - Generate hash of CB for verification" << std::endl;
            break;
        case 0x1d:
            std::cout << "SIG_VERIFY - RSA signature check of CB hash" << std::endl;
            break;
        case 0x1e:
            std::cout << "BRANCH - Jump to CB" << std::endl;
            break;
        }
    }
    /* 1BL PANICS */
    else if (postCode >= 0x81 && postCode <= 0x98)
    {
        std::cout << "1BL > ";
        SetConsoleTextAttribute(hConsole, 4);
        switch (postCode)
        {
            /* 1BL PANIC*/
        case 0x81:
            std::cout << "PANIC - MACHINE_CHECK" << std::endl;
            system("PAUSE");
            break;
        case 0x82:
            std::cout << "PANIC - DATA_STORAGE" << std::endl;
            system("PAUSE");
            break;
        case 0x83:
            std::cout << "PANIC - DATA_SEGMENT" << std::endl;
            system("PAUSE");
            break;
        case 0x84:
            std::cout << "PANIC - INSTRUCTION_STORAGE" << std::endl;
            system("PAUSE");
            break;
        case 0x85:
            std::cout << "POST: 1BL > PANIC - INSTRUCTION_SEGMENT" << std::endl;
            system("PAUSE");
            break;
        case 0x86:
            std::cout << "POST: 1BL > PANIC - EXTERNAL" << std::endl;
            system("PAUSE");
            break;
        case 0x87:
            std::cout << "POST: 1BL > PANIC - ALIGNMENT" << std::endl;
            system("PAUSE");
            break;
        case 0x88:
            std::cout << "POST: 1BL > PANIC - PROGRAM" << std::endl;
            system("PAUSE");
            break;
        case 0x89:
            std::cout << "POST: 1BL > PANIC - FPU_UNAVAILABLE" << std::endl;
            system("PAUSE");
            break;
        case 0x8a:
            std::cout << "POST: 1BL > PANIC - DECREMENTER" << std::endl;
            system("PAUSE");
            break;
        case 0x8b:
            std::cout << "POST: 1BL > PANIC - HYPERVISOR_DECREMENTER" << std::endl;
            system("PAUSE");
            break;
        case 0x8c:
            std::cout << "POST: 1BL > PANIC - SYSTEM_CALL" << std::endl;
            system("PAUSE");
            break;
        case 0x8d:
            std::cout << "POST: 1BL > PANIC - TRACE" << std::endl;
            system("PAUSE");
            break;
        case 0x8e:
            std::cout << "PANIC - VPU_UNAVAILABLE" << std::endl;
            system("PAUSE");
            break;
        case 0x8f:
            std::cout << "PANIC - MAINTENANCE" << std::endl;
            system("PAUSE");
            break;
        case 0x90:
            std::cout << "PANIC - VMX_ASSIST" << std::endl;
            system("PAUSE");
            break;
        case 0x91:
            std::cout << "PANIC - THERMAL_MANAGEMENT" << std::endl;
            system("PAUSE");
            break;
        case 0x92:
            std::cout << "PANIC - 1BL is executed on wrong CPU thread (panic)" << std::endl;
            system("PAUSE");
            break;
        case 0x93:
            std::cout << "PANIC - TOO_MANY_CORES - 1BL is executed on wrong CPU core (panic)" << std::endl;
            system("PAUSE");
            break;
        case 0x94:
            std::cout << "PANIC - VERIFY_OFFSET - CB offset verification failed" << std::endl;
            system("PAUSE");
            break;
        case 0x95:
            std::cout << "PANIC - VERIFY_HEADER - CB header verification failed" << std::endl;
            system("PAUSE");
            break;
        case 0x96:
            std::cout << "PANIC - SIG_VERIFY - CB RSA signature verification failed" << std::endl;
            system("PAUSE");
            break;
        case 0x97:
            std::cout << "PANIC - NONHOST_RESUME_STATUS" << std::endl;
            system("PAUSE");
            break;
        case 0x98:
            std::cout << "PANIC - NEXT_STAGE_SIZE - Size of next stage is out-of-bounds" << std::endl;
            system("PAUSE");
            break;
        }
    }
    /* CB */
    else if (postCode >= 0x20 && postCode <= 0x3b)
    {

        std::cout << "CB > ";
        SetConsoleTextAttribute(hConsole, 15);
        switch (postCode)
        {
        case 0x20:
            std::cout << "CB entry point. initialize SoC" << std::endl;
            break;
        case 0x21:
            std::cout << "INIT_SECOTP - Initialize secotp, verify lockdown fuses" << std::endl;
            break;
        case 0x22:
            std::cout << "INIT_SECENG - Iitialize security engine" << std::endl;
            break;
        case 0x23:
            std::cout << "INIT_SYSRAM - Initialize EDRAM" << std::endl;
            break;
        case 0x24:
            std::cout << "VERIFY_OFFSET_3BL_CC" << std::endl;
            break;
        case 0x25:
            std::cout << "LOCATE_3BL_CC" << std::endl;
            break;
        case 0x26:
            std::cout << "FETCH_HEADER_3BL_CC" << std::endl;
            break;
        case 0x27:
            std::cout << "VERIFY_HEADER_3BL_CC" << std::endl;
            break;
        case 0x28:
            std::cout << "FETCH_CONTENTS_3BL_CC" << std::endl;
            break;
        case 0x29:
            std::cout << "HMACSHA_COMPUTE_3BL_CC" << std::endl;
            break;
        case 0x2a:
            std::cout << "RC4_INITIALIZE_3BL_CC" << std::endl;
            break;
        case 0x2b:
            std::cout << "RC4_DECRYPT_3BL_CC" << std::endl;
            break;
        case 0x2c:
            std::cout << "SHA_COMPUTE_3BL_CC" << std::endl;
            break;
        case 0x2d:
            std::cout << "SIG_VERIFY_3BL_CC" << std::endl;
            break;
        case 0x2e:
            std::cout << "HWINIT - Hardware initialization" << std::endl;
            break;
        case 0x2f:
            std::cout << "RELOCATE - Setup TLB entries, relocate to RAM" << std::endl;
            break;
        case 0x30:
            std::cout << "VERIFY_OFFSET_4BL_CD - Verify CD offset" << std::endl;
            break;
        case 0x31:
            std::cout << "FETCH_HEADER_4BL_CD - Verify CD header" << std::endl;
            break;
        case 0x32:
            std::cout << "VERIFY_HEADER_4BL_CD - Verify CD header" << std::endl;
            break;
        case 0x33:
            std::cout << "FETCH_CONTENTS_4BL_CD - Copy CD from NAND" << std::endl;
            break;
        case 0x34:
            std::cout << "HMACSHA_COMPUTE_4BL_CD - Create HMAC key for CD decryption" << std::endl;
            break;
        case 0x35:
            std::cout << "RC4_INITIALIZE_4BL_CD - Initialize CD RC4 key using HMAC key" << std::endl;
            break;
        case 0x36:
            std::cout << "RC4_DECRYPT_4BL_CD - RC4 decrypt CD with key" << std::endl;
            break;
        case 0x37:
            std::cout << "SHA_COMPUTE_4BL_CD - Compute hash of CD for verification" << std::endl;
            break;
        case 0x38:
            std::cout << "SIG_VERIFY_4BL_CD - RSA signature check of CD hash" << std::endl;
            break;
        case 0x39:
            std::cout << "SHA_VERIFY_4BL_CD - MemCmp cumputed hash with expected one" << std::endl;
            break;
        case 0x3a:
            std::cout << "BRANCH - Setup memory encryption and jump to CD" << std::endl;
            break;
        case 0x3b:
            std::cout << "PCI_INIT - Initialize PCI" << std::endl;
            break;
        }
    }
    /* CB PANICS */
    else if (postCode >= 0x9B && postCode <= 0xB0)
    {
        std::cout << "CB > ";
        SetConsoleTextAttribute(hConsole, 4);
        switch (postCode)
        {
        case 0x9b:
            std::cout << "PANIC - VERIFY_SECOTP_1 - Secopt fuse verification fail" << std::endl;
            system("PAUSE");
            break;
        case 0x9c:
            std::cout << "PANIC - VERIFY_SECOTP_2 - Secopt fuse verification fail2" << std::endl;
            system("PAUSE");
            break;
        case 0x9d:
            std::cout << "PANIC - VERIFY_SECOTP_3 - Secopt fuse verification console type? fail" << std::endl;
            system("PAUSE");
            break;
        case 0x9e:
            std::cout << "PANIC - VERIFY_SECOTP_4 - Secopt fuse verification console type? fail" << std::endl;
            system("PAUSE");
            break;
        case 0x9f:
            std::cout << "PANIC - VERIFY_SECOTP_5 - Secopt fuse verification console type? fail" << std::endl;
            system("PAUSE");
            break;
        case 0xa0:
            std::cout << "PANIC - VERIFY_SECOTP_6 - CB revocation check failed" << std::endl;
            system("PAUSE");
            break;
        case 0xa1:
            std::cout << "PANIC - VERIFY_SECOTP_7 - Panic after 0x21" << std::endl;
            system("PAUSE");
            break;
        case 0xa2:
            std::cout << "PANIC - VERIFY_SECOTP_8 - Panic after 0x21" << std::endl;
            system("PAUSE");
            break;
        case 0xa3:
            std::cout << "PANIC - VERIFY_SECOTP_9 - Panic after 0x21" << std::endl;
            system("PAUSE");
            break;
        case 0xa4:
            std::cout << "PANIC - VERIFY_SECOTP_10 - Failed SMC HMAC" << std::endl;
            system("PAUSE");
            break;
        case 0xa5:
            std::cout << "PANIC - VERIFY_OFFSET_3BL_CC" << std::endl;
            system("PAUSE");
            break;
        case 0xa6:
            std::cout << "PANIC - LOCATE_3BL_CC" << std::endl;
            system("PAUSE");
            break;
        case 0xa7:
            std::cout << "PANIC - VERIFY_HEADER_3BL_CC" << std::endl;
            system("PAUSE");
            break;
        case 0xa8:
            std::cout << "PANIC - SIG_VERIFY_3BL_CC" << std::endl;
            system("PAUSE");
            break;
        case 0xa9:
            std::cout << "PANIC - HWINIT - HArdware Initialization failed" << std::endl;
            system("PAUSE");
            break;
        case 0xaa:
            std::cout << "PANIC - VERIFY_OFFSET_4BL_CC" << std::endl;
            system("PAUSE");
            break;
        case 0xab:
            std::cout << "PANIC - VERIFY_HEADER_4BL_CC" << std::endl;
            system("PAUSE");
            break;
        case 0xac:
            std::cout << "PANIC - SIG_VERIFY_4BL_CC" << std::endl;
            system("PAUSE");
            break;
        case 0xad:
            std::cout << "PANIC - SHA_VERIFY_4BL_CC" << std::endl;
            system("PAUSE");
            break;
        case 0xae:
            std::cout << "PANIC - UNEXPECTED_INTERRUPT" << std::endl;
            system("PAUSE");
            break;
        case 0xaf:
            std::cout << "PANIC - UNSUPPORTED_RAM_SIZE" << std::endl;
            system("PAUSE");
            break;
        default:
            std::cout << "Unrecognized PANIC code 0x" << postCode << std::endl;
            break;
        }
    }
    /* CB_A */
    else if (postCode >= 0xD0 && postCode <= 0xDB)
    {
        std::cout << "CB_A > ";
        SetConsoleTextAttribute(hConsole, 15);

        switch (postCode)
        {
        case 0xD0:
            std::cout << "CB_A_ENTRY - CB_A entry point, copy self to 0x8000.0200.0001.C000 and continue from there." << std::endl;
            break;
        case 0xD1:
            std::cout << "READ_FUSES - Copy fuses from SoC for CB_B decryption." << std::endl;
            break;
        case 0xD2:
            std::cout << "VERIFY_OFFSET_CB_B - Verify CB_B offset." << std::endl;
            break;
        case 0xD3:
            std::cout << "FETCH_HEADER_CB_B - Copy CB_B header from NAND for verification." << std::endl;
            break;
        case 0xD4:
            std::cout << "VERIFY_HEADER_CB_B - Verify CB_B header." << std::endl;
            break;
        case 0xD5:
            std::cout << "FETCH_CONTENTS_CB_B - Copy CBB into memory at 0x8000.0200.0001.0000 (old location of CB_A)." << std::endl;
            break;
        case 0xD6:
            std::cout << "HMACSHA_COMPUTE_CB_B - Create HMAC key for CD decryption." << std::endl;
            break;
        case 0xD7:
            std::cout << "RC4_INITIALIZE_CB_B - Initialize CD RC4 key using HMAC key." << std::endl;
            break;
        case 0xD8:
            std::cout << "RC4_DECRYPT_CB_B - RC4 decrypt CD." << std::endl;
            break;
        case 0xD9:
            std::cout << "SHA_COMPUTE_CB_B - Compute hash of CD for verification." << std::endl;
            break;
        case 0xDa:
            std::cout << "SHA_VERIFY_CB_B - MemCmp computed hash with expected one (where rgh2 glitches)." << std::endl;
            break;
        case 0xDb:
            std::cout << "BRANCH_CB_B - Verify CB_B offset." << std::endl;
            break;
        }
    }
    /* CB_A PANICS */
    else if (postCode >= 0xF0 && postCode <= 0xF3)
    {
    std::cout << "CB_A > ";
    SetConsoleTextAttribute(hConsole, 4);
    switch (postCode)
    {
    case 0xF0:
        std::cout << "PANIC - VERIFY_OFFSET_CB_B - CB_B offset verification fail." << std::endl;
        system("PAUSE");
        break;
    case 0xF1:
        std::cout << "PANIC - VERIFY_HEADER_CB_B - CB_B header verification fail" << std::endl;
        system("PAUSE");
        break;
    case 0xF2:
        std::cout << "PANIC - SHA_VERIFY_CB_B - CB_B security hash comparison fail." << std::endl;
        system("PAUSE");
        break;
    case 0xF3:
        std::cout << "PANIC - ENTRY_SIZE_INVALID_CB_B - CB_B size check fail (must be less than 0xC000)." << std::endl;
        system("PAUSE");
        break;
    }
    }
    /* CD */
    else if (postCode >= 0x40 && postCode <= 0x53)
    {
        std::cout << "CD > ";
        SetConsoleTextAttribute(hConsole, 15);

        switch (postCode)
        {
        case 0x40:
            std::cout << "Entrypoint of CD, setup memory paging." << std::endl;
            break;
        case 0x41:
            std::cout << "VERIFY_OFFSET - Verify offset to CE" << std::endl;
            break;
        case 0x42:
            std::cout << "FETCH_HEADER - Copy CE header from NAND for verification" << std::endl;
            break;
        case 0x43:
            std::cout << "VERIFY_HEADER - Verify CE header" << std::endl;
            break;
        case 0x44:
            std::cout << "FETCH_CONTENTS - Read CE from NAND into memory" << std::endl;
            break;
        case 0x45:
            std::cout << "HMACSHA_COMPUTE - Create HMAC key for CE decryption" << std::endl;
            break;
        case 0x46:
            std::cout << "RC4_INITIALIZE - Initialize CE RC4 key using HMAC key" << std::endl;
            break;
        case 0x47:
            std::cout << "RC4_DECRYPT - RC4 decrypt CE" << std::endl;
            break;
        case 0x48:
            std::cout << "SHA_COMPUTE - Compute hash of CE for verification" << std::endl;
            break;
        case 0x49:
            std::cout << "SHA_VERIFY - MemCmp computed hash with expected one. (RGH1 Glitches here)" << std::endl;
            break;
        case 0x4a:
            std::cout << "LOAD_6BL_CF" << std::endl;
            break;
        case 0x4b:
            std::cout << "LZX_EXPAND - LZX Decompress CE" << std::endl;
            break;
        case 0x4c:
            std::cout << "SWEEP_CACHES" << std::endl;
            break;
        case 0x4d:
            std::cout << "DECODE_FUSES" << std::endl;
            break;
        case 0x4e:
            std::cout << "FETCH_OFFSET_6BL_CF - Load CD (kernel patches) offset" << std::endl;
            break;
        case 0x4f:
            std::cout << "VERIFY_OFFSET_6BL_CF - Verify CF offset" << std::endl;
            break;
        case 0x50:
            std::cout << "LOAD_UPDATE_1 - Load CF1/CG1 (patch slot 1) if version & header check pass" << std::endl;
            break;
        case 0x51:
            std::cout << "LOAD_UPDATE_2 - Load CF2/CG2 (patch slot 2) if version & header check pass" << std::endl;
            break;
        case 0x52:
            std::cout << "BRANCH - Startup kernel/hypervisor" << std::endl;
            break;
        case 0x53:
            std::cout << "DECRYPT_VERIFY_HV_CERT - Decrypt and verify hypervisor certificate" << std::endl;
            break;
        }
    }
    /* CD PANICS */
    else if (postCode >= 0xB1 && postCode <= 0xB8)
    {
        std::cout << "CD > ";
        SetConsoleTextAttribute(hConsole, 4);
        switch (postCode)
        {
        case 0xB1:
            std::cout << "PANIC - VERIFY_OFFSET - CE decryption failed" << std::endl;
            system("PAUSE");
            break;
        case 0xB2:
            std::cout << "PANIC - VERIFY_HEADER - Failed to verify CE header" << std::endl;
            system("PAUSE");
            break;
        case 0xB3:
            std::cout << "PANIC - SHA_VERIFY - CE hash comparison fail" << std::endl;
            system("PAUSE");
            break;
        case 0xB4:
            std::cout << "PANIC - LZX_EXPAND - CE LZX decompression failed" << std::endl;
            system("PAUSE");
            break;
        case 0xB5:
            std::cout << "PANIC - VERIFY_OFFSET_6BL - CF verification failed" << std::endl;
            system("PAUSE");
            break;
        case 0xB6:
            std::cout << "PANIC - DECODE_FUSES - Fuse decryption/check failed" << std::endl;
            system("PAUSE");
            break;
        case 0xB7:
            std::cout << "PANIC - UPDATE_MISSING - CF decryption failed, patches missing" << std::endl;
            system("PAUSE");
            break;
        case 0xB8:
            std::cout << "PANIC - CF_HASH_AUTH - CF hash auth failed" << std::endl;
            system("PAUSE");
            break;
        }
    }
    /* CE/CF PANICS */
    else if (postCode >= 0xC1 && postCode <= 0xC8)
    {
        std::cout << "CE/F > ";
        SetConsoleTextAttribute(hConsole, 4);
        switch (postCode)
        {
        case 0xC1:
            std::cout << "PANIC - LZX_EXPAND_1 - 	Panic - LDICreateDecompression" << std::endl;
            system("PAUSE");
            break;
        case 0xC2:
            std::cout << "PANIC - LZX_EXPAND_2 - 7BL Size Verification" << std::endl;
            system("PAUSE");
            break;
        case 0xC3:
            std::cout << "PANIC - LZX_EXPAND_3 - Header/Patch Fragment Info" << std::endl;
            system("PAUSE");
            break;
        case 0xC4:
            std::cout << "PANIC - LZX_EXPAND_4 - Unexpected LDI Fragment" << std::endl;
            system("PAUSE");
            break;
        case 0xC5:
            std::cout << "PANIC - LZX_EXPAND_5 - LDISetWindowData" << std::endl;
            system("PAUSE");
            break;
        case 0xC6:
            std::cout << "PANIC - LZX_EXPAND_6 - LDIDecompress" << std::endl;
            system("PAUSE");
            break;
        case 0xC7:
            std::cout << "PANIC - LZX_EXPAND_7 - LDIResetDecompression" << std::endl;
            system("PAUSE");
            break;
        case 0xC8:
            std::cout << "PANIC - SHA_VERIFY - 7BL Signature Verify" << std::endl;
            system("PAUSE");
            break;
        }
    }
    /* HYPERVISOR */
    else if (postCode >= 0x58 && postCode <= 0x5E)
    {
        std::cout << "HV > ";
        SetConsoleTextAttribute(hConsole, 15);

        switch (postCode)
        {
        case 0x58:
            std::cout << "INIT_HYPERVISOR - Hypervisor Initialization begin" << std::endl;
            break;
        case 0x59:
            std::cout << "INIT_SOC_MMIO - Initialize SoC MMIO" << std::endl;
            break;
        case 0x5a:
            std::cout << "INIT_XEX_TRAINING - Initialize XEX training" << std::endl;
            break;
        case 0x5b:
            std::cout << "INIT_KEYRING - Initialize key ring" << std::endl;
            break;
        case 0x5c:
            std::cout << "INIT_KEYS - Initialize keys" << std::endl;
            break;
        case 0x5d:
            std::cout << "INIT_SOC_INT - Initialize SoC Interrupts" << std::endl;
            break;
        case 0x5e:
            std::cout << "INIT_SOC_INT_COMPLETE - Initialization complete" << std::endl;
            break;
        }
    }
    /* HYPERVISOR PANICS */
    else if (postCode == 0xFF)
    {
        std::cout << "HV > ";
        SetConsoleTextAttribute(hConsole, 4);
        std::cout << "PANIC - FATAL!" << std::endl;
    }
    /* KERNEL */
    else if (postCode >= 0x60 && postCode <= 0x79)
    {
        std::cout << "KENREL > ";
        SetConsoleTextAttribute(hConsole, 15);

        switch (postCode)
        {
        case 0x60:
            std::cout << "INIT_KERNEL - Initialize kernel" << std::endl;
            break;
        case 0x61:
            std::cout << "INITIAL_HAL_PHASE_0 - Initialize HAL phase 0" << std::endl;
            break;
        case 0x62:
            std::cout << "INIT_PROCESS_OBJECTS - Initialize process objects" << std::endl;
            break;
        case 0x63:
            std::cout << "INIT_KERNEL_DEBUGGER - Initialize kernel debugger" << std::endl;
            break;
        case 0x64:
            std::cout << "INIT_MEMORY_MANAGER - Initialize memory manager" << std::endl;
            break;
        case 0x65:
            std::cout << "INIT_STACKS - Initialize stacks" << std::endl;
            break;
        case 0x66:
            std::cout << "INIT_OBJECT_SYSTEM - Initialize object system" << std::endl;
            break;
        case 0x67:
            std::cout << "INIT_PHASE1_THREAD - Initialize phase 1 thread" << std::endl;
            break;
        case 0x68:
            std::cout << "INIT_PROCESSORS - Initialize processors" << std::endl;
            break;
        case 0x69:
            std::cout << "INIT_KEYVAULT - Initialize keyvault" << std::endl;
            break;
        case 0x6A:
            std::cout << "INIT_HAL_PHASE_1 - Initialize HAL phase 1" << std::endl;
            break;
        case 0x6B:
            std::cout << "INIT_SFC_DRIVER - Initialize flash controller" << std::endl;
            break;
        case 0x6C:
            std::cout << "INIT_SECURITY - Initialize security" << std::endl;
            break;
        case 0x6D:
            std::cout << "INIT_KEY_EX_VAULT - Initialize extended keyvault" << std::endl;
            break;
        case 0x6E:
            std::cout << "INIT_SETTINGS - Initialize settings" << std::endl;
            break;
        case 0x6F:
            std::cout << "INIT_POWER_MODE - Initialize power mode" << std::endl;
            break;
        case 0x70:
            std::cout << "INIT_VIDEO_DRIVER - Initialize video driver" << std::endl;
            break;
        case 0x71:
            std::cout << "INIT_AUDIO_DRIVER - Initialize audio driver" << std::endl;
            break;
        case 0x72:
            std::cout << "INIT_BOOT_ANIMATION - Initialize bootanim.xex, XMADecoder, XAudioRender" << std::endl;
            break;
        case 0x73:
            std::cout << "INIT_SATA_DRIVER - Initialize SATA driver" << std::endl;
            break;
        case 0x74:
            std::cout << "INIT_SHADOWBOOT - Initialize shadowboot" << std::endl;
            break;
        case 0x75:
            std::cout << "INIT_DUMP_SYSTEM - Initialize dump system" << std::endl;
            break;
        case 0x76:
            std::cout << "INIT_SYSTEM_ROOT - Initialize system root" << std::endl;
            break;
        case 0x77:
            std::cout << "INIT_OTHER_DRIVERS - Initialize other drivers" << std::endl;
            break;
        case 0x78:
            std::cout << "INIT_STFS_DRIVER - Initialize STFS driver" << std::endl;
            break;
        case 0x79:
            std::cout << "LOAD_XAM - Initialize xam.xex" << std::endl;
            break;
        }
        }
    else
    {
        SetConsoleTextAttribute(hConsole, 15);
        std::cout << "POST: Unrecognized post code: " << std::hex << postCode << std::endl;
    }
}
