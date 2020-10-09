/*
* Author: Biren Patel
* Description: PRNG library implementation
*/

#include "random.h"

#include <string.h>
#include <assert.h>

/******************************************************************************/
//prototypes

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

    //request failed, try again with pauses
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

/*******************************************************************************
Marsaglia 64-bit Xorshift (temporary placeholder)
*/

uint64_t rng_generator(uint64_t *state)
{
	uint64_t x = *state;
    
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
    
	return *state = x;
}


/*******************************************************************************
invertible mix (temporary placeholder)
*/

static uint64_t mix (uint64_t value)
{
    value = (value >> 48) | (value << 16);
    value ^= value << 30;
    value ^= value >> 15;
    value ^= value << 45;
    value = (value >> 32) | (value << 32);
    
    return value;
}

/*******************************************************************************
The only potential point of failure on initialization is rdseed, but this would
be a rare situation, especially on single threading.
*/

random_t rng_init(const uint64_t seed, const uint8_t retry)
{
    random_t rng;
    
    rng.next = rng_generator;
    rng.rand = rng_rand;
    rng.bias = rng_bias;
    rng.bino = rng_binomial;
    
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

/*******************************************************************************
Bitmask rejection sampling technique that Apple uses in their 2008 arc4random C 
source. I made minor adjustments for a variable lower bound and inclusive upper 
bound. I also throw away the random number after failure instead of attempting 
to use the upper bits.
*/

uint64_t rng_rand(uint64_t *state, const uint64_t min, const uint64_t max)
{    
    assert(state != NULL && "generator state is null");
    assert(min < max && "bounds violation");
    
    uint64_t sample;
    uint64_t scaled_max = max - min;
    uint64_t bitmask = ~((uint64_t) 0) >> __builtin_clzll(scaled_max);
    
    assert(__builtin_clzll(bitmask) == __builtin_clzll(scaled_max) && "bad mask");
    assert(__builtin_popcountll(bitmask) == 64 - __builtin_clzll(scaled_max) && "bad mask");
    
    do 
    {
        sample = rng_generator(state) & bitmask;
    } 
    while (sample > scaled_max);
    
    assert(sample <= scaled_max && "scaled bounds violation");
    
    return sample + min;
}

/*******************************************************************************
This function uses a virtual machine to interpret a portion of the bit pattern
in the numerator parameter as executable bitcode. I wrote a short essay at url
https://stackoverflow.com/questions/35795110/ (username Ollie) to demonstrate
the concepts using 256 bits of resolution.
*/

uint64_t rng_bias (uint64_t *state, const uint64_t n, const int m)
{
    assert(state != NULL && "generator state is null");
    assert(n != 0 && "probability is 0");
    assert(m > 0 && m <= 64 && "invalid base 2 exponent");
    
    uint64_t accumulator = 0;
    
    for (int pc = __builtin_ctzll(n); pc < m; pc++)
    {
        assert(((((n >> pc) & 1) == 0) || (((n >> pc) & 1) == 1)) && "opcode");
        
        switch ((n >> pc) & 1)
        {
            case 0:
                accumulator &= rng_generator(state);
                break;
                
            case 1:
                accumulator |= rng_generator(state);
                break;
        }
    }
    
    return accumulator;
}

/*******************************************************************************
Von Neumann Debiaser for biased bits with no autocorrelation. Feed a low
entropy n-byte stream into the debiaser, return a high-entropy at-most-m-byte
stream. Source blocks are fed in until enough entropy is extracted to fill a
byte. The byte is written to destination and the process repeats. Partially
filled blocks at the end are implicitly discarded.
*/

uint64_t rng_debiaser(void *src, const uint64_t n, void *dest, const uint64_t m)
{
    assert (m <= n && "extra bytes in destination cannot be filled");
    assert (n != 0 && "no source blocks for read");
    assert (m != 0 && "no destination blocks for write");
    
    unsigned char *src_block  = (unsigned char *) src;
    unsigned char *dest_block = (unsigned char *) dest;
    
    uint64_t total = 0;    
    uint64_t byte_pos = 0;
    unsigned char bit_pos = 0;
    unsigned char out = 0;
    
    for (uint64_t i = 0; i < n; i++)
    {
        for (unsigned char curr = src_block[i]; curr != 0; curr >>= 2)
        {
            switch (curr & 3)
            {
                case 1:
                    out |= (1U << bit_pos++);
                    break;
                case 2:
                    bit_pos++;
                    break;
            }
            
            if (bit_pos == 8)
            {
                dest_block[byte_pos++] = out;
                total++;
                bit_pos = 0;
                out = 0;
                
                if (byte_pos == m) goto dest_filled;
            }
        }
    }
    
    dest_filled:
    return total;
}


/*******************************************************************************
Generate a number from a binomial distribution by simultaneous simulation of
64 iid bernoulli trials per loop.
*/

uint64_t rng_binomial(uint64_t *state, uint64_t k, const uint64_t n, const int m)
{
    assert(state != NULL && "generator state is null");
    assert(n != 0 && "probability is 0");
    assert(m > 0 && m <= 64 && "invalid base 2 exponent");
    
    uint64_t success = 0;
    
    for (; k > 64; k-= 64)
    {
        success += __builtin_popcountll(rng_bias(state, n, m));
        assert(success < k && "generated too many bernoulli trials");
    }
    
    assert(k <= 64 && "bad loop conditions");
    
    success += __builtin_popcountll(rng_bias(state, n, m) >> (64 - k));
    
    assert(success <= k && "generated too many remainder bernoulli trials");
    
    return success;
}