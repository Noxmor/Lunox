#include "bits.h"

#include "lunox/core/core.h"

uint8_t lsb_index(uint64_t x)
{
    uint8_t index = 0;

    while(x)
    {
        if(x & 1)
            return index;

        x >>= 1;
        ++index;
    }

    LNX_ASSERT(LNX_FALSE);

    return 64;
}

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