#pragma once

#include "Xenon/Base/TypeDefs.h"

union PCI_TYPE1_REG0
{
	u32 dataHex;
	struct
	{
		u32 vendorID : 16;
		u32 deviceID : 16;
	};
};

union PCI_TYPE1_REG1
{
	u32 dataHex;
	struct
	{
		u32 command : 16;
		u32 status : 16;
	};
};

union PCI_TYPE1_REG2
{
	u32 dataHex;
	struct
	{
		u32 revID : 8;
		u32 progIF : 8;
		u32 subClass : 8;
		u32 classCode : 8;
	};
};

struct PCI_TYPE1_CONFIG_HDR
{
	PCI_TYPE1_REG0 reg0;
	PCI_TYPE1_REG1 reg1;
	PCI_TYPE1_REG2 reg2;
};

struct PCIDeviceInfo
{
	const char* deviceName;
	u64 size;
};

class PCIDevice
{
public:
	void Initialize(const char* deviceName, u64 size) { deviceInfo.deviceName = deviceName; deviceInfo.size = size; };
	virtual void Read(u64 readAddress, u64* data, u8 byteCount) {};
	virtual void Write(u64 writeAddress, u64 data, u8 byteCount) {};

	virtual void ConfigRead(u64 readAddress, u64* data, u8 byteCount) {};
	virtual void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {};

	const char* GetDeviceName() { return deviceInfo.deviceName; }
	u64 GetStartAddress() { 
		u32 bar0 = 0;
		memcpy(&bar0, &configReg[0x10], 4);
		return bar0;
	}
	u64 GetEndAddress() {
		u32 bar0 = 0;
		memcpy(&bar0, &configReg[0x10], 4);
		return bar0 + deviceInfo.size;
	}

	// Configuration Ring.
	u8 configReg[256] = {};

private:
	PCIDeviceInfo deviceInfo;
};