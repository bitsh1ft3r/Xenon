// Copyright 2025 Xenon Emulator Project

#include "ODD.h"

//
// ATAPI Registers Offsets
//

// Communication with disk drive controllers is achieved via I/O registers.
// Registers and their offsets relative to the base
// address of command block registers and the base address of control block
// registers.

// Registers Offsets from Command Block

// Data Reg (Read/Write)
#define ATAPI_REG_DATA 0x0
// Error Reg (Read)
#define ATAPI_REG_ERROR 0x1
// Features Reg (Write)
#define ATAPI_REG_FEATURES 0x1
// Interrupt Reason Reg (Read)
#define ATAPI_REG_INT_REAS 0x2
// Sector Count Reg (Write)
#define ATAPI_REG_SECTOR_COUNT 0x2
// LBA Low Reg (Read/Write)
#define ATAPI_REG_LBA_LOW 0x3
// Byte Count Low Reg (Read/Write)
#define ATAPI_REG_BYTE_COUNT_LOW 0x4
// Byte Count High Reg (Read/Write)
#define ATAPI_REG_BYTE_COUNT_HIGH 0x5
// Device Reg (Read/Write)
#define ATAPI_REG_DEVICE 0x6
// Status Reg (Read)
#define ATAPI_REG_STATUS 0x7
// Command Reg (Write)
#define ATAPI_REG_COMMAND 0x7

// Registers Offsets from Control Block

// Direct Memory Access Command
#define ATAPI_DMA_REG_COMMAND 0x0
// Direct Memory Access Status
#define ATAPI_DMA_REG_STATUS 0x2
// Direct Memory Access Table Offset
#define ATAPI_DMA_REG_TABLE_OFFSET 0x4
// Alternate Status Reg (Read)
#define ATAPI_REG_ALTERNATE_STATUS 0x6
// Device Control Reg (Write)
#define ATAPI_REG_DEVICE_CONTROL 0x6

//
//  DMA Registers Bitmasks
//

#define XE_ATAPI_DMA_ACTIVE 0x1
#define XE_ATAPI_DMA_ERR 0x2
#define XE_ATAPI_DMA_INTR 0x4
#define XE_ATAPI_DMA_WR 0x8

//
// ATA Status Register
//

/*
    This register contains the current status of the drive. If the BSY bit is 0,
   the other bits of the register contain valid information; otherwise the other
   bits do not contain valid information. If this register is read by the host
   computer during a pending interrupt, the interrupt condition is cleared.

    Bits 1 & 2 are undefined
    Bit 4 is Command Specific
*/

/*
    Bit 0 (ERR / CHK – Error / Check) is defined as ERR for all commands except
   for the Packet and Service commands, for which this bit is defined as CHK.
*/
#define ATA_STATUS_ERR_CHK 0x1
/*
    Bit 3 (DRQ – Data Request) indicates by value 1 that the disk drive is ready
   to transfer data between the host computer and the drive. After the computer
   writes a commmand code to the Command register, the drive sets the BSY bit or
   the DRQ bit to 1 until command completion.
*/
#define ATA_STATUS_DRQ 0x08
/*
    Bit 5 (DF – Device Fault) indicates by value 1 that a device fault has been
   detected.
*/
#define ATA_STATUS_DF 0x10
/*
    Bit 6 (DRDY – Device Ready) is set to 1 to indicate that the disk drive
   accepts commands. If the DRDY bit is 0, the drive will accept and attempt to
   execute the Device Reset and Execute Device Diagnostic commands. Other
   commands will not be accepted, and the drive will set the ABRT bit in the
   Error register and the ERR/CHK bit in the Status register, before resetting
   the BSY bit to indicate completion of the command.
*/
#define ATA_STATUS_DRDY 0x40
/*
    Bit 7 (BSY – Busy) is set to 1 whenever the disk drive has control of the
   Command Block registers. If the BSY bit is 1, a write to any Command Block
   register by the host computer will be ignored by the drive. The BSY bit is
   cleared to 0 by the drive at command completion and after setting the DRQ
   status bit to 1 to indicate the device is ready to transfer data.
*/
#define ATA_STATUS_BSY 0x80

//
// ATA Data Register
//

/*
    This is a 32-bit register and is used for reading or writing the data during
   data transfers. This register shall be accessed for data transfers in PIO
   mode only when the DRQ bit of the Status register is set to 1.
*/

//
// ATA Error Register
//

/*
    This register contains the status of the last command executed by the disk
   drive or a diagnostic code. At completion of any command except the Execute
   Device Diagnostic and Device Reset commands, the contents of this register
   are valid when the BSY and DRQ bits of the Status register are cleared to 0
   and the ERR/CHK bit in the same register is set to 1. At completion of an
   Execute Device Diagnostic or Device Reset command and after a hardware or
   software reset, this register contains a diagnostic code. Except for bit 2
   (ABRT), the meaning of other bits of the Error register varies depending on
    the command that has been executed.
*/

/*
    Bit 2 (ABRT – Command Aborted) indicates by value 1 that the requested
   command has been aborted because the command code or a command parameter is
   invalid, the command is not implemented, or some other error has occurred.
*/
#define ATA_ERROR_ABRT 0x4

//
// ATA Device Register
//

/*
    This register is used for selecting the disk drive. The register shall be
   written only when the BSY and DRQ bits of the Status register are both 0. The
   contents of this register are valid only when the BSY bit of the Status
   register is 0. Except the DEV bit, all other bits of this register become a
   command parameter when the command code is written into the Command register

     Bits 3..0 are command specific.
     Bit 7 and bit 5 are undefined.
*/

/*
    Bit 4 (DEV – Device Select) selects by value 0 the drive 0, and by value 1
   the drive 1.
*/
#define ATA_DEV_DEV 0x8
/*
    Bit 6 (LBA) selects the sector addressing mode. Some commands require to set
   this bit to 1 to select LBA addressing. If this bit is cleared to 0, the CHS
   addressing is selected
*/
#define ATA_DEV_LBA 0x20

//
// ATA Command Register
//

/*
    This register contains the command code to be sent to the disk drive.
   Command execution begins immediately after the command code is written into
   the Command register. The contents of the Command Block registers become
   parameters of the command when this register is written. Writing this
   register clears any pending interrupt condition.
*/

//
// ATA Alternate Status Register
//

/*
    This register contains the same information as the Status register. The only
   difference is that reading the Alternate Status register does not imply an
   interrupt acknowledgement or clearing of the interrupt condition
*/

//
// ATA Device Control Register Register
//

/*
    This register allows the host computer to perform a software reset of the
   disk drives and to enable or disable the assertion of the INTRQ interrupt
   signal by the selected drive. When the Device Control register is written,
   both drives respond to the write regardless of which drive is selected.

     Bit 0 shall be cleared to 0.
     Bits 6..3 are reserved.
*/

/*
    Bit 1 (nIEN – INTRQ Enable) enables by value 0 the assertion of the INTRQ
   interrupt request signal by the disk drive.
*/
#define ATA_DEVICE_CONTROL_NIEN 0x02
/*
    Bit 2 (SRST – Software Reset) is the software reset bit of the disk drives.
   If there are two daisy-chained drives, by setting this bit to 1 both drives
   are reset.
*/
#define ATA_DEVICE_CONTROL_SRST 0x04
/*
    Bit 7 (HOB – High Order Byte) is defined only when the 48-bit LBA addressing
   is implemented. If this bit is set to 1, reading of the Features register,
   the Sector Count register, and the LBA address registers is performed from
   the “previous content” location. If the HOB bit is set to 0, reading is
   performed from the “most recently written” location. Writing to any Command
   Block register has the effect of resetting the HOB bit to 0.
*/
#define ATA_DEVICE_CONTROL_HOB 0x80

//
// Command Descriptor Block for SCSI Commands
//

// The CDB on the Xenon ODD is 12 bytes in size
#define XE_ATAPI_CDB_SIZE 12

//
// ATA Commands
//

#define ATA_COMMAND_DEVICE_RESET 0x08
#define ATA_COMMAND_READ_SECTORS 0x20
#define ATA_COMMAND_READ_DMA_EXT 0x25
#define ATA_COMMAND_WRITE_SECTORS 0x30
#define ATA_COMMAND_WRITE_DMA_EXT 0x35
#define ATA_COMMAND_VERIFY 0x40
#define ATA_COMMAND_VERIFY_EXT 0x42
#define ATA_COMMAND_SET_DEVICE_PARAMETERS 0x91
#define ATA_COMMAND_PACKET 0xA0
#define ATA_COMMAND_IDENTIFY_PACKET_DEVICE 0xA1
#define ATA_COMMAND_READ_MULTIPLE 0xC4
#define ATA_COMMAND_WRITE_MULTIPLE 0xC5
#define ATA_COMMAND_SET_MULTIPLE_MODE 0xC6
#define ATA_COMMAND_READ_DMA 0xC8
#define ATA_COMMAND_WRITE_DMA 0xCA
#define ATA_COMMAND_STANDBY_IMMEDIATE 0xE0
#define ATA_COMMAND_FLUSH_CACHE 0xE7
#define ATA_COMMAND_IDENTIFY_DEVICE 0xEC
#define ATA_COMMAND_SET_FEATURES 0xEF
#define ATA_COMMAND_SECURITY_SET_PASSWORD 0xF1
#define ATA_COMMAND_SECURITY_UNLOCK 0xF2
#define ATA_COMMAND_SECURITY_DISABLE_PASSWORD 0xF6

//
// IDE feature flags for an ATAPI device.
//

#define IDE_FEATURE_DMA 0x01
#define IDE_FEATURE_OVL 0x02

//
// IDE interrupt reason flags for an ATAPI device.
//

#define IDE_INTERRUPT_REASON_CD 0x01
#define IDE_INTERRUPT_REASON_IO 0x02

//
// Data transfer values for an ATAPI device.
//

#define ATAPI_CDROM_SECTOR_SIZE 2048

//
// Control and status flags for the DMA interface.
//

#define ATAPI_DMA_CONTROL_RUN 0x8000
#define ATAPI_DMA_CONTROL_PAUSE 0x4000
#define ATAPI_DMA_CONTROL_FLUSH 0x2000
#define ATAPI_DMA_CONTROL_WAKE 0x1000
#define ATAPI_DMA_CONTROL_DEAD 0x0800
#define ATAPI_DMA_CONTROL_ACTIVE 0x0400

//
// SCSI Command Descriptor Block Operation codes
//
#define SCSIOP_TEST_UNIT_READY 0x00
#define SCSIOP_REZERO_UNIT 0x01
#define SCSIOP_REWIND 0x01
#define SCSIOP_REQUEST_BLOCK_ADDR 0x02
#define SCSIOP_REQUEST_SENSE 0x03
#define SCSIOP_FORMAT_UNIT 0x04
#define SCSIOP_READ_BLOCK_LIMITS 0x05
#define SCSIOP_REASSIGN_BLOCKS 0x07
#define SCSIOP_INIT_ELEMENT_STATUS 0x07
#define SCSIOP_READ6 0x08
#define SCSIOP_RECEIVE 0x08
#define SCSIOP_WRITE6 0x0A
#define SCSIOP_PRINT 0x0A
#define SCSIOP_SEND 0x0A
#define SCSIOP_SEEK6 0x0B
#define SCSIOP_TRACK_SELECT 0x0B
#define SCSIOP_SLEW_PRINT 0x0B
#define SCSIOP_SEEK_BLOCK 0x0C
#define SCSIOP_PARTITION 0x0D
#define SCSIOP_READ_REVERSE 0x0F
#define SCSIOP_WRITE_FILEMARKS 0x10
#define SCSIOP_FLUSH_BUFFER 0x10
#define SCSIOP_SPACE 0x11
#define SCSIOP_INQUIRY 0x12
#define SCSIOP_VERIFY6 0x13
#define SCSIOP_RECOVER_BUF_DATA 0x14
#define SCSIOP_MODE_SELECT 0x15
#define SCSIOP_RESERVE_UNIT 0x16
#define SCSIOP_RELEASE_UNIT 0x17
#define SCSIOP_COPY 0x18
#define SCSIOP_ERASE 0x19
#define SCSIOP_MODE_SENSE 0x1A
#define SCSIOP_START_STOP_UNIT 0x1B
#define SCSIOP_STOP_PRINT 0x1B
#define SCSIOP_LOAD_UNLOAD 0x1B
#define SCSIOP_RECEIVE_DIAGNOSTIC 0x1C
#define SCSIOP_SEND_DIAGNOSTIC 0x1D
#define SCSIOP_MEDIUM_REMOVAL 0x1E
#define SCSIOP_READ_FORMATTED_CAPACITY 0x23
#define SCSIOP_READ_CAPACITY 0x25
#define SCSIOP_READ 0x28
#define SCSIOP_WRITE 0x2A
#define SCSIOP_SEEK 0x2B
#define SCSIOP_LOCATE 0x2B
#define SCSIOP_POSITION_TO_ELEMENT 0x2B
#define SCSIOP_WRITE_VERIFY 0x2E
#define SCSIOP_VERIFY 0x2F
#define SCSIOP_SEARCH_DATA_HIGH 0x30
#define SCSIOP_SEARCH_DATA_EQUAL 0x31
#define SCSIOP_SEARCH_DATA_LOW 0x32
#define SCSIOP_SET_LIMITS 0x33
#define SCSIOP_READ_POSITION 0x34
#define SCSIOP_SYNCHRONIZE_CACHE 0x35
#define SCSIOP_COMPARE 0x39
#define SCSIOP_COPY_COMPARE 0x3A
#define SCSIOP_WRITE_DATA_BUFF 0x3B
#define SCSIOP_READ_DATA_BUFF 0x3C
#define SCSIOP_CHANGE_DEFINITION 0x40
#define SCSIOP_READ_SUB_CHANNEL 0x42
#define SCSIOP_READ_TOC 0x43
#define SCSIOP_READ_HEADER 0x44
#define SCSIOP_PLAY_AUDIO 0x45
#define SCSIOP_PLAY_AUDIO_MSF 0x47
#define SCSIOP_PLAY_TRACK_INDEX 0x48
#define SCSIOP_PLAY_TRACK_RELATIVE 0x49
#define SCSIOP_PAUSE_RESUME 0x4B
#define SCSIOP_LOG_SELECT 0x4C
#define SCSIOP_LOG_SENSE 0x4D
#define SCSIOP_STOP_PLAY_SCAN 0x4E
#define SCSIOP_READ_DISK_INFORMATION 0x51
#define SCSIOP_READ_TRACK_INFORMATION 0x52
#define SCSIOP_MODE_SELECT10 0x55
#define SCSIOP_MODE_SENSE10 0x5A
#define SCSIOP_REPORT_LUNS 0xA0
#define SCSIOP_SEND_KEY 0xA3
#define SCSIOP_REPORT_KEY 0xA4
#define SCSIOP_MOVE_MEDIUM 0xA5
#define SCSIOP_LOAD_UNLOAD_SLOT 0xA6
#define SCSIOP_EXCHANGE_MEDIUM 0xA6
#define SCSIOP_SET_READ_AHEAD 0xA7
#define SCSIOP_READ_DVD_STRUCTURE 0xAD
#define SCSIOP_REQUEST_VOL_ELEMENT 0xB5
#define SCSIOP_SEND_VOLUME_TAG 0xB6
#define SCSIOP_READ_ELEMENT_STATUS 0xB8
#define SCSIOP_READ_CD_MSF 0xB9
#define SCSIOP_SCAN_CD 0xBA
#define SCSIOP_PLAY_CD 0xBC
#define SCSIOP_MECHANISM_STATUS 0xBD
#define SCSIOP_READ_CD 0xBE
#define SCSIOP_INIT_ELEMENT_RANGE 0xE7

#define XE_MAX_DMA_PRD 16

void ODD::atapiReset() {
  // Set status to Drive Ready.
  atapiState.atapiRegs.statusReg = ATA_STATUS_DRDY;

  // Initialize our input and output buffers
  atapiState.dataWriteBuffer.Initialize(ATAPI_CDROM_SECTOR_SIZE, true);
  atapiState.dataWriteBuffer.ResetPtr();
  atapiState.dataReadBuffer.Initialize(ATAPI_CDROM_SECTOR_SIZE, true);
  atapiState.dataReadBuffer.ResetPtr();

  // Set our Inquiry Data
  const char vendorIdentification[] = "PLDS   16D2S";
  memcpy(&atapiState.atapiInquiryData.vendorIdentification,
         vendorIdentification, sizeof(vendorIdentification));

  wchar_t *CdImageFilename = (wchar_t *)L"C:/Xbox/xenon.iso";
  atapiState.mountedCDImage = new Storage(CdImageFilename);
}

void ODD::atapiIdentifyCommand() {
  /*
      ATAPI drives will set the ABRT bit in the Error register and will place
     the signature of ATAPI drives in the Interrupt Reason, LBA Low, Byte Count
     Low, and Byte Count High registers.

      ATAPI Reg         | ATAPI Signature
      ------------------------------------
      Interrupt Reason  | 0x1
      LBA Low           | 0x1
      Byte Count Low    | 0x14
      Byte Count High   | 0xEB
  */

  // Set the drive status.
  atapiState.atapiRegs.statusReg |= ATA_STATUS_ERR_CHK;

  atapiState.atapiRegs.errorReg |= ATA_ERROR_ABRT;
  atapiState.atapiRegs.interruptReasonReg = 0x1;
  atapiState.atapiRegs.lbaLowReg = 0x1;
  atapiState.atapiRegs.byteCountLowReg = 0x14;
  atapiState.atapiRegs.byteCountHighReg = 0xEB;

  // An interrupt must also be requested
  parentBus->RouteInterrupt(PRIO_SATA_ODD);
}

void ODD::processSCSICommand() {
  atapiState.dataWriteBuffer.ResetPtr();
  memcpy(&atapiState.scsiCBD.AsByte, atapiState.dataWriteBuffer.Ptr(), 16);

  // Read/Sector Data for R/W operations.
  u64 readOffset = 0;
  memcpy(&readOffset, &atapiState.scsiCBD.CDB12.LogicalBlock, 4);
  u32 sectorCount = 0;
  memcpy(&sectorCount, &atapiState.scsiCBD.CDB12.TransferLength, 4);
  readOffset = _byteswap_ulong((u32)readOffset);
  sectorCount = _byteswap_ulong((u32)sectorCount);

  switch (atapiState.scsiCBD.CDB12.OperationCode) {
  case SCSIOP_INQUIRY:
    // Copy our data struct.
    memcpy(atapiState.dataReadBuffer.Ptr(), &atapiState.atapiInquiryData,
           sizeof(XE_ATAPI_INQUIRY_DATA));
    // Set the Status register to Data Request.
    atapiState.atapiRegs.statusReg |= ATA_STATUS_DRQ;
    break;
  case SCSIOP_READ:
    readOffset *= ATAPI_CDROM_SECTOR_SIZE;
    sectorCount *= ATAPI_CDROM_SECTOR_SIZE;

    atapiState.dataReadBuffer.Initialize(sectorCount, false);
    atapiState.dataReadBuffer.ResetPtr();
    atapiState.mountedCDImage->Read(readOffset, atapiState.dataReadBuffer.Ptr(),
                                    sectorCount);

    break;
  default:
    std::cout << "ATAPI: Unknown SCSI Command requested: 0x"
              << atapiState.scsiCBD.CDB12.OperationCode << std::endl;
  }

  atapiState.atapiRegs.interruptReasonReg = IDE_INTERRUPT_REASON_IO;
}

void ODD::doDMA() {
  for (;;) {
    // Read the first entry of the table in memory.
    memcpy(
        &atapiState.dmaState,
        mainMemory->getPointerToAddress(atapiState.atapiRegs.dmaTableOffsetReg +
                                        atapiState.dmaState.currentTableOffset),
        8); // Each entry is 64 bit long

    // Store current position in the table.
    atapiState.dmaState.currentTableOffset += 8;

    // If this bit in the Command register is set we're facing a read operation.
    bool readOperation = atapiState.atapiRegs.dmaCmdReg & XE_ATAPI_DMA_WR;
    // This bit specifies that we're facing the last entry in the PRD Table.
    bool lastEntry = atapiState.dmaState.currentPRD.control & 0x8000;
    // The byte count to read/write.
    u16 byteCount = atapiState.dmaState.currentPRD.sizeInBytes;
    // The address in memory to be written to/read from.
    u32 bufferAddress = atapiState.dmaState.currentPRD.physAddress;
    // Buffer Pointer in main memory.
    u8 *bufferInMemory = mainMemory->getPointerToAddress(bufferAddress);

    if (readOperation) {
      // Reading from us
      byteCount = fmin((u32)byteCount, atapiState.dataReadBuffer.Space());

      // Buffer overrun?
      if (byteCount == 0)
        return;
      memcpy(bufferInMemory, atapiState.dataReadBuffer.Ptr(), byteCount);
      atapiState.dataReadBuffer.Increment(byteCount);
    } else {
      // Writing to us
      byteCount = fmin((u32)byteCount, atapiState.dataWriteBuffer.Space());
      // Buffer overrun?
      if (byteCount == 0)
        return;
      memcpy(atapiState.dataWriteBuffer.Ptr(), bufferInMemory, byteCount);
      atapiState.dataWriteBuffer.Increment(byteCount);
    }
    if (lastEntry) {
      // Reset the current position.
      atapiState.dmaState.currentTableOffset = 0;
      // After completion we must raise an interrupt.
      parentBus->RouteInterrupt(PRIO_SATA_ODD);
      return;
    }
  }
}

ODD::ODD(PCIBridge *parentPCIBridge, RAM *ram) {
  // Set PCI Ven & Dev ID.
  u32 devID = 0x58021414;
  memcpy(&pciConfigSpace.data[0], &devID, 4);

  // Assign our PCI Bridge and RAM pointers.
  parentBus = parentPCIBridge;
  mainMemory = ram;

  // Reset our State.
  atapiReset();
}

void ODD::Read(u64 readAddress, u64 *data, u8 byteCount) {
  // PCI BAR0 is the Primary Command Block Base Address.
  u8 atapiCommandReg =
      (u8)(readAddress - pciConfigSpace.configSpaceHeader.BAR0);

  // PCI BAR1 is the Primary Control Block Base Address.
  u8 atapiControlReg =
      (u8)(readAddress - pciConfigSpace.configSpaceHeader.BAR1);

  // Who are we reading from?
  if (atapiCommandReg < (pciConfigSpace.configSpaceHeader.BAR1 -
                         pciConfigSpace.configSpaceHeader.BAR0)) {
    // Command Registers
    switch (atapiCommandReg) {
    case ATAPI_REG_DATA:
      // Check if we have some data to return.
      if (!atapiState.dataReadBuffer.Empty()) {
        byteCount = fmin(byteCount, atapiState.dataReadBuffer.Space());
        memcpy(data, atapiState.dataReadBuffer.Ptr(), byteCount);
        atapiState.dataReadBuffer.Increment(byteCount);
        return;
      }
      return;
    case ATAPI_REG_ERROR:
      memcpy(data, &atapiState.atapiRegs.errorReg, byteCount);
      return;
    case ATAPI_REG_LBA_LOW:
      memcpy(data, &atapiState.atapiRegs.lbaLowReg, byteCount);
      return;
    case ATAPI_REG_STATUS:
      memcpy(data, &atapiState.atapiRegs.statusReg, byteCount);
      return;
    default:
      std::cout << "ATAPI: Unknown Command Register Block register being read, "
                   "command"
                << "code = 0x" << atapiCommandReg << std::endl;
      break;
    }
  } else {
    // Control Registers
    switch (atapiControlReg) {
    case ATAPI_DMA_REG_COMMAND:
      memcpy(data, &atapiState.atapiRegs.dmaCmdReg, byteCount);
      break;
    case ATAPI_DMA_REG_STATUS:
      memcpy(data, &atapiState.atapiRegs.dmaStatusReg, byteCount);
      break;
    case ATAPI_DMA_REG_TABLE_OFFSET:
      memcpy(data, &atapiState.atapiRegs.dmaTableOffsetReg, byteCount);
      break;
    default:
      std::cout << "ATAPI: Unknown Control Register Block register being read, "
                   "command"
                << "code = 0x" << atapiControlReg << std::endl;
      break;
    }
  }
}

void ODD::Write(u64 writeAddress, u64 data, u8 byteCount) {
  // PCI BAR0 is the Primary Command Block Base Address.
  u8 atapiCommandReg =
      (u8)(writeAddress - pciConfigSpace.configSpaceHeader.BAR0);

  // PCI BAR1 is the Primary Control Block Base Address.
  u8 atapiControlReg =
      (u8)(writeAddress - pciConfigSpace.configSpaceHeader.BAR1);

  // Who are we writing to?
  if (atapiCommandReg < (pciConfigSpace.configSpaceHeader.BAR1 -
                         pciConfigSpace.configSpaceHeader.BAR0)) {
    // Command Registers
    switch (atapiCommandReg) {
    case ATAPI_REG_DATA:
      // Reset the DRQ status
      atapiState.atapiRegs.statusReg &= ~ATA_STATUS_DRQ;

      memcpy(&atapiState.atapiRegs.dataReg, &data, byteCount);

      // Push the data onto our buffer
      byteCount = fmin(byteCount, atapiState.dataWriteBuffer.Space());
      memcpy(atapiState.dataWriteBuffer.Ptr(), &data, byteCount);
      atapiState.dataWriteBuffer.Increment(byteCount);

      // Check if we're executing a SCSI command input and we have a full
      // command
      if (atapiState.dataWriteBuffer.Count() >= XE_ATAPI_CDB_SIZE &&
          atapiState.atapiRegs.commandReg == ATA_COMMAND_PACKET) {
        // Process SCSI Command
        processSCSICommand();
        // Reset our buffer ptr.
        atapiState.dataWriteBuffer.ResetPtr();
        // Request an Interrupt.
        parentBus->RouteInterrupt(PRIO_SATA_ODD);
        // Clear our Command Register.
        atapiState.atapiRegs.commandReg = 0;
      }
      return;
    case ATAPI_REG_FEATURES:
      memcpy(&atapiState.atapiRegs.featuresReg, &data, byteCount);
      return;
    case ATAPI_REG_SECTOR_COUNT:
      memcpy(&atapiState.atapiRegs.sectorCountReg, &data, byteCount);
      return;
    case ATAPI_REG_LBA_LOW:
      memcpy(&atapiState.atapiRegs.lbaLowReg, &data, byteCount);
      return;
    case ATAPI_REG_BYTE_COUNT_LOW:
      memcpy(&atapiState.atapiRegs.byteCountLowReg, &data, byteCount);
      return;
    case ATAPI_REG_BYTE_COUNT_HIGH:
      memcpy(&atapiState.atapiRegs.byteCountHighReg, &data, byteCount);
      return;
    case ATAPI_REG_DEVICE:
      memcpy(&atapiState.atapiRegs.deviceReg, &data, byteCount);
      return;
    case ATAPI_REG_COMMAND:
      memcpy(&atapiState.atapiRegs.commandReg, &data, byteCount);

      // Reset the Status register.
      atapiState.atapiRegs.statusReg &= ~ATA_STATUS_ERR_CHK;
      // Reset the Error register.
      atapiState.atapiRegs.errorReg &= ~ATA_ERROR_ABRT;

      switch (data) {
      case ATA_COMMAND_PACKET:
        atapiState.atapiRegs.statusReg |= ATA_STATUS_DRQ;
        return;
      case ATA_COMMAND_IDENTIFY_DEVICE:
        atapiIdentifyCommand();
        return;
      default:
        std::cout << "ATAPI: Unknown command, command code = 0x" << data
                  << std::endl;
        break;
      }
      return;
    default:
      std::cout << "ATAPI: Unknown Command Register Block register being "
                   "written, command"
                << "code = 0x" << atapiCommandReg << std::endl;
      break;
    }
  } else {
    // Control Registers
    switch (atapiControlReg) {
    case ATAPI_DMA_REG_COMMAND:
      memcpy(&atapiState.atapiRegs.dmaCmdReg, &data, byteCount);

      if (data & XE_ATAPI_DMA_ACTIVE) {
        // Start our DMA Operation
        doDMA();
        // Change our DMA Status after completion
        atapiState.atapiRegs.dmaStatusReg &= ~XE_ATAPI_DMA_ACTIVE;
      }
      break;
    case ATAPI_DMA_REG_STATUS:
      memcpy(&atapiState.atapiRegs.dmaStatusReg, &data, byteCount);
      break;
    case ATAPI_DMA_REG_TABLE_OFFSET:
      memcpy(&atapiState.atapiRegs.dmaTableOffsetReg, &data, byteCount);
      break;
    default:
      std::cout << "ATAPI: Unknown Control Register Block register being "
                   "written, command"
                << "code = 0x" << atapiControlReg << std::endl;
      break;
    }
  }
}

void ODD::ConfigRead(u64 readAddress, u64 *data, u8 byteCount) {
  memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
}

void ODD::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {
  memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
}
