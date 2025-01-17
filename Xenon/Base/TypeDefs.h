// Copyright 2025 Xenon Emulator Project

#pragma once

#include <stdint.h>

//Typedefs and "#define"s. Nothing else here.

/* Unsigned */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

/* Signed */
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#define BITMASK(width, index) (((u64)(1)) << ((width)-(index)-(1)))

#define BITGET(data,width,index) (((data) & BITMASK(width,index)) ? 1 : 0)
#define BITSET(data,width,index)    (data) |= BITMASK(width,index)

#define DMASK(b,e)                      (((0xFFFFFFFF << ((31+(b))-(e))) >> (b)))
#define QMASK(begin,end)                      ((0xFFFFFFFFFFFFFFFF << ((63+(begin))-(end))) >> (begin))
#define EXTS(data,input)     ((((u64)(data)) & (((u64)(1)) << ((input)-1))) ? (((u64)(data)) | QMASK(0,63-(input))) : ((u64)(data)))
#define QGET(data,begin,end)                    (((u64)(data) & QMASK((begin),(end))) >> (63-(end)))