#include "SMC.h"

// Further research needed, taken from free60.org info.
#define SMC_AREA_SIZE				0x10 // For what I've found on docs.
#define SMC_AREA_BUS_CONTROL		0x200EA001000
#define SMC_AREA_UART				0x200EA001010
#define SMC_AREA_GPIO_0				0x200EA001020
#define SMC_AREA_GPIO_1				0x200EA001030
#define SMC_AREA_GPIO_2				0x200EA001040
#define SMC_AREA_SMI				0x200EA001050

// Comunication between kernel/software happens on a FIFO queue, at address
// 0x200EA001084 for writes and 0x200EA001094 for reads.
#define SMC_FIFO_WRITE  			0x200EA001084
#define SMC_FIFO_WRITE_MSG			0x200EA001080
#define SMC_FIFO_READ				0x200EA001094
#define SMC_FIFO_READ_MSG			0x200EA001090
#define SMC_FIFO_READY				0x4
#define SMC_FIFO_NOT_READY			0x0

// UART Adresses
#define UART_CONFIG_ADDR			0x200EA00101C
#define UART_STATUS_ADDR			0x200EA001018
#define UART_BYTE_IN_ADDR			0x200EA001010
#define UART_BYTE_OUT_ADDR			0x200EA001014

// FIFO Queue Querys/Commands
#define SMC_QUERY_PWRON_TYPE		0x1
#define SMC_QUERY_REALTIME_CLK		0x4
#define SMC_QUERY_TEMP_READ			0x7
#define SMC_QUERY_TRAY_STATE		0xA
#define SMC_QUERY_AVPACK_TYPE		0xF
#define SMC_QUERY_READ_ANA			0x11
#define SMC_QUERY_SMC_VER			0x12
#define SMC_QUERY_IR_ACCESS			0x16
#define SMC_QUERY_TILT_STATE		0x17
#define SMC_COMMAND_SET_STBY		0x83
#define SMC_COMMAND_SET_PWRLED		0x8C

void SMC::Read(u64 readAddress, u64* data, u8 byteCount)
{
	smcProcessRead(readAddress, data, byteCount);
}

void SMC::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	smcProcessWrite(writeAddress, data, byteCount);
}

void SMC::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &configReg[static_cast<u8>(readAddress)], byteCount);
}

void SMC::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&configReg[static_cast<u8>(writeAddress)], &data, byteCount);
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
	else if (readAddress == SMC_FIFO_READ || readAddress == SMC_FIFO_READ_MSG
		|| readAddress == SMC_FIFO_WRITE)
	{
		// FIFO Queue Read
		smcFIFOProcessRead(readAddress, data, byteCount);
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
	else if (writeAddress == SMC_FIFO_WRITE || writeAddress == SMC_FIFO_WRITE_MSG
		|| writeAddress == SMC_FIFO_READ)
	{
		// FIFO Queue Write
		smcFIFOProcessWrite(writeAddress, data, byteCount);
	}
	else
	{
		std::cout << "SMC: Write to unknown address, addr 0x" << std::hex <<
			writeAddress << " data = 0x" << data << std::endl;
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
	case UART_BYTE_OUT_ADDR:
		// Further research required.
		break;
	default:
		std::cout << "SMC > UART: Read to unknown address!" << std::endl;
		*data = 0;
		break;
	}
}

void SMC::smcFIFOProcessRead(u64 readAddress, u64* data, u8 byteCount)
{
	
	// If there's a begin read message and we have a message ready.
	if (readAddress == SMC_FIFO_READ || readAddress == SMC_FIFO_WRITE)
	{
		// Return message.
		*data = fifoReadReg;
		return;
	}

	if (readAddress == SMC_FIFO_READ_MSG && fifoReadReg == SMC_FIFO_READY)
	{
		// Begin message read.
		if (currentReadPos == 16)
		{
			// New Message.
			currentWritePos = 0;
			// Reset buffer.
			memset(&fifoReadedMsg, 0, sizeof(fifoReadedMsg));
		}

		memcpy(data, &fifoReadedMsg[currentReadPos], byteCount);
		currentReadPos += 4;
		return;
	}
	
}

void SMC::smcFIFOProcessWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	// If FIFO Ready and data is 0x4 the we are starting a message.
	if (writeAddress == SMC_FIFO_WRITE && fifoWriteReg == SMC_FIFO_READY 
		&& data == 4)
	{
		fifoWriteReg = static_cast<u8>(data);
		return;
	}

	if (writeAddress == SMC_FIFO_WRITE_MSG && fifoWriteReg == SMC_FIFO_READY)
	{
		if (currentWritePos == 16)
			currentWritePos = 0; // New Message.
		memcpy(&fifoWrittenMsg[currentWritePos], &data, byteCount);
		currentWritePos += 4;
		return;
	}

	// If FIFO Ready and data is 0, begin processing.
	if (writeAddress == SMC_FIFO_WRITE && fifoWriteReg == SMC_FIFO_READY
		&& data == 0)
	{
		// Begin Message Procesing!
		switch (fifoWrittenMsg[0])
		{
		case SMC_QUERY_PWRON_TYPE:
			// Cahange here to not enter Xell!
			fifoReadedMsg[0] = SMC_QUERY_PWRON_TYPE;
			fifoReadedMsg[1] = 0x12; // Eject button.
			fifoReadReg = SMC_FIFO_READY;
			break;
		case SMC_QUERY_SMC_VER:
			fifoReadedMsg[0] = SMC_QUERY_SMC_VER;
			fifoReadedMsg[1] = 0x41;
			fifoReadedMsg[2] = 0x02;
			fifoReadedMsg[3] = 0x03;
			break;
		default:
			std::cout << "SMC Unimplemented SMC FIFO Command 0x" 
				<< (u16)fifoWrittenMsg[0] << std::endl;
			break;
		}
		
		return;
	}

	if (writeAddress == SMC_FIFO_READ && fifoReadReg == SMC_FIFO_READY)
	{
		fifoReadReg = static_cast<u8>(data);
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
			//std::cout << "SMC > UART: " << std::endl;
			std::cout << uartDataStr.c_str() << std::endl;
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
