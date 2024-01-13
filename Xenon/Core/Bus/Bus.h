#pragma once

#include <vector>

#include "Xenon/Base/SystemDevice.h"
#include "Xenon/Core/PCIBus/PCIBus.h"

typedef struct _PCI_TYPE1_CFG_BITS {
    union {
        struct {
            u32   Reserved1 : 2;
            u32   RegisterNumber : 6;
            u32   FunctionNumber : 3;
            u32   DeviceNumber : 5;
            u32   BusNumber : 8;
            u32   Reserved2 : 7;
            u32   Enable : 1;
        } bits;

        u32   AsUINT32;
    } u;
} PCI_TYPE1_CFG_BITS, * PPCI_TYPE1_CFG_BITS;

class Bus
{
public:
	void Init();

    void AddPCIBus(PCIBus* newPCIBus);
	void AddDevice(SystemDevice* device);
	void Read(u64 readAddress, u64* data, u8 byteCount);
	void Write(u64 writeAddress, u64 data, u8 byteCount);
private:
    PCIBus* _PCIBus;
	u32 deviceCount;
	std::vector<SystemDevice*> conectedDevices;
};