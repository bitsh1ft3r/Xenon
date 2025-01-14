#include "SMC.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/SMC_Config.h"
#include "Xenon/Core/RootBus/HostBridge/PCIBridge/SMC/HANA_State.h"

//
// Registers Offsets
//

// UART Region
#define UART_BYTE_OUT_REG		0x10
#define UART_BYTE_IN_REG		0x14
#define UART_STATUS_REG			0x18
#define UART_CONFIG_REG			0x1C

// SMI Region
#define SMI_INT_STATUS_REG		0x50
#define SMI_INT_ACK_REG			0x58
#define SMI_INT_ENABLED_REG		0x5C

// Clock Region
#define CLCK_INT_ENABLED_REG	0x64
#define CLCK_INT_STATUS_REG		0x6C

// FIFO Region
#define FIFO_IN_DATA_REG		0x80
#define FIFO_IN_STATUS_REG		0x84
#define FIFO_OUT_DATA_REG		0x90
#define FIFO_OUT_STATUS_REG		0x94

//
// UART Definitions
//
#define UART_STATUS_EMPTY		0x2
#define UART_STATUS_DATA_PRES	0x1

//
// FIFO Definitions
//
#define FIFO_STATUS_READY		0x4
#define FIFO_STATUS_BUSY		0x0

//
// SMI Definitions
//
#define SMI_INT_ENABLED			0xC
#define SMI_INT_NONE			0x0
#define SMI_INT_PENDING			0x10000000

//
// Clock Definitions
//
#define CLCK_INT_ENABLED		0x10000000
#define CLCK_INT_READY			0x1
#define CLCK_INT_TAKEN			0x3

// Class Constructor.
Xe::PCIDev::SMC::SMCCore::SMCCore(PCIBridge* parentPCIBridge, SMC_CORE_STATE* newSMCCoreState)
{
	std::cout << "SMCCore: Initializing." << std::endl;

	// Assign our parent PCI Bus Ptr.
	pciBridge = parentPCIBridge;

	// Assign our core sate, this is already filled with config data regarding AVPACK, PWRON Reason and TrayState.	
	smcCoreState = newSMCCoreState;

	// Create a new SMC PCI State
	smcPCIState = new SMC_PCI_STATE;
	memset(smcPCIState, 0, sizeof(SMC_PCI_STATE));
	
	// Set UART Status to Empty.
	smcPCIState->uartStatusReg = UART_STATUS_EMPTY;

	// Set PCI Config Space registers.
	u8 i = 0;
	for (u16 idx = 0; idx < 256; idx += 4)
	{
		memcpy(&pciConfigSpace.data[idx], &smcConfigSpaceMap[i], 4);
		i++;
	}
	
	// Enter main execution thread.
	smcThread = std::thread(&SMCCore::smcMainThread, this);
}

// Class Destructor.
Xe::PCIDev::SMC::SMCCore::~SMCCore()
{
	std::cout << "SMCCore: Exiting." << std::endl;
}

// PCI Read
void Xe::PCIDev::SMC::SMCCore::Read(u64 readAddress, u64* data, u8 byteCount)
{
	u8 regOffset = static_cast<u8>(readAddress);
	
	switch (regOffset)
	{
	case UART_CONFIG_REG:			// UART Config Register
		memcpy(data, &smcPCIState->uartConfigReg, byteCount);
		break;
	case UART_BYTE_OUT_REG:			// UART Data Out Register
		smcCoreState->retVal = ReadFile(smcCoreState->comPortHandle, &smcPCIState->uartOutReg, 1,
			&smcCoreState->currentBytesReadCount, NULL);
		memcpy(data, &smcPCIState->uartOutReg, byteCount);
		break;
	case UART_STATUS_REG:			// UART Status Register
		// First lets check if the UART has already been setup, if so, proceed to do the TX/RX.
		if (smcCoreState->uartInitialized)
		{
			// Get current COM Port Status
			ClearCommError(smcCoreState->comPortHandle, &smcCoreState->comPortError, &smcCoreState->comPortStat);
			if (smcCoreState->comPortStat.cbInQue > 0) // The queue has any bytes remaining?
			{
				// Got something to read in the input queue.
				smcPCIState->uartStatusReg = UART_STATUS_DATA_PRES;
			}
			else
			{
				// The input queue is empty.
				smcPCIState->uartStatusReg = UART_STATUS_EMPTY;
			}
		}
		else
		{
			// XeLL doesn't initialize UART before sending data trough it. Initialize it first then.
			setupUART(0x1e6); // 115200,8,N,1.
		}
		memcpy(data, &smcPCIState->uartStatusReg, byteCount);
		break;
	case SMI_INT_STATUS_REG:		// SMI INT Status Register
		memcpy(data, &smcPCIState->smiIntPendingReg, byteCount);
		break;
	case SMI_INT_ACK_REG:			// SMI INT ACK Register
		memcpy(data, &smcPCIState->smiIntAckReg, byteCount);
		break;
	case SMI_INT_ENABLED_REG:		// SMI INT Enabled Register
		memcpy(data, &smcPCIState->smiIntEnabledReg, byteCount);
		break;
	case FIFO_IN_STATUS_REG:		// FIFO In Status Register
		memcpy(data, &smcPCIState->fifoInStatusReg, byteCount);
		break;
	case FIFO_OUT_STATUS_REG:		// FIFO Out Status Register
		memcpy(data, &smcPCIState->fifoOutStatusReg, byteCount);
		break;
	case FIFO_OUT_DATA_REG:			// FIFO Data Out Register
		// Copy the data to our input buffer.
		memcpy(data ,&smcCoreState->fifoDataBuffer[smcCoreState->fifoBufferPos], byteCount);
		smcCoreState->fifoBufferPos += 4;
		break;
	default:
		std::cout << "[WARN] SMCCore: Unknown register being read, offset 0x" 
			<< static_cast<u16>(regOffset) << std::endl;
		break;
	}
}

// PCI Config Read
void Xe::PCIDev::SMC::SMCCore::ConfigRead(u64 readAddress, u64* data, u8 byteCount)
{
	memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

// PCI Write
void Xe::PCIDev::SMC::SMCCore::Write(u64 writeAddress, u64 data, u8 byteCount)
{
	u8 regOffset = static_cast<u8>(writeAddress);
	
	switch (regOffset)
	{
	case UART_CONFIG_REG:		// UART Config Register
		memcpy(&smcPCIState->uartConfigReg, &data, byteCount);
		// Check if UART is already initialized.
		if (!smcCoreState->uartInitialized)
		{
			// Initialize UART.
			setupUART(static_cast<u32>(data));
		}
		break;
	case UART_BYTE_IN_REG:		// UART Data In Register
		memcpy(&smcPCIState->uartInReg, &data, byteCount);
		// Write the data out.
		smcCoreState->retVal = WriteFile(smcCoreState->comPortHandle, &data, 1,
			&smcCoreState->currentBytesWrittenCount, NULL);
		break;
	case SMI_INT_STATUS_REG:	// SMI INT Status Register
		memcpy(&smcPCIState->smiIntPendingReg, &data, byteCount);
		break;
	case SMI_INT_ACK_REG:		// SMI INT ACK Register
		memcpy(&smcPCIState->smiIntAckReg, &data, byteCount);
		break;
	case SMI_INT_ENABLED_REG:	// SMI INT Enabled Register
		memcpy(&smcPCIState->smiIntEnabledReg, &data, byteCount);
		break;
	case CLCK_INT_ENABLED_REG:	// Clock INT Enabled Register
		memcpy(&smcPCIState->clockIntEnabledReg, &data, byteCount);
		break;
	case CLCK_INT_STATUS_REG:	// Clock INT Status Register
		memcpy(&smcPCIState->clockIntStatusReg, &data, byteCount);
		break;
	case FIFO_IN_STATUS_REG:	// FIFO In Status Register
		smcPCIState->fifoInStatusReg = static_cast<u32>(data);
		if (data == FIFO_STATUS_READY) // We're about to receive a message.
		{
			// Reset our input buffer and buffer pointer.
			memset(&smcCoreState->fifoDataBuffer, 0, 16);
			smcCoreState->fifoBufferPos = 0;
		}
		break;
	case FIFO_OUT_STATUS_REG:	// FIFO Out Status Register
		smcPCIState->fifoOutStatusReg = static_cast<u32>(data);
		if (data == FIFO_STATUS_READY) // We're about to send a reply.
		{
			// Reset our FIFO buffer pointer.
			smcCoreState->fifoBufferPos = 0;
		}
		break;
	case FIFO_IN_DATA_REG:		// FIFO Data In Register
		// Copy the data to our input buffer at current position and increse buffer pointer position.
		memcpy(&smcCoreState->fifoDataBuffer[smcCoreState->fifoBufferPos], &data, byteCount);
		smcCoreState->fifoBufferPos += 4;
		break;
	default:
		std::cout << "[WARN] SMCCore: Unknown register being written, offset 0x" 
			<< static_cast<u16>(regOffset) << ", data 0x" << data << std::endl;
		break;
	}
}

// PCI Config Write
void Xe::PCIDev::SMC::SMCCore::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount)
{
	memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}

// Setups the UART Communication at a given configuration.
void Xe::PCIDev::SMC::SMCCore::setupUART(u32 uartConfig)
{
#ifdef _WIN32
	// Windows Init Code
	std::cout << "[INFO] SMCCore: Initializing UART:" << std::endl;

	//  Initialize the DCB structure.
	SecureZeroMemory(&smcCoreState->comPortDCB, sizeof(DCB));
	smcCoreState->comPortDCB.DCBlength = sizeof(DCB);
	switch (uartConfig)
	{
	case 0x1e6:
		std::cout << "\t* BaudRate: 115200bps, DataSize: 8, Parity: N, StopBits: 1" << std::endl;
		smcCoreState->comPortDCB.BaudRate = CBR_115200;
		smcCoreState->comPortDCB.ByteSize = 8;
		smcCoreState->comPortDCB.Parity = NOPARITY;
		smcCoreState->comPortDCB.StopBits = ONESTOPBIT;
		break;
	case 0x1bb2:
		std::cout << "\t* BaudRate: 38400bps, DataSize: 8, Parity: N, StopBits: 1" << std::endl;
		smcCoreState->comPortDCB.BaudRate = CBR_38400;
		smcCoreState->comPortDCB.ByteSize = 8;
		smcCoreState->comPortDCB.Parity = NOPARITY;
		smcCoreState->comPortDCB.StopBits = ONESTOPBIT;
		break;
	case 0x163:
		std::cout << "\t* BaudRate: 19200bps, DataSize: 8, Parity: N, StopBits: 1" << std::endl;
		smcCoreState->comPortDCB.BaudRate = CBR_19200;
		smcCoreState->comPortDCB.ByteSize = 8;
		smcCoreState->comPortDCB.Parity = NOPARITY;
		smcCoreState->comPortDCB.StopBits = ONESTOPBIT;
		break;
	default:
		std::cout << "[WARN] SMCCore: Unknown UART config being set: ConfigValue = 0x" << uartConfig << std::endl;
		break;
	}

	// Open COM# port using the CreateFile function.
	smcCoreState->comPortHandle = CreateFile(TEXT("\\\\.\\COM2"), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);

	if (smcCoreState->comPortHandle == INVALID_HANDLE_VALUE)
	{
		printf("[ERR] SMCCore: CreateFile failed with error %d. Make sure the Selected COM " 
			"Port is availeable in your system.\n", GetLastError());
		return;
	}

	// Get Current COM Port State
	if (!GetCommState(smcCoreState->comPortHandle, &smcCoreState->comPortDCB))
	{
		printf("[ERR] SMCCore: UART: GetCommState failed with error %d.\n", GetLastError());
	}
	// Set The COM Port State as per config value.
	if (!SetCommState(smcCoreState->comPortHandle, &smcCoreState->comPortDCB))
	{
		printf("[ERR] SMCCore: UART: SetCommState failed with error %d.\n", GetLastError());
	}


	std::cout << "[INFO] SMCCore: UART Initialized Successfully." << std::endl;

	// Everything OK.
	smcCoreState->uartInitialized = true;

#elif
	std::cout << "[ERR] SMCCore: UART Initialization is not supported on this platform!" << std::endl;
#endif// _WIN32
}

// SMC Main Thread
void Xe::PCIDev::SMC::SMCCore::smcMainThread()
{
	// Set FIFO_IN_STATUS_REG to FIFO_STATUS_READY to indicate we are ready to receive a message.
	smcPCIState->fifoInStatusReg = FIFO_STATUS_READY;

	// Timer for measuring elapsed time since last Clock Interrupt.
	std::chrono::steady_clock::time_point timerStart = std::chrono::steady_clock::now();

	while (true)
	{
		// The System Management Controller (SMC) does the following:
		// * Communicates over a FIFO Queue with the kernel to execute commands and provide system info.
		// * Does the UART/Serial communication between the console and remote Serial Device/PC.
		// * Ticks the clock and sends an interrupt (PRIO_CLOCK) every x milliseconds.

		// Core State (PowerOn Cause, SMC Ver, FAN Speed, Temps, etc...) should be already set.

		/*
			1. FIFO communication.
		*/

		// This is done in simple steps:
		
		/* Message Write (System -> SMC) */

		// 1. System reads FIFO_IN_STATUS_REG to check wheter the SMC is ready to receive a command.
		// 2. If the status is FIFO_STATUS_READY (0x4), the System proceeds, else it loops until the SMC Input Status
		// Register is set to FIFO_STATUS_READY.
		// 3. System then does a write to FIFO_IN_STATUS_REG setting it to FIFO_STATUS_READY. This signals the SMC
		// that a new message/command is about to receive.
		// 4. System does 4 32 Bit writes to FIFO_IN_DATA_REG, this is our 16 Bytes message.
		// 5. System then does a write to FIFO_IN_STATUS_REG setting it to FIFO_STATUS_BUSY. This Signals the SMC that
		// the message is transmitted and that it should start message processing.
		// 6. If SMM (System Management Mode) interrupts are enabled, the SMC changes the SMI_INT_PENDING_REG to 
		// SMI_INT_PENDING and issues one signaling the System it should read the message. It also sets the 
		// FIFO_OUT_STATUS_REG to FIFO_STATUS_READY.
		
		/* Message Read (SMC -> System) */

		// Reads Proceed as following:
		// A. Asynchronous Mode (Interrupts Enabled):
		// 1. If an interrupt was issued (Asynchronous Mode), System reads SMI_INT_STATUS_REG to check wheter an
		// interrupt is pending(SMI_INT_PENDING).
		// 2. If SMI_INT_STATUS_REG == SMI_INT_PENDING, then a DPC routine is invoked in order to read the response
		// and the SMI_INT_ACK_REG is set to 0. Else it just continues normal kernel execution.
		
		// B. Synchronous Mode (Interrupts Disabled):
		
		// 1. System reads FIFO_OUT_STATUS_REG to check wheter the SMC has finished processing the command. If the 
		// status is FIFO_STATUS_READY (0x4), the System proceeds, else it loops until the FIFO_OUT_STATUS_REG is
		// set to FIFO_STATUS_READY.

		// The process afterwards in both cases is the same as when the system does a command write. The diffrence
		// resides on the Registers being used, using FIFO_OUT_STATUS_REG instead of FIFO_IN_STATUS_REG and 
		// FIFO_OUT_DATA_REG instead of FIFO_IN_DATA_REG.

		// Check wheter we've received a command. If so, process it.
		// Software sets FIFO_IN_STATUS_REG to FIFO_STATUS_BUSY after it has finished sending a command.
		if (smcPCIState->fifoInStatusReg == FIFO_STATUS_BUSY)
		{
			// This is set first as software waits for this register to become Ready in order to read a reply.
			// Set FIFO_OUT_STATUS_REG to FIFO_STATUS_BUSY
			smcPCIState->fifoOutStatusReg = FIFO_STATUS_BUSY;

			// Set FIFO_IN_STATUS_REG to FIFO_STATUS_READY
			smcPCIState->fifoInStatusReg = FIFO_STATUS_READY;

			// Note that the first byte in the response is always Command ID.

			switch (smcCoreState->fifoDataBuffer[0]) // Data Buffer[0] is our message ID.
			{
			case Xe::PCIDev::SMC::SMC_PWRON_TYPE:
				// Zero out the buffer
				memset(&smcCoreState->fifoDataBuffer, 0, 16);
				smcCoreState->fifoDataBuffer[0] = SMC_PWRON_TYPE;	
				smcCoreState->fifoDataBuffer[1] = smcCoreState->currPowerOnReas;
				break;
			case Xe::PCIDev::SMC::SMC_QUERY_RTC:
				// Zero out the buffer
				memset(&smcCoreState->fifoDataBuffer, 0, 16);
				smcCoreState->fifoDataBuffer[0] = SMC_QUERY_RTC;
				smcCoreState->fifoDataBuffer[1] = 0;
				break;
			case Xe::PCIDev::SMC::SMC_QUERY_TEMP_SENS:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_QUERY_TEMP_SENS" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_QUERY_TRAY_STATE:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_QUERY_TRAY_STATE" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_QUERY_AVPACK:
				smcCoreState->fifoDataBuffer[0] = SMC_QUERY_AVPACK;
				smcCoreState->fifoDataBuffer[1] = smcCoreState->currAVPackType;
				break;
			case Xe::PCIDev::SMC::SMC_I2C_READ_WRITE:
				switch (smcCoreState->fifoDataBuffer[1])
				{
				case 0x10: // SMC_READ_ANA
					smcCoreState->fifoDataBuffer[0] = SMC_I2C_READ_WRITE;
					smcCoreState->fifoDataBuffer[1] = 0x0;
					smcCoreState->fifoDataBuffer[4] = (HANA_State[smcCoreState->fifoDataBuffer[6]] & 0xFF);
					smcCoreState->fifoDataBuffer[5] = ((HANA_State[smcCoreState->fifoDataBuffer[6]] >> 8) & 0xFF);
					smcCoreState->fifoDataBuffer[6] = ((HANA_State[smcCoreState->fifoDataBuffer[6]] >> 16) & 0xFF);
					smcCoreState->fifoDataBuffer[7] = ((HANA_State[smcCoreState->fifoDataBuffer[6]] >> 24) & 0xFF);
					break;
				case 0x60: // SMC_WRITE_ANA
					smcCoreState->fifoDataBuffer[0] = SMC_I2C_READ_WRITE;
					smcCoreState->fifoDataBuffer[1] = 0x0;
					HANA_State[smcCoreState->fifoDataBuffer[6]] = smcCoreState->fifoDataBuffer[4]
						| (smcCoreState->fifoDataBuffer[5] << 8)
						| (smcCoreState->fifoDataBuffer[6] << 16)
						| (smcCoreState->fifoDataBuffer[7] << 24);
					break;
				default:
					std::cout << "[WARN] SMCCore: SMC_I2C_READ_WRITE: Unimplemented command 0x"
						<< static_cast<u16>(smcCoreState->fifoDataBuffer[1]) << std::endl;
					smcCoreState->fifoDataBuffer[0] = SMC_I2C_READ_WRITE;
					smcCoreState->fifoDataBuffer[1] = 0x1; // Set R/W Failed.
				}
				break;
			case Xe::PCIDev::SMC::SMC_QUERY_VERSION:
				smcCoreState->fifoDataBuffer[0] = SMC_QUERY_VERSION;
				smcCoreState->fifoDataBuffer[1] = 0x41;
				smcCoreState->fifoDataBuffer[2] = 0x02;
				smcCoreState->fifoDataBuffer[3] = 0x03;
				break;
			case Xe::PCIDev::SMC::SMC_FIFO_TEST:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_FIFO_TEST" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_QUERY_IR_ADDRESS:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_QUERY_IR_ADDRESS" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_QUERY_TILT_SENSOR:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_QUERY_TILT_SENSOR" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_READ_82_INT:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_READ_82_INT" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_READ_8E_INT:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_READ_8E_INT" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_STANDBY:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_STANDBY" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_TIME:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_TIME" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_FAN_ALGORITHM:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_FAN_ALGORITHM" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_FAN_SPEED_CPU:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_FAN_SPEED_CPU" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_DVD_TRAY:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_DVD_TRAY" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_POWER_LED:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_POWER_LED" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_AUDIO_MUTE:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_AUDIO_MUTE" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_ARGON_RELATED:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_ARGON_RELATED" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_FAN_SPEED_GPU:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_FAN_SPEED_GPU" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_IR_ADDRESS:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_IR_ADDRESS" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_DVD_TRAY_SECURE:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_DVD_TRAY_SECURE" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_FP_LEDS:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_FP_LEDS" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_RTC_WAKE:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_RTC_WAKE" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_ANA_RELATED:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_ANA_RELATED" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_ASYNC_OPERATION:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_ASYNC_OPERATION" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_82_INT:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_82_INT" << std::endl;
				break;
			case Xe::PCIDev::SMC::SMC_SET_9F_INT:
				std::cout << "[WARN] SMCCore: Unimplemented SMC_FIFO_CMD: SMC_SET_9F_INT" << std::endl;
				break;
			default:
				std::cout << "[WARN] SMCCore: Unknown SMC_FIFO_CMD: ID = 0x" 
					<< static_cast<u16>(smcCoreState->fifoDataBuffer[0]) << std::endl;
				break;
			}

			// Set FIFO_OUT_STATUS_REG to FIFO_STATUS_READY, signaling we're ready to transmit a response.
			smcPCIState->fifoOutStatusReg = FIFO_STATUS_READY;

			// If interrupts are active set Int status and issue one.
			if (smcPCIState->smiIntEnabledReg & SMI_INT_ENABLED)
			{
				smcPCIState->smiIntPendingReg = SMI_INT_PENDING;
				pciBridge->RouteInterrupt(PRIO_SMM);
			}
		}

		// Measure elapsed time
		std::chrono::steady_clock::time_point timerNow = std::chrono::steady_clock::now();

		// Check for SMC Clock interrupt register.
		if (smcPCIState->clockIntEnabledReg == CLCK_INT_ENABLED) // Clock Int Enabled.
		{
			if (smcPCIState->clockIntStatusReg == CLCK_INT_READY) // Clock Interrupt Not Taken.
			{
				// Wait X time before next clock interrupt. TODO: Find the correct delay.
				if (timerNow >= timerStart + std::chrono::milliseconds(500))
				{
					// Update internal timer.
					timerStart = std::chrono::steady_clock::now();
					smcPCIState->clockIntStatusReg = CLCK_INT_TAKEN;
					pciBridge->RouteInterrupt(PRIO_CLOCK);
				}
			}
		}
	}
}