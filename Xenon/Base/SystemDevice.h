#pragma once

#include "Xenon/Base/TypeDefs.h"

struct deviceInfo
{
	const char* deviceName;
	u64 startAddr;
	u64 endAddr;
};

class SystemDevice
{
public:
	void Initialize(const char* deviceName, u64 startAddress, u64 endAddress) { info.deviceName = deviceName; info.startAddr = startAddress; info.endAddr = endAddress; };
	virtual void Read(u64 readAddress, u64* data, u8 byteCount) {};
	virtual void Write(u64 writeAddress, u64 data, u8 byteCount) {};

	const char* GetDeviceName() { return info.deviceName; }
	u64 GetStartAddress() { return info.startAddr; }
	u64 GetEndAddress() { return info.endAddr; }

private:
	deviceInfo info;
};