/*
* Author: Biren Patel
* Description: A technique used in MacOS Sierra to generate unbiased bounded
* integers from a PRNG. I saw the c++ source on the PCG blog owned by Melissa
* O'Neil a while back and I was taken aback by its simplicity. I wanted to
* rewrite it from memory as a little exericse.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

/*******************************************************************************
* I realize that __builtin_clzll() exists. Even in pure C this can be knocked
* down to a few machine instructions via some tricks in Hacker's Delight, where
* you mask the lagging zeroes and simulate the popcnt instruction with a table.
*******************************************************************************/

int clz(uint64_t word)
{
    enum {WORD_SIZE = 64};
    int shifts = 0;

    while (word != 0)
    {
        word >>= 1;
        ++shifts;
    }

    return WORD_SIZE - shifts;
}

/*******************************************************************************
* Generate unbiased unsigned 64-bit integers in the range [0, max)
*******************************************************************************/

uint64_t unbiased_rand (uint64_t (*prng)(void), uint64_t max)
{
    if (max < 2 || prng == NULL) return 0;

    uint64_t mask = ~((uint64_t) 0) >> clz(--max);

    uint64_t rand;

    do
    {
        rand = prng() & mask;
    }
    while (rand > max);

    return rand;
}

/******************************************************************************/