// Copyright 2025 Xenon Emulator Project

#pragma once

#include <fstream>

#include "Base/SystemDevice.h"

#define NAND_START_ADDR 0xC8000000
#define NAND_END_ADDR 0xCC000000 // 64 Mb region

enum MetaType {
  metaType0 = 0,             // Pre Jasper (0x01198010)
  metaType1 = 1,             // Jasper, Trinity & Corona (0x00023010 [Jasper
                             // & Trinity] and 0x00043000 [Corona])
  metaType2 = 2,             // BigBlock Jasper (0x008A3020 and 0x00AA3020)
  metaTypeUninitialized = 3, // Really old JTAG XeLL images
  metaTypeNone = 4           // No spare type or unknown
};

class NAND : public SystemDevice {
public:
  bool Load(const std::string filePath);

  void Read(u64 readAddress, u64 *data, u8 byteCount) override;
  void Write(u64 writeAddress, u64 data, u8 byteCount) override;

private:
  FILE *inputFile;

  // 64 Mb NAND Data
  u8 *rawNANDData = new (u8[0x4000000]);

  bool CheckMagic();
  void CheckSpare();
  bool CheckPageECD(u8 *data, s32 offset);
  void CalculateECD(u8 *data, int offset, u8 ret[]);
  MetaType DetectSpareType(bool firstTry = true);

  // void SeekPos(s32 addres);

  size_t rawFileSize = 0;
  bool hasSpare = false;
  MetaType imageMetaType = MetaType::metaTypeNone;
};
