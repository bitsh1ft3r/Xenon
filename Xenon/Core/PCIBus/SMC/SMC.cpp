#include "SMC.h"

void SMC::Read(u64 readAddress, u64* data, u8 byteCount)
{
	std::cout << "SMC: Read at address 0x" << std::hex << readAddress << std::endl;
	data = 0;
}

void SMC::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	std::cout << "SMC: Write at address 0x" << std::hex << writeAddress << " data: 0x" << data << std::endl;
}
