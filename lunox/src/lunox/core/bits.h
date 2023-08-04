#ifndef LNX_BITS_H
#define LNX_BITS_H

#include <stdint.h>

#define LNX_BIT(x) (1ull << (x))
#define LNX_BIT_GET(x, bit) (((x) << (sizeof(x) * 8 - 1 - (bit))) >> (sizeof(x) * 8 - 1))
#define LNX_BIT_SET(x, bit) ((x) |= (bit))
#define LNX_BIT_CLEAR(x, bit) ((x) &=  ~(bit))
#define LNX_BIT_TOGGLE(x, bit) ((x) ^= (bit))

#ifdef __GNUC__
#define LNX_BIT_LSB_INDEX(x) __builtin_ctzll(x)
#else
uint8_t lsb_index(uint64_t x);
#define LNX_BIT_LSB_INDEX(x) lsb_index(x)
#endif

#define LNX_BIT_LSB_RESET(x) (x &= (x) - 1)

#if defined(__clang__) || defined(__GNUC__)
#define LNX_BIT_COUNT(x) __builtin_popcountll(x)
#else
uint8_t bit_count(uint64_t x);
#define LNX_BIT_COUNT(x) bit_count(x)
#endif

#endif