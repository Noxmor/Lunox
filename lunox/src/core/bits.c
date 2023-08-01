#include "bits.h"

uint8_t bit_count(uint64_t x)
{
    uint8_t count = 0;
    while(x)
    {
        x &= x - 1;
        ++count;
    }

    return count;
}