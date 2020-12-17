/*
* NAME: Copyright (c) 2020, Biren Patel
* DESC: subroutines for psuedo random number generation
* LISC: MIT License
*/

#include "random.h"

#include <assert.h>
#include <immintrin.h>
#include <stdbool.h>
#include <stdlib.h>


/*******************************************************************************
prototypes for all random sampling subroutines
*/

uint64_t random_next_pcg64_insecure(void *state);
uint64_t random_int_64(generator_64bit rng, const uint64_t min, const uint64_t max);
uint64_t random_bernoulli_64(generator_64bit rng, const uint64_t n, const int m);
uint64_t random_binomial_64(generator_64bit rng, uint64_t k, const uint64_t n, const int m);

/*******************************************************************************
Sebastiano Vigna's version of Java SplittableRandom. This is used as a one-off 
mixing function for seeding, so the state increment from Vigna's original code
is removed in favor of an overwriting call by reference.

http://xoshiro.di.unimi.it/splitmix64.c
http://prng.di.unimi.it/splitmix64.c
*/

static uint64_t random_hash(uint64_t *value)
{    
    uint64_t i = *value;
    
    i ^=  i >> 30;
    i *= 0xbf58476d1ce4e5b9ULL;
    i ^= i >> 27;
    i *= 0x94d049bb133111ebULL;
    i ^= i >> 31;
    
    *value = i;
    
    return i;
}

/*******************************************************************************
the rdrand instruction is used to seed all generators when non-deterministic
behavior is requested. Per Intel documentation, the rdrand instruction must be 
retried ten times on the rare chance that underflow occurs. rdrand itself can
be used as an underlying generator. The ULL cast silences some GCC warnings.
*/

static bool random_try_rdrand(uint64_t *x, size_t limit)
{    
    for (size_t i = 0; i < limit; i++)
    {
        if (_rdrand64_step((unsigned long long *) x))
        {
            return true;
        }
    }
    
    return false;
}

/*******************************************************************************
These are the actual generators hiding underneath the abstract generator_64bit
object. During initialization, we assign one generator to the state member and
hook the corresponding next() function. This polymorphism helps to simplify the 
API. Note that each generator needs its own seed() and next() functions.
*/

#define SIZEOF_GENERATOR_64BIT (sizeof(struct generator_64bit))

struct rdrand64
{
    uint64_t dummy;
};
#define SIZEOF_RDRAND64 (sizeof(struct rdrand64))

struct pcg64_insecure
{
    uint64_t state;
    uint64_t increment;
};
#define SIZEOF_PCG64_INSECURE (sizeof(struct pcg64_insecure))

struct xorshift64
{
    uint64_t state;
};
#define SIZEOF_XORSHIFT64 (sizeof(struct xorshift64))

/*******************************************************************************
PCG64 insecure seeding. The increment must be odd. This library is non-crypto 
so we use the faster rdrand instruction to avoid the rdseed extractor and to 
minimize risk of underflow. This function demonstrates the general init approach
for all generators:

1. allocate the abstract interface and pocket the generator just in front of it
2. seed the internal state
3. hook the callbacks, where next() is unique to each generator
*/

generator_64bit random_init_pcg64_insecure(uint64_t seed, int *error)
{
    //allocate both interface and generator together for cache locality
    int bytes = SIZEOF_PCG64_INSECURE + SIZEOF_GENERATOR_64BIT;
    struct pcg64_insecure * pcg64i = malloc(bytes);
    
    if (!pcg64i)
    {
        if (error) *error = RANDOM_MALLOC_FAIL;
        return NULL;
    }
    
    struct generator_64bit *g64b = (void*) ((char*) pcg64i + SIZEOF_PCG64_INSECURE);
    
    //seed the generator
    if (seed != 0)
    {
        pcg64i->state = random_hash(&seed);
        pcg64i->increment = random_hash(&seed);
    }
    else
    {
        if (!random_try_rdrand(&pcg64i->state, 10))
        {
            if (error) *error = RANDOM_RDRAND_FAIL;
            return NULL;
        }
        
        if (!random_try_rdrand(&pcg64i->increment, 10))
        {
            if (error) *error = RANDOM_RDRAND_FAIL;
            return NULL;
        }
    }
    
    pcg64i->increment |= 1;
    
    //hook pcg into the interface
    g64b->state = (void*) pcg64i;
    g64b->next = random_next_pcg64_insecure;
    g64b->rint = random_int_64;
    g64b->bern = random_bernoulli_64;
    g64b->bino = random_binomial_64;
    
    //drop the pcg handle and return the genreator, use state ptr to recover
    if (error) *error = RANDOM_SUCCESS;
    return g64b;    
}

/*******************************************************************************
The following function is originally Copyright 2014 Melissa O'Neill, which is
licensed under the Apache License, Version 2.0. It is the default insecure
64-bit output PCG denoted commonly as pcg_output_rxs_m_xs_64_64. This function
exhibits several modifications.

1. The original seeding mechanism via either dev/urandom or address space
layout randomization has been replaced with x86 rdrand and/or seed hashing.

2. The preprocssor macros from O'Neill have been replaced, decimal constants
were swapped for hex, and variable names have been changed. These modifications
are made to engender simplicity.
*/

uint64_t random_next_pcg64_insecure(void *state)
{
    struct pcg64_insecure *pcg64i = state;
    
    uint64_t x = pcg64i->state;

    pcg64i->state = pcg64i->state * 0x5851F42D4C957F2DULL + pcg64i->increment;

    uint64_t fx = ((x >> ((x >> 59ULL) + 5ULL)) ^ x) * 0xAEF17502108EF2D9ULL;

    return (fx >> 43ULL) ^ fx;
}

/*******************************************************************************
Bitmask rejection sampling lifted from the Applce 2008 arc4random C source with
minor modifications. A variable lower bound is introduced and there is an
immediate rejection of the full 64-bit output after the first failure, rather
than attempting to use the upper remaining bits.
*/

uint64_t random_int_64(generator_64bit rng, const uint64_t min, const uint64_t max)
{
    uint64_t outp = 0;
    uint64_t ceil = max - min;
    uint64_t mask = ~((uint64_t) 0) >> __builtin_clzll(ceil);
    
    do
    {
        outp = rng->next(rng->state) & mask;
    }
    while (outp > ceil);
    
    return outp + min;
}

/*******************************************************************************
This function uses a virtual machine to simultaneously generate 64 iid bernoulli
trials without the SIMD instruction set. I wrote a short essay at the following
url: https://stackoverflow.com/questions/35795110/ (username Ollie) which uses
256 bit resolution to demonstrate the main concepts.

To give some additional detail on why this works, consider a binary tree with
double values where all left children are half the value of the parent, and all
right children are P + (.5 - .5P). With a root value of 0.5, the traversal path
taken to a desired probability maps precisely to a compact binary representation
when AND=0 and OR=1.

                                    ORI
                                     |
                                     v
                                    0.5
                                     |
                  _______AND_________|________OR________
                 |                                      |
                .25                                    .75
                 |                                      |
      ____AND____|____OR____                 ____AND____|____OR____
     |                      |               |                      |
   .125                   .625            .325                   .875
     |                      |               |                      |
     |                      |               |                      |
    ...                    ...             ...                    ...
    
Therefore, for some p = n/(2^m), n unpacks from its compact binary and can be
read from the first nonzero LSB to MSB as the exact traversal starting at the
root node.

So given a generator that outputs unbiased bits, we can psuedo-vectorized the
production of biased bits with at most log2(n) calls to the generator for some
resolution n. Although this is more wasteful from an information theoretic view
than running an arithmetic decoder, it is almost certainly much faster.

In probablity, nodes map to some event given a sequence of bernoulli trials.
i.e,. .875 is the event of at least one success. 0.625 is the event where either
the first two trials are both successful, or the final trial is successful.
*/

uint64_t random_bernoulli_64(generator_64bit rng, const uint64_t n, const int m)
{
    uint64_t accumulator = 0;

    for (int pc = __builtin_ctzll(n); pc < m; pc++)
    {
        switch ((n >> pc) & 1)
        {
            case 0:
                accumulator &= rng->next(rng->state);
                break;

            case 1:
                accumulator |= rng->next(rng->state);
                break;
        }
    }

    return accumulator;
}

/*******************************************************************************
Generate a number from a binomial distribution by simultaneous simulation of
64 iid bernoulli trials per loop.
*/

uint64_t random_binomial_64(generator_64bit rng, uint64_t k, const uint64_t n, const int m)
{
    uint64_t success = 0;
    uint64_t trials = 0;

    for (; k > 64; k -= 64)
    {
        trials = random_bernoulli_64(rng, n, m);
        success += (uint64_t) __builtin_popcountll(trials);
    }

    trials = random_bernoulli_64(rng, n, m) >> (64 - k);
    success += (uint64_t) __builtin_popcountll(trials);
    
    return success;
}

int main(void)
{
    return 0;
}