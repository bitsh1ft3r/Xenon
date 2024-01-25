#include "SMC.h"

// Further research needed, taken from free60.org info.
#define SMC_AREA_SIZE				0x10 // For what I've found on docs.
#define SMC_AREA_BUS_CONTROL		0x200EA001000
#define SMC_AREA_UART				0x200EA001010
#define SMC_AREA_GPIO_0				0x200EA001020
#define SMC_AREA_GPIO_1				0x200EA001030
#define SMC_AREA_GPIO_2				0x200EA001040
#define SMC_AREA_SMI				0x200EA001050

// UART Adresses
#define UART_CONFIG_ADDR			0x200EA00101C
#define UART_STATUS_ADDR			0x200EA001018
#define UART_BYTE_IN_ADDR			0x200EA001010
#define UART_BYTE_OUT_ADDR			0x200EA001014

void SMC::Read(u64 readAddress, u64* data, u8 byteCount)
{
	smcProcessRead(readAddress, data, byteCount);
}

void SMC::Write(u64 writeAddress, u64 data, u8 byteCount)
{	
	smcProcessWrite(writeAddress, data, byteCount);
}

void SMC::smcProcessRead(u64 readAddress, u64* data, u8 byteCount)
{
	if (readAddress >= SMC_AREA_BUS_CONTROL
		&& readAddress < SMC_AREA_BUS_CONTROL + SMC_AREA_SIZE)
	{
		// Bus control.
		std::cout << "SMC: Bus Control Read, addr 0x" << std::hex <<
			readAddress << std::endl;
	}
	else if (readAddress >= SMC_AREA_UART
		&& readAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// UART.
		uartRead(readAddress, data, byteCount);
	}
	else if (readAddress >= SMC_AREA_UART
		&& readAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// GPIO 0.
		std::cout << "SMC: GPIO 0 Read, addr 0x" << std::hex <<
			readAddress << std::endl;
	}
	else if (readAddress >= SMC_AREA_UART
		&& readAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// GPIO 1.
		std::cout << "SMC: GPIO 1 Read, addr 0x" << std::hex <<
			readAddress << std::endl;
	}
	else if (readAddress >= SMC_AREA_UART
		&& readAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// GPIO 2.
		std::cout << "SMC: GPIO 2 Read, addr 0x" << std::hex <<
			readAddress << std::endl;
	}
	else if (readAddress >= SMC_AREA_UART
		&& readAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// SMI.
		std::cout << "SMC: SMI Read, addr 0x" << std::hex <<
			readAddress << std::endl;
	}
	else
	{
		std::cout << "SMC: Read to unknown address, addr 0x" << std::hex <<
			readAddress << std::endl;
		*data = 0;
	}
}

void SMC::smcProcessWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	if (writeAddress >= SMC_AREA_BUS_CONTROL 
		&& writeAddress < SMC_AREA_BUS_CONTROL + SMC_AREA_SIZE)
	{
		// Bus control.
		std::cout << "SMC: Bus Control Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
	}
	else if (writeAddress >= SMC_AREA_UART
		&& writeAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// UART.
		uartWrite(writeAddress, data, byteCount);
	}
	else if (writeAddress >= SMC_AREA_UART
		&& writeAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// GPIO 0.
		std::cout << "SMC: GPIO 0 Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
	}
	else if (writeAddress >= SMC_AREA_UART
		&& writeAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// GPIO 1.
		std::cout << "SMC: GPIO 1 Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
	}
	else if (writeAddress >= SMC_AREA_UART
		&& writeAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// GPIO 2.
		std::cout << "SMC: GPIO 2 Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
	}
	else if (writeAddress >= SMC_AREA_UART
		&& writeAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// SMI.
		std::cout << "SMC: SMI Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
	}
	else
	{
		std::cout << "SMC: Write to unknown address, addr 0x" << std::hex <<
			writeAddress << std::endl;
	}
}

void SMC::uartRead(u64 readAddress, u64* data, u8 byteCount)
{
	switch (readAddress)
	{
	case UART_STATUS_ADDR:
		// Software reads to this register in order to know when a UART
		// transfer has finished.
		memcpy(data, &uartStatus, byteCount);
		break;
	case UART_BYTE_IN_ADDR:
		// Future input, for now we don't return nothing.
		break;
	default:
		std::cout << "SMC > UART: Read to unknown address!" << std::endl;
		*data = 0;
		break;
	}
}

void SMC::uartWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	u32 writtenData = 0x0;
	memcpy(&writtenData, &data, byteCount);

	switch (writeAddress)
	{
	case UART_CONFIG_ADDR:
		std::cout << "SMC: UART Config Write [STUB], data 0x" << std::hex <<
			writtenData << std::endl;
		return;
		break;
	case UART_BYTE_OUT_ADDR:
		if ((char)writtenData == '\r')
		{
			// End of line, output string and reset buffer.
			std::cout << "SMC > UART: [" << uartDataStr.c_str() << "]"
				<< std::endl;
			uartDataStr.resize(0);
		}
		else
		{
			uartDataStr.push_back((char)writtenData);
		}
		break;
	default:
		std::cout << "SMC > UART: Write to unknown address!" << std::endl;
		break;
	}
	// Set status reg to 0x2 (Xell checks this), signaling software that 
	// we are ready to receive data again.
	uartStatus = 0x2;
}
