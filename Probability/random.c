/*
* Author: Biren Patel
* Description: Implementation file for pseudo random number generator.
*/

#include "random.h"

#include <string.h>
#include <math.h>

/******************************************************************************/
//static prototypes

static uint64_t mix (uint64_t value);

/******************************************************************************/

#define CPUID(leaf, subleaf)                                                   \
        __asm__ volatile                                                       \
        (                                                                      \
            "cpuid"                                                            \
            : "=a" (reg.eax), "=b" (reg.ebx), "=c" (reg.ecx), "=d" (reg.edx)   \
            : "a" ((uint32_t) leaf), "c" ((uint32_t) subleaf)                  \
        )                                                                      \

int rng_verify_hardware(void)
{
    struct
    {
        uint32_t eax;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
    }
    reg = {.eax = 0, .ebx = 0, .edx = 0, .ecx = 0};

    //EAX 0 = manufacturer id formed by EBX-EDX-ECX
    CPUID(0,0);
    if (memcmp(&reg.ebx, "GenuineIntel", 12) != 0) return RNG_NO_INTEL;

    //impossible to execute RDSEED check on this machine
    if (reg.eax < 7) return RNG_MAX_EAX_PARAMETER_TOO_LOW;

    //EAX 1 = feature flags, bit 30 of ECX register must be set
    CPUID(1,0);
    if ((reg.ecx >> 30 & 1) == 0) return RNG_NO_RDRAND;

    //EAX 7 = extended features, bit 18 of EBX register must be set
    CPUID(7,0);
    if ((reg.ebx >> 18 & 1) == 0) return RNG_NO_RDSEED;

    return RNG_YES_RDRAND_RDSEED;
}

/******************************************************************************/

bool rng_rdseed64(uint64_t *seed, const uint8_t retry)
{
    if (seed == NULL) return false;

    uint8_t carry_flag;

    __asm__ volatile
    (
        "rdseed     %[seed]\n"
        "setc       %[carry_flag]\n"
        : [seed] "=r" (*seed), [carry_flag] "=q" (carry_flag)
    );

    //first request failed, now try again with pause instructions
    if (!carry_flag)
    {
        for (uint8_t i = 0; i < retry; ++i)
        {
            __asm__ volatile
            (
                "rdseed     %[seed]\n"
                "setc       %[carry_flag]\n"
                "pause\n"
                : [seed] "=r" (*seed), [carry_flag] "=q" (carry_flag)
            );

            if (carry_flag) break;
        }
    }

    return (bool) carry_flag;
}

/******************************************************************************/
//Marsaglia 64-bit Xorshift (temporary placeholder)

uint64_t rng_generator(uint64_t *state)
{
	uint64_t x = *state;
    
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
    
	return *state = x;
}


/******************************************************************************/
//invertible mix (temporary placeholder)

static uint64_t mix (uint64_t value)
{
    value = (value >> 48) | (value << 16);
    value ^= value << 30;
    value ^= value >> 15;
    value ^= value << 45;
    value = (value >> 32) | (value << 32);
    
    return value;
}

/******************************************************************************/

random_t rng_init(const uint64_t seed, const uint8_t retry)
{
    random_t rng;
    
    //set up prototypes
    rng.next = rng_generator;
    rng.rand = rng_rand;
    rng.bias = rng_bias;
    
    //set up seed
    if (seed == 0)
    {
        if (rng_rdseed64(&rng.state, retry) == false)
        {
            rng.state = 0;
        }
    }
    else
    {
        rng.state = mix(seed);
    }
    
    return rng;
}

/******************************************************************************/

uint64_t rng_rand(uint64_t *state, const uint64_t min, const uint64_t max)
{    
    //errors map to zero and degenerate cases cause early stopping
    if (state == NULL || max - min == 0) return 0;
    if (max - min < 2) return min;
    
    //sample a value from [0, scaled_max)
    uint64_t sample;
    uint64_t scaled_max = max - min - 1;
    uint64_t bitmask = ~((uint64_t) 0) >> __builtin_clzll(scaled_max);
    
    //rejection sampling on the generator output
    do 
    {
        sample = rng_generator(state) & bitmask;
    } 
    while (sample > scaled_max);
    
    //scale back to user range
    return sample + min;
}

/******************************************************************************/

uint64_t rng_bias (uint64_t *state, const uint8_t resolution)
{
    if (state == NULL) return 0;
    
    #define b(i) (rng_generator(state))
    
    switch(resolution)
    {
        case 0  : //0.0
                return (uint64_t) 0;
        case 1  : //0.00390625
                return b(1) & b(2) & b(3) & b(4) & b(5) & b(6) & b(7) & b(8);
        case 2  : //0.0078125
                return b(1) & b(2) & b(3) & b(4) & b(5) & b(6) & b(7);
        case 3  : //0.01171875
                return b(1) & b(2) & b(3) & b(4) & b(5) & b(6) & (b(7) | b(8));
        case 4  : //0.015625
                return b(1) & b(2) & b(3) & b(4) & b(5) & b(6);
        default : //use this for test cases
                return b(1) | (b(2) & (b(3) | b(4)));
    }
    
    #undef b
}
