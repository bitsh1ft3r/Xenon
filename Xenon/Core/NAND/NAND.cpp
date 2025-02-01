// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"

#include "NAND.h"

/********************Responsible for loading the NAND file********************/
bool NAND::Load(const std::string filePath) {
  LOG_INFO(System, "NAND: Loading file {}", filePath.c_str());
  inputFile.open(filePath, std::ios_base::in | std::ios_base::binary);
  if (!inputFile.is_open()) {
    LOG_CRITICAL(System, "NAND: Unable to load file!");
    return false;
  }

  rawFileSize = std::filesystem::file_size(filePath);

  LOG_INFO(System, "NAND: File size = {:#x} bytes.", rawFileSize);

  CheckMagic();

  if (!CheckMagic()) {
    LOG_ERROR(System, "NAND: Wrong magic found, Xbox 360 Retail NAND magic is 0xFF4F and Devkit NAND magic 0x0F4F.");
    return false;
  }

  const u32 blockSize = 0x4000;

  for (int currentBlock = 0; currentBlock < rawFileSize;
       currentBlock += blockSize) {
    inputFile.read(reinterpret_cast<char*>(rawNANDData + currentBlock), blockSize);
  }


  CheckSpare();

  if (hasSpare) {
    LOG_INFO(System, "NAND: Image has spare.");

    // Check Meta Type
    imageMetaType = DetectSpareType();
  }

  inputFile.close();

  return true;
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
  inputFile.seekg(offset + 524);
  inputFile.read(reinterpret_cast<char*>(actualData), sizeof(actualData));
  inputFile.seekg(0, std::ios_base::_Seekbeg);

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

  inputFile.read(reinterpret_cast<char*>(magic), sizeof(magic));
  inputFile.seekg(0, std::ios_base::_Seekbeg);

  if ((magic[0] == 0xff || magic[0] == 0x0f) &&
      (magic[1] == 0x3f || magic[1] == 0x4f)) {
    return true;
  }
  return false;
}

//*Checks Spare.
void NAND::CheckSpare() {
  u8 data[0x630] = {0};                       
  inputFile.seekg(0, std::ios_base::_Seekbeg);
  inputFile.read(reinterpret_cast<char*>(data), sizeof(data));
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

  inputFile.seekg(firstTry ? 0x4400 : (u32)rawFileSize - 0x4400, std::ios_base::_Seekbeg);

  u8 tmp[0x10];
  inputFile.read(reinterpret_cast<char*>(tmp), sizeof(tmp));

  u8 a = 0;
  return metaTypeNone;
}
