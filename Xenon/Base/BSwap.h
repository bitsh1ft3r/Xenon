#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
// Return: 1 is LE, 0 is BE
static int test_endianess()
{
	uint32_t x = 1;
	return (int)*((char*)&x);
}
#define b8_swap(v) (v)
#define b16_swap(v) (test_endianess() ? (v) : ((v)>>8) | ((v)<<8))
#define b32_swap(v) (test_endianess() ? (v) : ((v)>>24) | (((v)>>8) & 0xFF00) | (((v)<<8) & 0xFF0000) | ((v)<<24))
#define b64_swap(v) (test_endianess() ? (v) : ((v)>>24) | (((v)>>8) & 0xFF00) | (((v)<<8) & 0xFF0000) | ((v)<<24))
#define b64_swap(v, cast) (cast)(test_endianess() ? (v) : \
  (((v) >> 56) | (((v) >> 40) & 0xFF00) | (((v) >> 24) & 0xFF0000) | (((v) >> 8) & 0xFF000000) | \
  (((v) << 8) & ((cast)0xFF << 32)) | (((v) << 24) & ((cast)0xFF << 40)) | (((v) << 40) & ((cast)0xFF << 48)) | \
  ((v) << 56)))

#define uint8(v) (uint8_t)(b8_swap(v))
#define int8(v) (int8_t)(b8_swap(v))
#define int16(v) (int16_t)(b16_swap(v))
#define uint16(v) (uint16_t)(b16_swap(v))
#define int32(v) (int32_t)(b32_swap(v))
#define uint32(v) (uint32_t)(b32_swap(v))
#define int64(v) b64_swap(v, int64_t)
#define uint64(v) b64_swap(v, uint64_t)

#ifdef __cplusplus
};
#endif