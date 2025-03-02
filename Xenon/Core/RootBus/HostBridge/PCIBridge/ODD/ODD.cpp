// Copyright 2025 Xenon Emulator Project

#include "ODD.h"

#include "Base/Config.h"
#include "Base/Logging/Log.h"

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

  atapiState.mountedCDImage = new Storage(Config::oddImagePath());
}

void ODD::atapiIdentifyPacketDeviceCommand() {
  // This command is only for ATAPI devices.

  // TODO(bitsh1ft3r): Fill out the struct with data from an actual drive.

  if (!atapiState.dataReadBuffer.Initialize(ATAPI_CDROM_SECTOR_SIZE, true)) {
    LOG_ERROR(ODD, "Failed to initialize data buffer for atapiIdentifyPacketDeviceCommand");
  }

  // Set the struct.
  atapiState.atapiIdentifyData.generalConfiguration = 0x8000; // ATAPI device.

  // Reset the pointer.
  atapiState.dataReadBuffer.ResetPtr();
  // Copy the data.
  memcpy(atapiState.dataReadBuffer.Ptr(), &atapiState.atapiIdentifyData, 
    sizeof(XE_ATA_IDENTIFY_DATA));
  // Set the drive status.
  atapiState.atapiRegs.statusReg |= ATA_STATUS_DRQ;
  // Request an interrupt.
  parentBus->RouteInterrupt(PRIO_SATA_ODD);
  // Set interrupt reason.
  atapiState.atapiRegs.interruptReasonReg = IDE_INTERRUPT_REASON_IO;
}

void ODD::atapiIdentifyCommand() {
  // Used by software to decide whether the device is an ATA or ATAPI device.
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
  // Set interrupt reason.
  atapiState.atapiRegs.interruptReasonReg = IDE_INTERRUPT_REASON_IO;
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
    LOG_ERROR(ODD, "Unknown SCSI Command requested: {:#x}", atapiState.scsiCBD.CDB12.OperationCode);
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
                                                 
ODD::ODD(const char* deviceName, u64 size,
  PCIBridge *parentPCIBridge, RAM *ram) : PCIDevice(deviceName, size) {
  // Note:
  // The ATA/ATAPI Controller in the Xenon Southbridge contain two BAR's:
  // The first is for the Command Block (Regs 0-7) + DevCtrl/AltStatus reg at offset 0xA.
  // The second is for the BMDMA (Bus Master DMA) block.

  // Set PCI Properties.
  pciConfigSpace.configSpaceHeader.reg0.hexData = 0x58021414;
  pciConfigSpace.configSpaceHeader.reg1.hexData = 0x02300006;
  pciConfigSpace.configSpaceHeader.reg2.hexData = 0x01060000;
  pciConfigSpace.configSpaceHeader.regD.hexData = 0x00000058; // Capabilites Ptr.
  pciConfigSpace.configSpaceHeader.regF.hexData = 0x00000100; // Int line, pin.

  u32 data = 0;

  // Capabilities at offset 0x58:
  data = 0x80020001;
  memcpy(&pciConfigSpace.data[0x58], &data, 4);
  data = 0x00112400;
  memcpy(&pciConfigSpace.data[0x60], &data, 4);
  data = 0x7f7f7f7f;
  memcpy(&pciConfigSpace.data[0x70], &data, 4);
  memcpy(&pciConfigSpace.data[0x74], &data, 4); // Field value is the same as above.
  data = 0xc07231be;
  memcpy(&pciConfigSpace.data[0x80], &data, 4);
  data = 0x100c04cc;
  memcpy(&pciConfigSpace.data[0x98], &data, 4);
  data = 0x004108c0;
  memcpy(&pciConfigSpace.data[0x9C], &data, 4);

  // Set the SCR's at offset 0xC0 (SiS-like).
  // SStatus.
  data = 0x00000113;
  memcpy(&pciConfigSpace.data[0xC0], &data, 4); // SSTATUS_DET_COM_ESTABLISHED.
                                                // SSTATUS_SPD_GEN1_COM_SPEED.
                                                // SSTATUS_IPM_INTERFACE_ACTIVE_STATE.
  // SError.
  data = 0x001f0201;
  memcpy(&pciConfigSpace.data[0xC4], &data, 4);
  // SControl.
  data = 0x00000300;
  memcpy(&pciConfigSpace.data[0xC8], &data, 4); // SCONTROL_IPM_ALL_PM_DISABLED.
  
  // Set our PCI Dev Sizes.
  pciDevSizes[0] = 0x20; // BAR0
  pciDevSizes[1] = 0x10; // BAR1

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
    case ATAPI_REG_INT_REAS:
      memcpy(data, &atapiState.atapiRegs.interruptReasonReg, byteCount);
      return;
    case ATAPI_REG_LBA_LOW:
      memcpy(data, &atapiState.atapiRegs.lbaLowReg, byteCount);
      return;
    case ATAPI_REG_BYTE_COUNT_LOW:
      memcpy(data, &atapiState.atapiRegs.byteCountLowReg, byteCount);
      return;
    case ATAPI_REG_BYTE_COUNT_HIGH:
      memcpy(data, &atapiState.atapiRegs.byteCountHighReg, byteCount);
      return;
    case ATAPI_REG_DEVICE:
      memcpy(data, &atapiState.atapiRegs.deviceReg, byteCount);
      return;
    case ATAPI_REG_STATUS:
      memcpy(data, &atapiState.atapiRegs.statusReg, byteCount);
      return;
    case ATAPI_REG_ALTERNATE_STATUS:
      memcpy(data, &atapiState.atapiRegs.altStatusReg, byteCount);
      return;
    default:
      LOG_ERROR(ODD, "Unknown Command Register Block register being read, command code = {:#x}", atapiCommandReg);
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
      LOG_ERROR(ODD, "Unknown Control Register Block register being read, command code = {:#x}", atapiControlReg);
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
      case ATA_COMMAND_IDENTIFY_PACKET_DEVICE:
        atapiIdentifyPacketDeviceCommand();
        return;
      case ATA_COMMAND_IDENTIFY_DEVICE:
        atapiIdentifyCommand();
        return;
      default:
        LOG_ERROR(ODD, "Unknown command, command code = {:#x}", data);
        break;
      }
      return;
    case ATAPI_REG_DEVICE_CONTROL:
      memcpy(&atapiState.atapiRegs.devControlReg, &data, byteCount);
      return;
    default:
      LOG_ERROR(ODD, "Unknown Command Register Block register being written, command reg = {:#x}"
        ", write address = {:#x}, data = {:#x}", atapiCommandReg, writeAddress, data);
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
      LOG_ERROR(ODD, "Unknown Control Register Block register being written, command code = {:#x}", atapiControlReg);
      break;
    }
  }
}

void ODD::ConfigRead(u64 readAddress, u64 *data, u8 byteCount) {
  u8 readReg = static_cast<u8>(readAddress);
  if (readReg >= XE_SIS_SCR_BASE && readReg <= 0xFF)
  {
    // Confiduration read to the SATA Status and Control Registers.
    switch ((readReg - XE_SIS_SCR_BASE) / 4)
    {
    case SCR_STATUS_REG:
      LOG_WARNING(ODD, "SCR ConfigRead to SCR_STATUS_REG.");
      break;
    case SCR_ERROR_REG:
      LOG_WARNING(ODD, "SCR ConfigRead to SCR_ERROR_REG.");
      break;
    case SCR_CONTROL_REG:
      LOG_WARNING(ODD, "SCR ConfigRead to SCR_CONTROL_REG.");
      break;
      case SCR_ACTIVE_REG:
      LOG_WARNING(ODD, "SCR ConfigRead to SCR_ACTIVE_REG.");
      break;
      case SCR_NOTIFICATION_REG:
      LOG_WARNING(ODD, "SCR ConfigRead to SCR_NOTIFICATION_REG.");
      break;
    default:
      LOG_ERROR(ODD, "SCR ConfigRead to reg {:#x}", readReg * 4);
      break;
    }
  }
  memcpy(data, &pciConfigSpace.data[static_cast<u8>(readAddress)], byteCount);
  LOG_DEBUG(ODD, "ConfigRead to reg {:#x}", readReg * 4);
}

void ODD::ConfigWrite(u64 writeAddress, u64 data, u8 byteCount) {
  // Check if we're being scanned.
  if (static_cast<u8>(writeAddress) >= 0x10 && static_cast<u8>(writeAddress) < 0x34) {
    const u32 regOffset = (static_cast<u8>(writeAddress) - 0x10) >> 2;
    if (pciDevSizes[regOffset] != 0) {
      if (data == 0xFFFFFFFF) { // PCI BAR Size discovery.
        u64 x = 2;
        for (int idx = 2; idx < 31; idx++) {
          data &= ~x;
          x <<= 1;
          if (x >= pciDevSizes[regOffset]) {
            break;
          }
        }
        data &= ~0x3;
      }
    }
    if (static_cast<u8>(writeAddress) == 0x30) { // Expansion ROM Base Address.
      data = 0; // Register not implemented.
    }
  }

  u8 writeReg = static_cast<u8>(writeAddress);
  if (writeReg >= XE_SIS_SCR_BASE && writeReg <= 0xFF)
  {
    // Confiduration write to the SATA Status and Control Registers.
    switch ((writeReg - XE_SIS_SCR_BASE) / 4)
    {
    case SCR_STATUS_REG:
      LOG_WARNING(ODD, "SCR ConfigWrite to SCR_STATUS_REG, data {:#x}", data);
      break;
    case SCR_ERROR_REG:
      LOG_WARNING(ODD, "SCR ConfigWrite to SCR_ERROR_REG, data {:#x}", data);
      break;
    case SCR_CONTROL_REG:
      LOG_WARNING(ODD, "SCR ConfigWrite to SCR_CONTROL_REG, data {:#x}", data);
      break;
    case SCR_ACTIVE_REG:
      LOG_WARNING(ODD, "SCR ConfigWrite to SCR_ACTIVE_REG, data {:#x}", data);
      break;
    case SCR_NOTIFICATION_REG:
      LOG_WARNING(ODD, "SCR ConfigRead to SCR_NOTIFICATION_REG, data {:#x}", data);
      break;
    default:
      LOG_ERROR(ODD, "SCR ConfigWrite to reg {:#x}, data {:#x}", writeReg * 4, data);
      break;
    }
  }
  memcpy(&pciConfigSpace.data[static_cast<u8>(writeAddress)], &data, byteCount);
  LOG_DEBUG(ODD, "ConfigWrite to reg {:#x}, data {:#x}", writeReg * 4, data);
}
