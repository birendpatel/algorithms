/*
* Author: Biren Patel
* Description: 256-Bit SIMD API (AVX/AVX2 Instruction Sets)
*/

#ifndef RANDOM_H
#error "include random.h instead of random_simd.h"
#endif

#ifndef SIMD_RANDOM_H
#define SIMD_RANDOM_H

#include <stdint.h>
#include <immintrin.h>
#include <assert.h>

/*******************************************************************************
* NAME: simd_state_t
* DESC: internal state of the default vectorized PRNG
* @ current : contains state of 4 streams in lower 32 bits of each 64 bit block
* @ increment : contain stream identifiers in lower 32 bits of each 64 bit block
*******************************************************************************/
typedef struct
{
    __m256i current;
    __m256i increment;
} simd_state_t;

/*******************************************************************************
* NAME: simd_random_t
* DESC: manage PRNG state and provide methods for API access
* @ state : must be seeded with simd_rng_init() prior to any method calls
* @ next : call to simd_rng_generator()
*******************************************************************************/
typedef struct
{
    simd_state_t state;
    
    __m256i (*next)
    (
        simd_state_t * const_state
    );
    
    char buffer[24]; //temporary padding marker for -Wpadded
} simd_random_t;

/*******************************************************************************
* NAME: simd_rng_init
* DESC: initialize a variable of type simd_random_t
* OUTP: zero state and increment in return type indicates rdrand failure
* @ seed : If any seed is zero, then all four streams will be non-determinstic
*******************************************************************************/
simd_random_t simd_rng_init
(
    const uint64_t seed_1,
    const uint64_t seed_2,
    const uint64_t seed_3,
    const uint64_t seed_4
);

/*******************************************************************************
* NAME: simd_rng_generator
* DESC: generate 256-Bit psuedo random numbers via the default PRNG
* OUTP: random numbers not guaranteed equal to the updated state parameters
*******************************************************************************/
__m256i simd_rng_generator (simd_state_t * const state);

#endif
