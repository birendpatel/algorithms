/*
* Author: Biren Patel
* Description: PRNG library implementation
*/

#include "random.h"
#include "bit_array.h"

#include <string.h>
#include <assert.h>
#include <limits.h>
#include <immintrin.h>

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
Since this is a purely non-crypto library, I'm using rdrand instead of rdseed
because it is a) faster since it doesn't require a pass through an extrator for
full entropy and b) less or almost zero-prone to underflow. David Johnston
explains this well in 13.4 of "Random Number Generators". Per Intel docs, 10
max calls are attempted to rdrand before an error propogates back to the caller.
*/

random_t rng_init(const uint64_t seed)
{
    random_t rng;
    
    rng.next = rng_generator;
    rng.rand = rng_rand;
    rng.bias = rng_bias;
    rng.bino = rng_binomial;
    rng.vndb = rng_vndb;
    rng.cycc = rng_cyclic_autocorr;
    
    if (seed != 0) 
    {
        rng.state = mix(seed);
    }
    else
    {
        for (uint_fast8_t i = 0; i < 10; i++)
        {
            if (_rdrand64_step(&rng.state)) goto stop;
        }

        rng.state = 0;
        stop:;
    }
    
    return rng;
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
Von Neumann Debiaser for biased bits with no autocorrelation. Feed a low entropy
n-bit bitstream into the debiaser, get a high-entropy at-most-m-bit bitstream.
It may be that not all source bits are used and/or not all destination bits are
filled. The source is read as consecutive bit-pairs. The destination must be
zeroed out before the main loop since bitwise-or is used to set the bit array.
*/

stream_t rng_vndb (const void *src, void *dest, const uint64_t n, const uint64_t m)
{  
    assert(src != NULL && "null source");
    assert(dest != NULL && "null dest");
    assert(n != 0 && "nothing to read");
    assert(m != 0 && "nowhere to write");
    assert(n % 2 == 0 && "cannot process odd-length bitstream");
    
    bitarray_init(source, src, const unsigned char *);
    bitarray_init(destination, dest, unsigned char *);
    
    uint64_t write_pos = 0;
    uint64_t read_pos = 0;
    
    stream_t info = {.used = 0, .filled = 0};
    memset(destination, 0, (m-1)/CHAR_BIT + 1);
        
    while (read_pos < n)
    {        
        switch (bitarray_mask_at(source, CHAR_BIT, read_pos, 0x3))
        {
            case 1:
                bitarray_set(destination, CHAR_BIT, write_pos);
                write_pos++;
                break;
            case 2:
                write_pos++;
                break;
        }
        
        read_pos += 2;
        
        if (write_pos == m) goto destination_filled;
    }
    
    destination_filled:
        info.used = read_pos;
        info.filled = write_pos;
        return info;
}

/*******************************************************************************
Cyclic lag-K autocorrelation of an n-bit stream. This uses the SCC algorithm
from Donald Knuth as the base and adds the binary bit stream simplification
from David Johnston's "Random Number Generators". 
*/

double rng_cyclic_autocorr(const void *src, const uint64_t n, const uint64_t k)
{
    assert(src != NULL && "data pointer is null");
    assert(n != 0 && "no data");
    assert(k < n && "lag exceeds length of data");
    
    bitarray_init(source, src, const unsigned char *);
    
    uint64_t i = 0;
    uint64_t x1 = 0;
    uint64_t x2 = 0;
    
    while (i < n)
    {
        if (bitarray_test(source, CHAR_BIT, i))
        {            
            if (bitarray_test(source, CHAR_BIT, (i + k) % n))
            {
                x1++;
            }
            
            x2++;
        }
        
        i++;
    }
    
    double numerator = ((double) n * x1 - ((double) x2 * x2));
    double denominator = ((double) n * x2 - ((double) x2 * x2));
    
    assert(numerator/denominator >= -1.0 && "lower bound violation");
    assert(numerator/denominator <= 1.0 && "upper bound violation");
    
    return numerator/denominator;
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
Generate a number from a binomial distribution by simultaneous simulation of
64 iid bernoulli trials per loop.
*/

uint64_t rng_binomial(uint64_t *state, uint64_t k, const uint64_t n, const int m)
{
    assert(state != NULL && "generator state is null");
    assert(n != 0 && "probability is 0");
    assert(m > 0 && m <= 64 && "invalid base 2 exponent");
    assert(k != 0 && "no trials");
    
    uint64_t success = 0;
    
    for (; k > 64; k-= 64)
    {
        success += __builtin_popcountll(rng_bias(state, n, m));
    }
    
    return success + __builtin_popcountll(rng_bias(state, n, m) >> (64 - k));
}