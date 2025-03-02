// Copyright 2025 Xenon Emulator Project

#include "NAND.h"

#include "Base/Logging/Log.h"

/********************Responsible for loading the NAND file********************/
NAND::NAND(const char* deviceName, const std::string filePath,
  u64 startAddress, u64 endAddress,
  bool isSOCDevice) : SystemDevice(deviceName, startAddress, endAddress, isSOCDevice) {
  LOG_INFO(System, "NAND: Loading file {}", filePath.c_str());

  if (fopen_s(&inputFile, filePath.c_str(), "rb") != 0) {
    LOG_CRITICAL(System, "NAND: Unable to load file!");
    SYSTEM_PAUSE();
  }

  fseek(inputFile, 0, SEEK_END);
  rawFileSize = ftell(inputFile);
  fseek(inputFile, 0, SEEK_SET);

  LOG_INFO(System, "NAND: File size = {:#x} bytes.", rawFileSize);

  CheckMagic();

  if (!CheckMagic()) {
    LOG_ERROR(System, "NAND: Wrong magic found, Xbox 360 Retail NAND magic is 0xFF4F and Devkit NAND magic 0x0F4F.");
    SYSTEM_PAUSE();
  }

  const u32 blockSize = 0x4000;

  for (int currentBlock = 0; currentBlock < rawFileSize;
       currentBlock += blockSize) {
    fread(&rawNANDData[currentBlock], 1, blockSize, inputFile);
  }

  fseek(inputFile, 0, SEEK_SET);

  CheckSpare();

  if (hasSpare) {
    LOG_INFO(System, "NAND: Image has spare.");

    // Check Meta Type
    imageMetaType = DetectSpareType();
  }

  fclose(inputFile);
}

/************Responsible for reading the NAND************/
void NAND::Read(u64 readAddress, u64 *data, u8 byteCount) {
  u32 offset = (u32)readAddress & 0xffffff;
  offset = 1 ? ((offset / 0x200) * 0x210) + offset % 0x200 : offset;
  memcpy(data, &rawNANDData[offset], byteCount);
}

/************Responsible for writing the NAND************/
void NAND::Write(u64 writeAddress, u64 data, u8 byteCount) {
  u32 offset = (u32)writeAddress & 0xffffff;
  offset = 1 ? ((offset / 0x200) * 0x210) + offset % 0x200 : offset;
  memcpy(&rawNANDData[offset], &data, byteCount);
}

//*Checks ECD Page.
bool NAND::CheckPageECD(u8 *data, s32 offset) {
  u8 actualData[4] = {0};
  u8 calculatedECD[4] = {0};

  fseek(inputFile, offset + 524, SEEK_SET);
  fread(&actualData[0], 1, 4, inputFile);
  fseek(inputFile, 0, SEEK_SET);

  CalculateECD(data, offset, calculatedECD);

  return (
      calculatedECD[0] == actualData[0] && calculatedECD[1] == actualData[1] &&
      calculatedECD[2] == actualData[2] && calculatedECD[3] == actualData[3]);
}

//*Calculates the ECD.
void NAND::CalculateECD(u8 *data, int offset, u8 ret[]) {
  u32 i, val = 0, v = 0;
  u32 count = 0;
  for (i = 0; i < 0x1066; i++) {
    if ((i & 31) == 0) {
      u32 value = u32((u8)(data[count + offset]) << 24 |
                      (u8)(data[count + offset + 1]) << 16 |
                      (u8)(data[count + offset + 2]) << 8 |
                      (u8)(data[count + offset + 3]));
      value = _byteswap_ulong(value);
      v = ~value;
      count += 4;
    }
    val ^= v & 1;
    v >>= 1;
    if (val & 1)
      val ^= 0x6954559;
    val >>= 1;
  }
  val = ~val;
  ret[0] = (val << 6);
  ret[1] = (val >> 2) & 0xFF;
  ret[2] = (val >> 10) & 0xFF;
  ret[3] = (val >> 18) & 0xFF;
}

//*Checks Magic.
bool NAND::CheckMagic() {
  u8 magic[2];

  fread(&magic, 1, 2, inputFile);
  fseek(inputFile, 0, SEEK_SET);

  if ((magic[0] == 0xff || magic[0] == 0x0f) &&
      (magic[1] == 0x3f || magic[1] == 0x4f)) {
    return true;
  }
  return false;
}

//*Checks Spare.
void NAND::CheckSpare() {
  u8 data[0x630]{};
  fseek(inputFile, 0, SEEK_SET);
  fread(data, 1, 0x630, inputFile);
  hasSpare = true;
  u8 *spare = nullptr;

  for (int idx = 0; idx < sizeof(data); idx += 0x210) {
    if (!CheckPageECD(data, idx)) {
      hasSpare = false;
    }
  }
}

//*Detects Spare Type.
MetaType NAND::DetectSpareType(bool firstTry) {
  if (!hasSpare) {
    return metaTypeNone;
  }

  if (firstTry) {
    fseek(inputFile, 0x4400, SEEK_SET);
  } else {
    fseek(inputFile, (u32)rawFileSize - 0x4400, SEEK_SET);
  }

  u8 tmp[0x10]{};
  fread(tmp, 1, 0x10, inputFile);

  return metaTypeNone;
}
