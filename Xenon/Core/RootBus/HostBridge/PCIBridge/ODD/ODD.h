// Copyright 2025 Xenon Emulator Project

#pragma once

#include <fstream>
#include <filesystem>
#include <vector>

#include "Core/RAM/RAM.h"
#include "Core/RootBus/HostBridge/PCIBridge/PCIBridge.h"
#include "Core/RootBus/HostBridge/PCIBridge/PCIDevice.h"

#define ODD_DEV_SIZE 0x30

// Read Capacity Data - returned in Big Endian format
struct READ_CAPACITY_DATA {
  u32 logicalBlockAddress;
  u32 bytesPerBlock;
};

//
// Data Buffers
//

class DataBuffer {
public:
  bool Empty(void) { return Pointer >= Size; }
  u32 Space(void) { return Size - Pointer; }
  u32 Count(void) { return Pointer; }
  u8 *Ptr(void) { return Data + Pointer; }
  void Increment(s32 Inc) { Pointer += Inc; }
  void ResetPtr(void) { Pointer = 0; }
  bool Initialize(u32 MaxLength, bool fClear) {
    if (Data && (MaxLength > Size)) {
      delete Data;
      memset(this, 0, sizeof *this);
    }
    if (Data == nullptr) {
      Data = ::new u8[MaxLength];
    }
    if (Data) {
      Size = std::max(Size, MaxLength);
      Pointer = Size; // Empty()
      if (fClear)
        memset(Data, 0, MaxLength);
      return true;
    }
    return false;
  }

private:
  u8 *Data;
  u32 Size;
  u32 Pointer;
};

//
// Read Only Storage
//
class Storage {
public:
  Storage(const std::filesystem::path& filepath) : path(filepath) {
    file.open(path, std::ios_base::in | std::ifstream::ate | std::ifstream::binary);
  }

  ~Storage(void) {
    if (file.is_open())
      file.close();
  }

  bool IsFound(void) {
    return file.is_open() && !std::filesystem::file_size(path);
  }

  u32 Size(void) {
    return std::filesystem::file_size(path);
  }

  bool Read(u64 Offset, u8* Destination, u32 cu8s) {
    file.seekg(static_cast<std::streamoff>(Offset), std::ios::beg);
    if (!file) {
        return false;
    }
    return file.read(reinterpret_cast<char*>(Destination), cu8s) && file.gcount() == static_cast<std::streamsize>(cu8s);
  }

private:
  std::filesystem::path path;
  std::ifstream file;
};

//
// SCSI Inquiry Data Structure
//

union inquiryDataByte0 {
  u8 dataHex;
  struct {
    u8 peripheralDevType : 5;
    u8 peripheralQualifier : 3;
  };
};
union inquiryDataByte1 {
  u8 dataHex;
  struct {
    u8 res : 7;
    u8 rmb : 1;
  };
};
union inquiryDataByte3 {
  u8 dataHex;
  struct {
    u8 responseDataFormat : 4;
    u8 hisup : 1;
    u8 normaca : 1;
    u8 obs : 2;
  };
};
union inquiryDataByte5 {
  u8 dataHex;
  struct {
    u8 protect : 1;
    u8 res : 2;
    u8 threePc : 1;
    u8 tpgs : 2;
    u8 acc : 1;
    u8 sccs : 1;
  };
};
union inquiryDataByte6 {
  u8 dataHex;
  struct {
    u8 re0 : 1;
    u8 obs0 : 3;
    u8 multip : 1;
    u8 vs : 1;
    u8 encserv : 1;
    u8 ob1 : 1;
  };
};
union inquiryDataByte7 {
  u8 dataHex;
  struct {
    u8 vs : 1;
    u8 cmdque : 1;
    u8 obs0 : 2;
    u8 res : 2;
    u8 obs1 : 2;
  };
};

// XeLL only reads the first 36 bytes.
struct XE_ATAPI_INQUIRY_DATA {
  inquiryDataByte0 byte0;
  inquiryDataByte1 byte1;
  u8 version;
  inquiryDataByte3 byte3;
  u8 additionalLenght;
  inquiryDataByte5 byte5;
  inquiryDataByte6 byte6;
  inquiryDataByte7 byte7;
  u8 vendorIdentification[8];
  u8 productIdentification[16];
  u8 productRevisionLevel[4];
};

//
// SCSI Command Descriptor Block
//

union XE_CDB {

  //
  // Generic 6-Byte CDB
  //

  struct _CDB6GENERIC {
    u8 OperationCode;
    u8 Immediate : 1;
    u8 CommandUniqueBits : 4;
    u8 LogicalUnitNumber : 3;
    u8 CommandUniqueBytes[3];
    u8 Link : 1;
    u8 Flag : 1;
    u8 Reserved : 4;
    u8 VendorUnique : 2;
  } CDB6GENERIC, *PCDB6GENERIC;

  //
  // Standard 10-byte CDB
  //

  struct _CDB10 {
    u8 OperationCode;
    u8 RelativeAddress : 1;
    u8 Reserved1 : 2;
    u8 ForceUnitAccess : 1;
    u8 DisablePageOut : 1;
    u8 LogicalUnitNumber : 3;
    union {
      struct {
        u8 LogicalBlockByte0;
        u8 LogicalBlockByte1;
        u8 LogicalBlockByte2;
        u8 LogicalBlockByte3;
      };
      u32 LogicalBlock;
    };
    u8 Reserved2;
    union {
      struct {
        u8 TransferBlocksMsb;
        u8 TransferBlocksLsb;
      };
      u16 TransferBlocks;
    };
    u8 Control;
  } CDB10, *PCDB10;

  //
  // Standard 12-byte CDB
  //

  struct _CDB12 {
    u8 OperationCode;
    u8 RelativeAddress : 1;
    u8 Reserved1 : 2;
    u8 ForceUnitAccess : 1;
    u8 DisablePageOut : 1;
    u8 LogicalUnitNumber : 3;
    u8 LogicalBlock[4];   // [0]=MSB, [3]=LSB
    u8 TransferLength[4]; // [0]=MSB, [3]=LSB
    u8 Reserved2;
    u8 Control;
  } CDB12, *PCDB12;

  u32 AsUlong[4];
  u8 AsByte[16];
};

//
// Direct Memory Accesss PRD
//

// DMA Physical Region Descriptor
struct XE_ATAPI_DMA_PRD {
  u32 physAddress; // physical memory address of a data buffer
  u16 sizeInBytes;
  u16 control;
};

struct XE_ATAPI_DMA_STATE {
  XE_ATAPI_DMA_PRD currentPRD = {0};
  u32 currentTableOffset = 0;
};

//
// ATAPI Register State Structure
//
struct XE_ATAPI_REGISTERS {
  /* Command Block */

  // Offset 0x0
  u32 dataReg;
  // Offset 0x1
  struct {
    // When Read
    u32 errorReg;
    // When written
    u32 featuresReg;
  };
  // Offset 0x2
  struct {
    // When Read
    u32 interruptReasonReg;
    // When written
    u32 sectorCountReg;
  };
  // Offset 0x3
  u32 lbaLowReg;
  // Offset 0x4
  u32 byteCountLowReg;
  // Offset 0x5
  u32 byteCountHighReg;
  // Offset 0x6
  u32 deviceReg;
  // Offset 0x7
  struct {
    // When Read
    u32 statusReg;
    // When written
    u32 commandReg;
  };

  /* Control Block */
  // Offset 0x0
  u32 dmaCmdReg;
  // Offset 0x2
  u32 dmaStatusReg;
  // Offset 0x4
  u32 dmaTableOffsetReg;
  // Offset 0x6
  struct {
    // When Read
    u32 alternateStatusReg;
    // When written
    u32 deviceControlReg;
  };
};

//
// ATAPI Device State Structure
//
struct XE_ATAPI_DEV_STATE {
  // Register Set
  XE_ATAPI_REGISTERS atapiRegs = {0};
  // Data Read Buffer
  DataBuffer dataReadBuffer;
  // Data Write Buffer
  DataBuffer dataWriteBuffer;
  // ATAPI Inquiry Data
  XE_ATAPI_INQUIRY_DATA atapiInquiryData = {0};
  // SCSI Command Descriptor Block
  XE_CDB scsiCBD = {0};
  // Direct Memroy Access Processing
  XE_ATAPI_DMA_STATE dmaState = {0};
  // Mounted ISO Image
  Storage *mountedCDImage;
};

class ODD : public PCIDevice {
public:
  ODD(const char* deviceName, u64 size,
    PCIBridge *parentPCIBridge, RAM *ram);
  void Read(u64 readAddress, u64 *data, u8 u8Count) override;
  void ConfigRead(u64 readAddress, u64 *data, u8 u8Count) override;
  void Write(u64 writeAddress, u64 data, u8 u8Count) override;
  void ConfigWrite(u64 writeAddress, u64 data, u8 u8Count) override;

private:
  // PCI Bridge pointer. Used for Interrupts.
  PCIBridge *parentBus;

  // RAM pointer. Used for DMA.
  RAM *mainMemory;

  // ATAPI Device State
  XE_ATAPI_DEV_STATE atapiState = {0};

  // SCSI Command Processing
  void processSCSICommand();

  void doDMA();

  // Misc
  void atapiReset();
  void atapiIdentifyCommand();
};
