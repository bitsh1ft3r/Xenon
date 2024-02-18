#include "SMC.h"
#include "Xenon/Core/PCIBridge/SMC/HANA_State.h"

// Further research needed, taken from free60.org info.
#define SMC_AREA_SIZE					0x10 // For what I've found on docs.
#define SMC_AREA_BUS_CONTROL			0xEA001000
#define SMC_AREA_UART					0xEA001010
#define SMC_AREA_GPIO_0					0xEA001020
#define SMC_AREA_GPIO_1					0xEA001030
#define SMC_AREA_GPIO_2					0xEA001040
#define SMC_AREA_SMI					0xEA001050

// Comunication between kernel/software happens on a FIFO queue, at address
// 0x200EA001084 for writes and 0x200EA001094 for reads.
#define SMC_FIFO_WRITE_STATUS_REG  		0xEA001084
#define SMC_FIFO_WRITE_MSG_REG			0xEA001080
#define SMC_FIFO_READ_STATUS_REG		0xEA001094
#define SMC_FIFO_READ_MSG_REG			0xEA001090
#define SMC_FIFO_STATUS_READY			0x4
#define SMC_FIFO_STATUS_NOT_READY		0x0

// UART Adresses
#define UART_CONFIG_ADDR				0xEA00101C
#define UART_STATUS_ADDR				0xEA001018
#define UART_BYTE_IN_ADDR				0xEA001010
#define UART_BYTE_OUT_ADDR				0xEA001014

// SMC DVD Tray State
enum SMC_TRAY_STATE {
	SMC_TRAY_OPEN = 0x60,
	SMC_TRAY_OPEN_REQUEST = 0x61,
	SMC_TRAY_CLOSE = 0x62,
	SMC_TRAY_OPENING = 0x63,
	SMC_TRAY_CLOSING = 0x64,
	SMC_TRAY_UNKNOWN = 0x65,
	SMC_TRAY_SPINUP = 0x66,
};

// FIFO Queue Querys/Commands
enum SMC_CMD {
	SMC_PWRON_TYPE = 0x1,
	SMC_QUERY_RTC = 0x4,
	SMC_QUERY_TEMP_SENS = 0x7,
	SMC_QUERY_TRAY_STATE = 0xA,
	SMC_QUERY_AVPACK = 0xF,
	SMC_I2C_READ_WRITE = 0x11,
	SMC_QUERY_VERSION = 0x12,
	SMC_FIFO_TEST = 0x13,
	SMC_QUERY_IR_ADDRESS = 0x16,
	SMC_QUERY_TILT_SENSOR = 0x17,
	SMC_READ_82_INT = 0x1e,
	SMC_READ_8E_INT = 0x20,
	SMC_SET_STANDBY = 0x82,
	SMC_SET_TIME = 0x85,
	SMC_SET_FAN_ALGORITHM = 0x88,
	SMC_SET_FAN_SPEED_CPU = 0x89,
	SMC_SET_DVD_TRAY = 0x8b,
	SMC_SET_POWER_LED = 0x8c,
	SMC_SET_AUDIO_MUTE = 0x8d,
	SMC_ARGON_RELATED = 0x90,
	SMC_SET_FAN_SPEED_GPU = 0x94, // Not present on slim, not used/respected on newer fat.
	SMC_SET_IR_ADDRESS = 0x95,
	SMC_SET_DVD_TRAY_SECURE = 0x98,
	SMC_SET_FP_LEDS = 0x99,
	SMC_SET_RTC_WAKE = 0x9a,
	SMC_ANA_RELATED = 0x9b,
	SMC_SET_ASYNC_OPERATION = 0x9c,
	SMC_SET_82_INT = 0x9d,
	SMC_SET_9F_INT = 0x9f,
};

// SMC Power On Reason
enum SMC_PWR_REAS {
	SMC_PWR_REAS_PWRBTN = 0x11, // xss5 power button pushed
	SMC_PWR_REAS_EJECT = 0x12, // xss6 eject button pushed
	SMC_PWR_REAS_ALARM = 0x15, // xss guess ~ should be the wake alarm ~
	SMC_PWR_REAS_REMOPWR = 0x20, // xss2 power button on 3rd party remote/ xbox universal remote
	SMC_PWR_REAS_REMOEJC = 0x21, // eject button on xbox universal remote
	SMC_PWR_REAS_REMOX = 0x22, // xss3 xbox universal media remote X button
	SMC_PWR_REAS_WINBTN = 0x24, // xss4 windows button pushed IR remote
	SMC_PWR_REAS_RESET = 0x30, // xss HalReturnToFirmware(1 or 2 or 3) = hard reset by smc
	SMC_PWR_REAS_RECHARGE_RESET = 0x31, // after leaving pnc charge mode via power button
	SMC_PWR_REAS_KIOSK = 0x41, // xss7 console powered on by kiosk pin
	SMC_PWR_REAS_WIRELESS = 0x55, // xss8 wireless controller middle button/start button pushed to power on controller and console
	SMC_PWR_REAS_WIRED_F1 = 0x56, // xss9 wired guide button; fat front top USB port, slim front left USB port
	SMC_PWR_REAS_WIRED_F2 = 0x57, // xssA wired guide button; fat front botton USB port, slim front right USB port
	SMC_PWR_REAS_WIRED_R2 = 0x58, // xssB wired guide button; slim back middle USB port
	SMC_PWR_REAS_WIRED_R3 = 0x59, // xssC wired guide button; slim back top USB port
	SMC_PWR_REAS_WIRED_R1 = 0x5A, // xssD wired guide button; fat back USB port, slim back bottom USB port
	// possibles/reboot reasons  0x23, 0x2A, 0x42, 0x61, 0x64

	// slim with wired controller when horozontal, 3 back usb ports top to bottom 0x59, 0x58, 0x5A front left 0x56, right 0x57
	// slim with wireless controller w/pnc when horozontal, 3 back usb ports top to bottom 0x55, 0x58, 0x5A front left 0x56, right 0x57
	// fat with wired controller when horozontal, 1 back usb port 0x5A front top 0x56, bottom 0x57
	// fat with wireless controller w/pnc when horozontal, 1 back usb port 0x5A front top 0x56, bottom 0x57
	// Using Microsoft Wireless Controller: 0x55
	// Using Madcatz Wireless Keyboard (Rockband 3 Keyboard - Item Number 98161): 0x55
	// Using Activision Wireless Turntable Controller (DJ Hero Turntable): 0x55
	// Using Drums Controller from Activision Guitar Hero Warriors of Rock: 0x55
	// Using Guitar controller from Activision Guitar Hero 5: 0x55
};

SMC::SMC()
{
	// Set PCI Ven & Dev ID.
	u32 devID = 0x580d1414;
	memcpy(&configReg[0], &devID, 4);
}

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
	else if (readAddress == SMC_FIFO_READ_STATUS_REG || readAddress == SMC_FIFO_READ_MSG_REG
		|| readAddress == SMC_FIFO_WRITE_STATUS_REG)
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
	else if (writeAddress == SMC_FIFO_WRITE_STATUS_REG || writeAddress == SMC_FIFO_WRITE_MSG_REG
		|| writeAddress == SMC_FIFO_READ_STATUS_REG)
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
		*data = '\0';
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
		if (writtenData >= 0x20 && writtenData <= 0x7E)
		{
			std::cout << (char)writtenData;
			// Printable text!
			uartDataStr.push_back((char)writtenData);
		}
		else if (writtenData >= 0x00 && writtenData <= 0x1F ||
			writtenData == 0x7F)
		{
			// Control Characters
			switch (writtenData)
			{
			case 0x0:
				// '/0'
				break;
			case 0x7:
				std::cout << "SMC > UART: BELL!" << std::endl;
				break;
			case 0x8: // Backspace
			std::cout << "\b";
				break;
			case 0xA: // New Line
				std::cout << "\n";
				break;
			case 0xD: // Carriage Return
				std::cout << "\r";
				break;
			case 0x1c: // Carriage Return
				std::cout << (char)writtenData;
				break;
			default:
				std::cout << "SMC > UART: Unhandled Control Character: 0x"
					<< writtenData << std::endl;
				break;
			}
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

void SMC::smcFIFOProcessRead(u64 readAddress, u64* data, u8 byteCount)
{

	// IS message ready for reading?
	if (readAddress == SMC_FIFO_READ_STATUS_REG)
	{
		// Return message.
		*data = fifoStatusReg;
		return;
	}

	if (readAddress == SMC_FIFO_READ_MSG_REG)
	{
		memcpy(data, &fifoReadedMsg[currentReadPos], byteCount);
		// Begin message read.
		if (currentReadPos == 12)
		{
			// New Message.
			currentReadPos = 0;
			// Reset buffer.
			memset(&fifoReadedMsg, 0, sizeof(fifoReadedMsg));
			return;
		}
		currentReadPos += 4;
		return;
	}

	if (readAddress == SMC_FIFO_WRITE_STATUS_REG)
	{
		// Return message.
		*data = fifoStatusReg;
		return;
	}
}

void SMC::smcFIFOProcessWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	// Software tells us that it's about to receive a msg or that it has
	// finished receiving it.
	if (writeAddress == SMC_FIFO_READ_STATUS_REG)
	{
		fifoStatusReg = static_cast<u8>(data);

		// Finished sending a response.
		if (fifoStatusReg == SMC_FIFO_STATUS_NOT_READY)
		{
			// Reset reply buffer.
			memset(&fifoReadedMsg, 0, sizeof(fifoReadedMsg));

			// Ready to reveice another message.
			fifoStatusReg = SMC_FIFO_STATUS_READY;
		}
		return;
	}

	// Software tells us that it's about to send a msg or that it has
	// finished sending it.
	if (writeAddress == SMC_FIFO_WRITE_STATUS_REG)
	{
		fifoStatusReg = static_cast<u8>(data);

		u32 writtenValue = 0;

		// finished receiving the message. Process it.
		if (fifoStatusReg == SMC_FIFO_STATUS_NOT_READY)
		{
			// Begin Message Procesing!
			switch (fifoWrittenMsg[0])
			{
			case SMC_PWRON_TYPE:
				// Change here to not enter Xell on modified nands!
				fifoReadedMsg[0] = SMC_PWRON_TYPE;
				fifoReadedMsg[1] = SMC_PWR_REAS_EJECT; // Eject button.
				break;
			case SMC_QUERY_RTC:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_QUERY_RTC"
					<< std::endl;
				break;
			case SMC_QUERY_TEMP_SENS:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_QUERY_TEMP_SENS"
					<< std::endl;
				break;
			case SMC_QUERY_TRAY_STATE:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_QUERY_TRAY_STATE"
					<< std::endl;
				break;
			case SMC_QUERY_AVPACK:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_QUERY_AVPACK"
					<< std::endl;
				break;
			case SMC_I2C_READ_WRITE:				
				switch (fifoWrittenMsg[1])
				{
				case 0x10: // SMC_READ_ANA
					fifoReadedMsg[0] = SMC_I2C_READ_WRITE;
					fifoReadedMsg[1] = 0x0;
					fifoReadedMsg[4] = (HANA_State[fifoWrittenMsg[6]] & 0xFF);
					fifoReadedMsg[5] = ((HANA_State[fifoWrittenMsg[6]] >> 8) & 0xFF);
					fifoReadedMsg[6] = ((HANA_State[fifoWrittenMsg[6]] >> 16) & 0xFF);
					fifoReadedMsg[7] = ((HANA_State[fifoWrittenMsg[6]] >> 24) & 0xFF);
					break;
				case 0x60: // SMC_WRITE_ANA
					fifoReadedMsg[0] = SMC_I2C_READ_WRITE;
					fifoReadedMsg[1] = 0x0;
					HANA_State[fifoWrittenMsg[6]] = fifoWrittenMsg[4] 
						| (fifoWrittenMsg[5] << 8)
						| (fifoWrittenMsg[6] << 16) 
						| (fifoWrittenMsg[7] << 24);
					break;
				default:
					std::cout << "SMC: SMC_I2C_READ_WRITE Unimplemented command 0x"
						<< fifoWrittenMsg[1] << std::endl;
					fifoReadedMsg[0] = SMC_I2C_READ_WRITE;
					fifoReadedMsg[1] = 0x1; // Set R/W Failed.
				}
				break;
			case SMC_QUERY_VERSION:
				fifoReadedMsg[0] = SMC_QUERY_VERSION;
				fifoReadedMsg[1] = 0x41;
				fifoReadedMsg[2] = 0x02;
				fifoReadedMsg[3] = 0x03;
				break;
			case SMC_FIFO_TEST:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_FIFO_TEST"
					<< std::endl;
				break;
			case SMC_QUERY_IR_ADDRESS:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_QUERY_IR_ADDRESS"
					<< std::endl;
				break;
			case SMC_QUERY_TILT_SENSOR:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_QUERY_TILT_SENSOR"
					<< std::endl;
				break;
			case SMC_READ_82_INT:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_READ_82_INT"
					<< std::endl;
				break;
			case SMC_READ_8E_INT:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_READ_8E_INT"
					<< std::endl;
				break;
			case SMC_SET_STANDBY:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_STANDBY"
					<< std::endl;
				break;
			case SMC_SET_TIME:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_TIME"
					<< std::endl;
				break;
			case SMC_SET_FAN_ALGORITHM:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_FAN_ALGORITHM"
					<< std::endl;
				break;
			case SMC_SET_FAN_SPEED_CPU:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_FAN_SPEED_CPU"
					<< std::endl;
				break;
			case SMC_SET_DVD_TRAY:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_DVD_TRAY"
					<< std::endl;
				break;
			case SMC_SET_POWER_LED:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_POWER_LED"
					<< std::endl;
				break;
			case SMC_SET_AUDIO_MUTE:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_AUDIO_MUTE"
					<< std::endl;
				break;
			case SMC_ARGON_RELATED:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_ARGON_RELATED"
					<< std::endl;
				break;
			case SMC_SET_FAN_SPEED_GPU:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_FAN_SPEED_GPU"
					<< std::endl;
				break;
			case SMC_SET_IR_ADDRESS:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_IR_ADDRESS"
					<< std::endl;
				break;
			case SMC_SET_DVD_TRAY_SECURE:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_DVD_TRAY_SECURE"
					<< std::endl;
				break;
			case SMC_SET_FP_LEDS:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_FP_LEDS"
					<< std::endl;
				break;
			case SMC_SET_RTC_WAKE:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_RTC_WAKE"
					<< std::endl;
				break;
			case SMC_ANA_RELATED:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_ANA_RELATED"
					<< std::endl;
				break;
			case SMC_SET_ASYNC_OPERATION:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_ASYNC_OPERATION"
					<< std::endl;
				break;
			case SMC_SET_82_INT:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_82_INT"
					<< std::endl;
				break;
			case SMC_SET_9F_INT:
				std::cout << "SMC Unimplemented SMC FIFO Command SMC_SET_9F_INT"
					<< std::endl;
				break;
			default:
				std::cout << "SMC Unimplemented SMC FIFO Command 0x"
					<< (u16)fifoWrittenMsg[0] << std::endl;
				break;
			}

			// Set status reg to signal the software that we're ready to 
			// send a response.
			fifoStatusReg = SMC_FIFO_STATUS_READY;
			return;
		}
		return;
	}

	// Software sends 4 32-bit words to this address, this is our message.
	if (writeAddress == SMC_FIFO_WRITE_MSG_REG)
	{
		if (currentWritePos == 16)
		{
			// New Message.
			currentWritePos = 0;
			// Reset query/command buffer.
			memset(&fifoWrittenMsg, 0, sizeof(fifoWrittenMsg));
		}
		memcpy(&fifoWrittenMsg[currentWritePos], &data, byteCount);
		currentWritePos += 4;
		return;
	}
}