#pragma once

#include "Xenon/Base/TypeDefs.h"

struct PCIDeviceInfo
{
	const char* deviceName;
	u64 startAddr;
	u64 endAddr;
};

class PCIDevice
{
public:
	void Initialize(const char* deviceName, u64 startAddress, u64 endAddress) { deviceInfo.deviceName = deviceName; deviceInfo.startAddr = startAddress; deviceInfo.endAddr = endAddress; };
	virtual void Read(u64 readAddress, u64* data, u8 byteCount) {};
	virtual void Write(u64 writeAddress, u64 data, u8 byteCount) {};

	virtual void ConfigRead(u64 readAddress, u64* data, u8 byteCount) {};
	virtual void ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {};

	const char* GetDeviceName() { return deviceInfo.deviceName; }
	u64 GetStartAddress() { return deviceInfo.startAddr; }
	u64 GetEndAddress() { return deviceInfo.endAddr; }

private:
	PCIDeviceInfo deviceInfo;
};