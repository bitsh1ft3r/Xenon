// Copyright 2025 Xenon Emulator Project

#include "Base/Logging/Log.h"
#include "PPCInterpreter.h"
#include "PPC_Instruction.h"

const std::unordered_map<u32, PPCInterpreter::PPCInstruction> getCorrespondingJumpTable(const PPCInterpreter::PPCInstructionData& data) {
  u32 OPCD = ExtractBits(data.Data, 0, 5);
  if (OPCD == 19) {
    return PPCInterpreter::subgroup19Map;
  } else if (OPCD == 30) {
    return PPCInterpreter::subgroup30Map;
  } else if (OPCD == 30) {
    return PPCInterpreter::subgroup30Map;
  } else if (OPCD == 31) {
    return PPCInterpreter::subgroup31Map;
  } else if (OPCD == 58) {
    return PPCInterpreter::subgroup58Map;
  } else if (OPCD == 62) {
    return PPCInterpreter::subgroup62Map;
  } else if (OPCD == 63) {
    return PPCInterpreter::subgroup63Map;
  }
  //LOG_INFO(Xenon, "OPCD: {}", OPCD);
  return PPCInterpreter::opcodeMap;
}

PPCInterpreter::PPCInstruction PPCInterpreter::getInstruction(const PPCInterpreter::PPCInstructionData& data) {
  u32 OPCD = ExtractBits(data.Data, 0, 5);

  u32 XO_20to30 = ExtractBits(data.Data, 20, 30);
  u32 XO_27to29 = ExtractBits(data.Data, 27, 29);
  u32 XO_27to30 = ExtractBits(data.Data, 27, 30);
  u32 XO_21to30 = ExtractBits(data.Data, 21, 30);
  u32 XO_22to30 = ExtractBits(data.Data, 22, 30);
  u32 XO_21to29 = ExtractBits(data.Data, 21, 29);
  u32 XO_30to31 = ExtractBits(data.Data, 30, 31);
  if (OPCD == 19) {
    const auto& table = PPCInterpreter::subgroup19Map;
    if (const auto& entry = table.find(XO_21to30); entry != table.end()) {
      return entry->second;
    }
  } else if (OPCD == 30) {
    const auto& table = PPCInterpreter::subgroup30Map;
    if (const auto& entry = table.find(XO_27to29); entry != table.end()) {
      return entry->second;
    }
    if (const auto& entry = table.find(XO_27to30); entry != table.end()) {
      return entry->second;
    }
  } else if (OPCD == 31) {
    const auto& table = PPCInterpreter::subgroup31Map;
    if (const auto& entry = table.find(XO_21to30); entry != table.end()) {
      return entry->second;
    }
    if (const auto& entry = table.find(XO_22to30); entry != table.end()) {
      return entry->second;
    }
    if (const auto& entry = table.find(XO_21to29); entry != table.end()) {
      return entry->second;
    }
  } else if (OPCD == 58) {
    const auto& table = PPCInterpreter::subgroup58Map;
    if (const auto& entry = table.find(XO_30to31); entry != table.end()) {
      return entry->second;
    }
  } else if (OPCD == 62) {
    const auto& table = PPCInterpreter::subgroup62Map;
    if (const auto& entry = table.find(XO_30to31); entry != table.end()) {
      return entry->second;
    }
  } else if (OPCD == 63) {
    const auto& table = PPCInterpreter::subgroup63Map;
    if (const auto& entry = table.find(XO_21to30); entry != table.end()) {
      return entry->second;
    }
  }
  if (const auto& entry = PPCInterpreter::opcodeMap.find(OPCD); entry != PPCInterpreter::opcodeMap.end()) {
    return entry->second;
  }

  return { "invalid", PPCInterpreter_invalid };
}