#include "SMC.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/SMC_Config.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/HANA_State.h"

#define REG_FIFO_IN_DATA	0x80
#define REG_FIFO_IN_STATUS	0x84
#define FIFO_STATUS_RDY		0x4
#define FIFO_STATUS_NOT_RDY 0x0

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

// AVPACK's Taken from libXenon.
enum SMC_AVPACK_TYPE
{
	HDMI_AUDIO = 0x13,					// HDMI_AUDIO
	HDMI_AUDIO_0x14 = 0x14,				// HDMI_AUDIO - GHETTO MOD
	HDMI_AUDIO_GHETTO_MOD = 0x1C,		// HDMI_AUDIO - GHETTO MOD
	HDMI_AUDIO_GHETTO_MOD_0x1e = 0x1E,	// HDMI
	HDMI = 0x1F,						// HDMI
	COMPOSITE_TV_MODE = 0x43,			// COMPOSITE - TV MODE
	SCART = 0x47,						// SCART
	COMPOSITE_S_VIDEO = 0x54,			// COMPOSITE + S-VIDEO
	COMPOSITE = 0x57,					// NORMAL COMPOSITE
	COMPONENT = 0x0C,					// COMPONENT
	COMPONENT_0xF = 0x0F,				// COMPONENT
	COMPOSITE_HD_MODE = 0x4F,			// COMPOSITE - HD MODE
	VGA = 0x5B,							// VGA
	VGA_0x5B = 0x59,					// VGA
	VGA_ADP_FIX = 0x1B					// This fixes a generic VGA-HDMI Adapter
};

SMC::SMC(PCIBridge* parentPCIBridge)
{
	// Set config registers.
	u8 i = 0;
	for (u16 idx = 0; idx < 256; idx += 4)
	{
		memcpy(&pciConfigSpace.data[idx], &smcConfigSpaceMap[i], 4);
		i++;
	}

	// Asign parent PCI Bridge pointer.
	parentBus = parentPCIBridge;

	// Enter SMC Thread.
	smcThread = std::thread(&SMC::smcLoop, this);
}

void SMC::Read(u64 readAddress, u64* data, u8 byteCount)
{
	u8 offset = readAddress & 0xFF;

	smcProcessRead(readAddress, data, byteCount);
}

void SMC::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	smcProcessWrite(writeAddress, data, byteCount);

	u8 offset = writeAddress & 0xFF;
}

void SMC::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void SMC::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}

void SMC::smcProcessRead(u64 readAddress, u64* data, u8 byteCount)
{
	if (readAddress >= SMC_AREA_BUS_CONTROL
		&& readAddress < SMC_AREA_BUS_CONTROL + SMC_AREA_SIZE)
	{
		// Bus control.
		std::cout << "SMC: Bus Control Read, addr 0x" << std::hex <<
			readAddress << std::endl;
		return;
	}
	else if (readAddress >= SMC_AREA_UART
		&& readAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// UART.
		uartRead(readAddress, data, byteCount);
		return;
	}
	else if (readAddress >= SMC_AREA_GPIO_0
		&& readAddress < SMC_AREA_GPIO_0 + SMC_AREA_SIZE)
	{
		// GPIO 0.
		std::cout << "SMC: GPIO 0 Read, addr 0x" << std::hex <<
			readAddress << std::endl;
		return;
	}
	else if (readAddress >= SMC_AREA_GPIO_1
		&& readAddress < SMC_AREA_GPIO_1 + SMC_AREA_SIZE)
	{
		// GPIO 1.
		std::cout << "SMC: GPIO 1 Read, addr 0x" << std::hex <<
			readAddress << std::endl;
		return;
	}
	else if (readAddress >= SMC_AREA_GPIO_2
		&& readAddress < SMC_AREA_GPIO_2 + SMC_AREA_SIZE)
	{
		// GPIO 2.
		std::cout << "SMC: GPIO 2 Read, addr 0x" << std::hex <<
			readAddress << std::endl;
		return;
	}
	else if (readAddress >= SMC_AREA_SMI
		&& readAddress < SMC_AREA_SMI + SMC_AREA_SIZE)
	{
		// SMI.
		std::cout << "SMC: SMI Read, addr 0x" << std::hex <<
			readAddress << std::endl;
		*data = intPending;
		return;
	}
	else if (readAddress == SMC_FIFO_READ_STATUS_REG || readAddress == SMC_FIFO_READ_MSG_REG
		|| readAddress == SMC_FIFO_WRITE_STATUS_REG)
	{
		// FIFO Queue Read
		smcFIFOProcessRead(readAddress, data, byteCount);
		return;
	}
	else if (readAddress == 0xEA00106C)
	{
		// Clock interrupt related.
		*data = smcState.clockIntReg;
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
		return;
	}
	else if (writeAddress >= SMC_AREA_UART
		&& writeAddress < SMC_AREA_UART + SMC_AREA_SIZE)
	{
		// UART.
		uartWrite(writeAddress, data, byteCount);
		return;
	}
	else if (writeAddress >= SMC_AREA_GPIO_0
		&& writeAddress < SMC_AREA_GPIO_0 + SMC_AREA_SIZE)
	{
		// GPIO 0.
		std::cout << "SMC: GPIO 0 Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
		return;
	}
	else if (writeAddress >= SMC_AREA_GPIO_1
		&& writeAddress < SMC_AREA_GPIO_1 + SMC_AREA_SIZE)
	{
		// GPIO 1.
		std::cout << "SMC: GPIO 1 Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
		return;
	}
	else if (writeAddress >= SMC_AREA_GPIO_2
		&& writeAddress < SMC_AREA_GPIO_2 + SMC_AREA_SIZE)
	{
		// GPIO 2.
		std::cout << "SMC: GPIO 2 Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
		return;
	}
	else if (writeAddress >= SMC_AREA_SMI
		&& writeAddress < SMC_AREA_SMI + SMC_AREA_SIZE)
	{
		// SMI.
		std::cout << "SMC: SMI Write, addr 0x" << std::hex <<
			writeAddress << std::endl;
		intPending = static_cast<u32>(data);
		return;
	}
	else if (writeAddress == SMC_FIFO_WRITE_STATUS_REG || writeAddress == SMC_FIFO_WRITE_MSG_REG
		|| writeAddress == SMC_FIFO_READ_STATUS_REG)
	{
		// FIFO Queue Write
		smcFIFOProcessWrite(writeAddress, data, byteCount);
		return;
	}
	else if (writeAddress == 0xEA00106C)
	{
		// Clock interrupt related.
		smcState.clockIntReg = static_cast<u32>(data);
	}
	else
	{
		std::cout << "SMC: Write to unknown address, addr 0x" << std::hex <<
			writeAddress << " data = 0x" << data << std::endl;
	}
}

void SMC::initComPort()
{
	// First create the overlapped event.
	comOvEvent.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!comOvEvent.hEvent)
	{
		printf("Error creating overlapped event %d.\n", GetLastError());
		return;
	}

	// Defaulting to COM2 ATM.
	// Open COM# port using the CreateFile function and overlapped behavior.
	comPortHandle = CreateFile(TEXT("\\\\.\\COM2"), GENERIC_READ | GENERIC_WRITE, 
		0, NULL, OPEN_EXISTING, 0, NULL);
	
	if (comPortHandle == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with error %d.\n", GetLastError());
		return;
	}

	//  Initialize the DCB structure.
	SecureZeroMemory(&comPortDCB, sizeof(DCB));
	comPortDCB.DCBlength = sizeof(DCB);
	if (!GetCommState(comPortHandle, &comPortDCB))
	{
		printf("GetCommState failed with error %d.\n", GetLastError());
	}

	//  Print some of the DCB structure values
	_tprintf(TEXT("\n COM2 Opened: BaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n"),
		comPortDCB.BaudRate,
		comPortDCB.ByteSize,
		comPortDCB.Parity,
		comPortDCB.StopBits);
	printf("To change current COM port number please edit the source code in SMC.cpp.\n");
	// Set the default comm state.
	comPortDCB.BaudRate = CBR_115200;     //  baud rate
	comPortDCB.ByteSize = 8;             //  data size, xmit and rcv
	comPortDCB.Parity = NOPARITY;      //  parity bit
	comPortDCB.StopBits = ONESTOPBIT;    //  stop bit

	if (!SetCommState(comPortHandle, &comPortDCB))
	{
		printf("SetCommState failed with error %d.\n", GetLastError());
	}

	// Setup comm events
	u32 comPortEventMask = EV_ERR;
	if (!SetCommMask(comPortHandle, comPortEventMask))
	{
		printf("SetCommMask failed with error %d.\n", GetLastError());
		return;
	}

	if (!ClearCommError(comPortHandle, &comErr, &comStat))
	{
		printf("ClearCommError failed with error %d.\n", GetLastError());
		return;
	}
	comPortInitialized = true;
	//smcThread = std::thread(&SMC::smcLoop, this);
}

void SMC::uartWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	u32 writtenData = 0x0;
	memcpy(&writtenData, &data, byteCount);
	const char outdata = static_cast<const char>(writtenData);

	if (!comPortInitialized)
	{
		initComPort();
	}

	switch (writeAddress)
	{
	case UART_CONFIG_ADDR:
		std::cout << "SMC: UART Config set to: ";
		switch (writtenData)
		{
		case 0x1e6:
			std::cout << "115200,8,N,1" << std::endl;
			comPortDCB.BaudRate = CBR_115200;     //  baud rate
			comPortDCB.ByteSize = 8;             //  data size, xmit and rcv
			comPortDCB.Parity = NOPARITY;      //  parity bit
			comPortDCB.StopBits = ONESTOPBIT;    //  stop bit

			if (!SetCommState(comPortHandle, &comPortDCB))
			{
				printf("SetCommState failed with error %d.\n", GetLastError());
			}
			GetCommState(comPortHandle, &comPortDCB);
			break;
		case 0x1bb2:
			std::cout << "38400,8,N,1" << std::endl;
			break;
		case 0x163:
			std::cout << "19200,8,N,1" << std::endl;
			break;
		default:
			std::cout << "Unknown config value!" << std::endl;
			break;
		}
		uartStatus = 8;
		return;
		break;
	case UART_BYTE_OUT_ADDR:
		//uartDataStr.push_back((char)writtenData);
		//std::cout << (char)writtenData;
		retVal = WriteFile(comPortHandle, &data, 1, &byteswritten, NULL);
		break;
	default:
		std::cout << "SMC > UART: Write to unknown address!" << std::endl;
		break;
	}
}

void SMC::uartRead(u64 readAddress, u64* data, u8 byteCount)
{
	switch (readAddress)
	{
	case UART_CONFIG_ADDR:
		*data = 0x1e6;
		break;
	case UART_STATUS_ADDR:
		// Software reads to this register in order to know when a UART
		// transfer has finished.
		ClearCommError(comPortHandle, &comErr, &comStat);
		if (comStat.cbInQue > 0)
		{
			uartStatus = 1; // Got something to read in the input queue.
		}
		else
		{
			uartStatus = 2;
		}
		memcpy(data, &uartStatus, byteCount);
		break;
	case UART_BYTE_IN_ADDR:
		// Future input, for now we don't return nothing.
		retVal = ReadFile(comPortHandle, data, 1, &byteswritten, NULL);
		//std::cout << (u8)data;// << std::endl;
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
				std::cout << "SMC: SMC_QUERY_RTC, returning 0."
					<< std::endl;
				fifoReadedMsg[0] = SMC_QUERY_RTC;
				fifoReadedMsg[1] = 0;
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
				std::cout << "SMC: SMC_QUERY_AVPACK returning 0x5c."
					<< std::endl;
				fifoReadedMsg[0] = SMC_QUERY_AVPACK;
				fifoReadedMsg[1] = 0x1F; // HDMI NO Audio
				break;
			case SMC_I2C_READ_WRITE:				
				switch (fifoWrittenMsg[1])
				{
				case 0x10: // SMC_READ_ANA
					//std::cout << "SMC: ANA READ" << std::endl;
					fifoReadedMsg[0] = SMC_I2C_READ_WRITE;
					fifoReadedMsg[1] = 0x0;
					fifoReadedMsg[4] = (HANA_State[fifoWrittenMsg[6]] & 0xFF);
					fifoReadedMsg[5] = ((HANA_State[fifoWrittenMsg[6]] >> 8) & 0xFF);
					fifoReadedMsg[6] = ((HANA_State[fifoWrittenMsg[6]] >> 16) & 0xFF);
					fifoReadedMsg[7] = ((HANA_State[fifoWrittenMsg[6]] >> 24) & 0xFF);
					break;
				case 0x60: // SMC_WRITE_ANA
					std::cout << "SMC: ANA WRITE" << std::endl;
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
			// Clear the written mesage position counter.
			currentWritePos = 0;
			// Send Interrupt?
			intPending = 0x10000000; // Interrupt pending dispatch. If this register contains this value then the kernel issues a dpc routine for reading the smc response.
			parentBus->RouteInterrupt(PRIO_SMM);
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

void SMC::smcLoop()
{
	while (true)
	{
		// Clock tick.
		smcClockTick();
	}
}

void SMC::smcClockTick()
{
	// Check for SMC Clock interrupt register? Software writes here after a ClockInterrupt is taken.
	if (smcState.clockIntReg == 1)
	{
		parentBus->RouteInterrupt(PRIO_CLOCK);
		smcState.clockIntReg = 0;
	}
	// Need to find out what's the best delay here.
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
