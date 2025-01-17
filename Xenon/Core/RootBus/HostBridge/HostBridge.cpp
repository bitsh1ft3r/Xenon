// Copyright 2025 Xenon Emulator Project

#include "HostBridge.h"

HostBridge::HostBridge()
{
    xGPU = nullptr;
    pciBridge = nullptr;
    // Config HostBridge As Per Dump taken from a Jasper Console.
    // Device/Vendor ID
    hostBridgeConfigSpace.configSpaceHeader.reg0.hexData = 0x58301414;
    // Device Type/Revision
    hostBridgeConfigSpace.configSpaceHeader.reg1.hexData = 0x06000010;
}

void HostBridge::RegisterXGPU(Xe::Xenos::XGPU* newXGPU)
{
    xGPU = newXGPU;
}

void HostBridge::RegisterPCIBridge(PCIBridge* newPCIBridge)
{
    pciBridge = newPCIBridge;
    return;
}

bool HostBridge::Read(u64 readAddress, u64* data, u8 byteCount)
{
    // Reading from host bridge registers?
    if (isAddressMappedinBAR(static_cast<u32>(readAddress)))
    {
        switch (readAddress)
        {
            // HostBridge
        case 0xE0020000:
            *data = hostBridgeRegs.REG_E0020000;
            break;
        case 0xE0020004:
            *data = hostBridgeRegs.REG_E0020004;
            break;
            // BIU
        case 0xE1020004:
            *data = biuRegs.REG_E1020004;
            break;
        case 0xE1010010:
            *data = biuRegs.REG_E1010010;
            break;
        case 0xE1018000:
            *data = biuRegs.REG_E1018000;
            break;
        case 0xE1020000:
            *data = biuRegs.REG_E1020000;
            break;
        case 0xE1040000:
            *data = biuRegs.REG_E1040000;
            break;
        default:
            std::cout << "HostBridge/BIU: Unknown register being read at address 0x" 
                << readAddress << std::endl;
            *data = 0;
            break;
        }
        return true;
    }

    // Check if this address is in the PCI Bridge
    if (xGPU->isAddressMappedInBAR(static_cast<u32>(readAddress)))
    {
        xGPU->Read(readAddress, data, byteCount);
        return true;
    }

    // Check if this address is in the PCI Bridge
    if (pciBridge->isAddressMappedinBAR(static_cast<u32>(readAddress)))
    {
        pciBridge->Read(readAddress, data, byteCount);
        return true;
    }

    // Read failed or address is not on this bus.
    return false;
}

bool HostBridge::Write(u64 writeAddress, u64 data, u8 byteCount)
{
    // Writing to host bridge registers?
    if (isAddressMappedinBAR(static_cast<u32>(writeAddress)))
    {
        switch (writeAddress)
        {
            // HostBridge
        case 0xE0020000:
            hostBridgeRegs.REG_E0020000 = static_cast<u32>(data);
            break;
        case 0xE0020004:
            hostBridgeRegs.REG_E0020004 = static_cast<u32>(data);
            break;
            // BIU
        case 0xE1003000:
            biuRegs.REG_E1003000 = static_cast<u32>(data);
            break;
        case 0xE1003100:
            biuRegs.REG_E1003100 = static_cast<u32>(data);
            break;
        case 0xE1003200:
            biuRegs.REG_E1003200 = static_cast<u32>(data);
            break;
        case 0xE1003300:
            biuRegs.REG_E1003300 = static_cast<u32>(data);
            break;
        case 0xE1010000:
            biuRegs.REG_E1010000 = static_cast<u32>(data);
            biuRegs.REG_E1010010 = static_cast<u32>(data); // Reading to this addr on a retail returns the same data on this address.
            break;
        case 0xE1010010:
            biuRegs.REG_E1010010 = static_cast<u32>(data);
            break;
        case 0xE1010020:
            biuRegs.REG_E1010020 = static_cast<u32>(data);
            break;
        case 0xE1013000:
            biuRegs.REG_E1013000 = static_cast<u32>(data);
            break;
        case 0xE1013100:
            biuRegs.REG_E1013100 = static_cast<u32>(data);
            break;
        case 0xE1013200:
            biuRegs.REG_E1013200 = static_cast<u32>(data);
            break;
        case 0xE1013300:
            biuRegs.REG_E1013300 = static_cast<u32>(data);
            break;
        case 0xE1018020:
            biuRegs.REG_E1018000 = static_cast<u32>(data); // Same as above.
            biuRegs.REG_E1018020 = static_cast<u32>(data);
            break;
        case 0xE1020000:
            biuRegs.REG_E1020000 = static_cast<u32>(data);
            break;
        case 0xE1020004:
            biuRegs.REG_E1020004 = static_cast<u32>(data);
            break;
        case 0xE1020008:
            biuRegs.REG_E1020008 = static_cast<u32>(data);
            break;
        case 0xE1040000:
            biuRegs.REG_E1040000 = static_cast<u32>(data);
            break;
        case 0xE1040074:
            biuRegs.REG_E1040074 = static_cast<u32>(data);
            break;
        case 0xE1040078:
            biuRegs.REG_E1040078 = static_cast<u32>(data);
            break;
        default:
            std::cout << "HostBridge/BIU: Unknown register being written at address 0x"
                << writeAddress << " data = 0x" << data << std::endl;
            break;
        }
        return true;
    }

    // Check if this address is mapped on the GPU
    if (xGPU->isAddressMappedInBAR(static_cast<u32>(writeAddress)))
    {
        xGPU->Write(writeAddress, data, byteCount);
        return true;
    }

    // Check if this address is in the PCI Bridge
    if (pciBridge->isAddressMappedinBAR(static_cast<u32>(writeAddress)))
    {
        pciBridge->Write(writeAddress, data, byteCount);
        return true;
    }

    // Write failed or address is not on this bus.
    return false;
}

void HostBridge::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
    PCIE_CONFIG_ADDR configAddress = {};
    configAddress.hexData = static_cast<u32>(readAddress);

    if (configAddress.busNum == 0)
    {
        switch (configAddress.devNum)
        {
        case 0x0:   // PCI-PCI Bridge
            pciBridge->ConfigRead(readAddress, data, byteCount);
            break;
        case 0x1:   // Host Bridge
            memcpy(data, &hostBridgeConfigSpace.data[configAddress.regOffset], byteCount);
            break;
        case 0x2:   // GPU + Memory Controller!
            xGPU->ConfigRead(readAddress, data, byteCount);
            break;
        default:
            std::cout << "BUS 0: Configuration Read to inexistant PCI Device at address 0x"
                << readAddress << std::endl;
            break;
        }
        return;
    }

    // Config Address belongs to a secondary Bus, let's send it to the PCI-PCI Bridge
    pciBridge->ConfigRead(readAddress, data, byteCount);

}

void HostBridge::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
    PCIE_CONFIG_ADDR configAddress = {};
    configAddress.hexData = static_cast<u32>(writeAddress);

    if (configAddress.busNum == 0)
    {
        switch (configAddress.devNum)
        {
        case 0x0:   // PCI-PCI Bridge
            pciBridge->ConfigWrite(writeAddress, data, byteCount);
            break;
        case 0x1:   // Host Bridge
            memcpy(&hostBridgeConfigSpace.data[configAddress.regOffset], &data, byteCount);
            break;
        case 0x2:   // GPU/Memory Controller
            xGPU->ConfigWrite(writeAddress, data, byteCount);
            break;
        default:
            std::cout << "BUS 0: Configuration Write to inexistant PCI Device at address 0x"
                << writeAddress << std::endl;
            break;
        }
        return;
    }
    
    // Config Address belongs to a secondary Bus, let's send it to the PCI-PCI Bridge
    pciBridge->ConfigWrite(writeAddress, data, byteCount);
}

bool HostBridge::isAddressMappedinBAR(u32 address)
{
    u32 bar0 = hostBridgeConfigSpace.configSpaceHeader.BAR0;
    u32 bar1 = hostBridgeConfigSpace.configSpaceHeader.BAR1;
    u32 bar2 = hostBridgeConfigSpace.configSpaceHeader.BAR2;
    u32 bar3 = hostBridgeConfigSpace.configSpaceHeader.BAR3;
    u32 bar4 = hostBridgeConfigSpace.configSpaceHeader.BAR4;
    u32 bar5 = hostBridgeConfigSpace.configSpaceHeader.BAR5;

    if (address >= bar0 && address <= bar0 + HOST_BRIDGE_SIZE
        || address >= bar1 && address <= bar1 + HOST_BRIDGE_SIZE
        || address >= bar2 && address <= bar2 + HOST_BRIDGE_SIZE
        || address >= bar3 && address <= bar3 + HOST_BRIDGE_SIZE
        || address >= bar4 && address <= bar4 + HOST_BRIDGE_SIZE
        || address >= bar5 && address <= bar5 + HOST_BRIDGE_SIZE)
    {
        return true;
    }

    return false;
}
