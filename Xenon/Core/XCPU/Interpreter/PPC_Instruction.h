// Copyright 2025 Xenon Emulator Project

#pragma once

#include <string>
#include <unordered_map>

#include "Base/Types.h"

//
// PowerPC Instruction definitions
//

struct PPU_STATE;
extern void PPCInterpreter_invalid(PPU_STATE*);

namespace PPCInterpreter {
  // Define a type alias for function pointers
  using PPCInstructionHandler = void(*)(PPU_STATE*);
  struct PPCInstruction {
    const char* name;
    PPCInstructionHandler handler;
  };
  union PPCInstructionData {
    uint32_t Data;
    struct {
      unsigned int XO_30to31 : 2;
      unsigned int XO_27to30 : 4;
      unsigned int XO_27to29 : 3;
      unsigned int XO_22to30 : 9;
      unsigned int XO_21to30 : 10;
      unsigned int XO_21to29 : 9;
      unsigned int XO_20to30 : 11;
      unsigned int OPCD : 6;
    };
  };
  extern const std::unordered_map<u32, PPCInstruction> opcodeMap;
  extern const std::unordered_map<u32, PPCInstruction> subgroup19Map;
  extern const std::unordered_map<u32, PPCInstruction> subgroup30Map;
  extern const std::unordered_map<u32, PPCInstruction> subgroup31Map;
  extern const std::unordered_map<u32, PPCInstruction> subgroup58Map;
  extern const std::unordered_map<u32, PPCInstruction> subgroup62Map;
  extern const std::unordered_map<u32, PPCInstruction> subgroup63Map;
  extern PPCInstruction getInstruction(const PPCInstructionData& data);                                          \
} // namespace PPCInterpreter
