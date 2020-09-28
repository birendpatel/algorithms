/*
* Author: Biren Patel
* Description: Perform a reverse xor + shift operation. In other words, find
* the output of the inverse function of f(x) = x ^ (x >> CONSTANT). e.g., for
* x ^= x >> 18, rxs64 calculates x ^= (x >> 18) ^ (x >> 36) ^ (x >> 54).
*/

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

uint64_t rxs64 (const uint64_t input, const uint64_t rshift)
{
    enum {SIZEOF_WORD = 64};
    uint64_t output = 0;
    uint64_t base = rshift;

    //0 shift is not invertible and shift > 63 is undefined behavior
    if (rshift > 0 && rshift < SIZEOF_WORD)
    {
        while (base < SIZEOF_WORD)
        {
            output ^= (input >> base);
            base += rshift;
        }

        output ^= input;
    }

    return output;
}

/******************************************************************************/
//A few quick tests

int main(void)
{
    uint64_t x;

    x = 0xAF003CB764A8D87E;
    x ^= x >> 43;
    assert(rxs64(x, 43) == 0xAF003CB764A8D87E);

    x = 0xAF003CB764A8D87E;
    x ^= x >> 0;
    assert(rxs64(x, 0) == 0);

    x = UINT64_MAX;
    x ^= x >> 16;
    assert(rxs64(x, 16) == UINT64_MAX);

    x = UINT64_MAX;
    x ^= x >> 63;
    assert(rxs64(x, 63) == UINT64_MAX);

    x = UINT64_MAX;
    x ^= x >> 32;
    assert(rxs64(x, 32) == UINT64_MAX);

    x = 0;
    x ^= x >> 63;
    assert(rxs64(x, 63) == 0);

    x = 0xFFFFFFFF;
    x ^= x >> 16;
    assert(rxs64(x, 16) == 0xFFFFFFFF);

    return 0;
}