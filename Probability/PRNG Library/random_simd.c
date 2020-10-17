/*
* Author: Biren Patel
* Description: PRNG library implementation
*/

#include "random_simd.h"

/*******************************************************************************
Retry loop for RDRAND x86 instruction. Per the Intel documentation, we give up 
after ten failures.
*/

static bool rdrand(uint64_t *x)
{
    for (size_t i = 0; i < 10; i++)
    {
        if (_rdrand64_step(x))
        {
            return true;
        }
    }
    
    return false;
}

/*******************************************************************************
This is used to mix a user-supplied seed, it is Sebastiano Vigna's version of
Java's SplittableRandom: http://xoshiro.di.unimi.it/splitmix64.c but since its
just a one-off mixing function I removed the state increment that Vigna employs.
*/

static uint64_t rng_hash 
(
    uint64_t value
)
{
    value ^=  value >> 30;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27;
    value *= 0x94d049bb133111ebULL;
    value ^= value >> 31;
    
    return value;
}

/*******************************************************************************
Permuted Congruential Generator from Melissa O'Neill. This is the insecure 32
bit output PCG extracted from O'Neill's C implementation on pcg_random.org.
The major change that I have committed is to refactor the generator to use Intel
AVX2 instruction set intrinsics. In this manner, four independent streams can be
executed simultaneously. A decorator runs each stream twice so that in total we
output 256 bits over two calls.

pcg_setseq_32_rxs_m_xs_32_random_r (1686)
pcg_setseq_32_step_r (514)
pcg_output_rxs_m_xs_32_32 (184)
*/

__m256i simd_rng_generator 
(
    simd_state_t * const state
)
{
    const __m256i lcg_mult = _mm256_set1_epi64x((int64_t) 747796405U);
    const __m256i rxs_mult = _mm256_set1_epi64x((int64_t) 277803737U);
    const __m256i mod32_mask = _mm256_set1_epi64x((int64_t) 0xFFFFFFFFU);
    
    //generate the output permutation of all four current states
    __m256i x = state->current;
    __m256i fx;
    
    fx = _mm256_add_epi64(_mm256_srli_epi64(x, 28), _mm256_set1_epi64x(4LL));
    fx = _mm256_srlv_epi64(x, fx);
    fx = _mm256_xor_si256(x, fx);
    fx = _mm256_mul_epu32(fx, rxs_mult);
    fx = _mm256_and_si256(fx, mod32_mask);
    fx = _mm256_xor_si256(_mm256_srli_epi64(fx, 22), fx);    
    
    //advance all four internal states to the next step using 32-Bit LCGs    
    state->current = _mm256_mul_epu32(state->current, lcg_mult);
    state->current = _mm256_and_si256(state->current, mod32_mask);
    state->current = _mm256_add_epi64(state->current, state->increment);
    state->current = _mm256_and_si256(state->current, mod32_mask);
    
    //return the four permutation outputs of the previous state
    return fx;
}

/*******************************************************************************
This it the initialization function for the AVX API. ALmost the same as 64-Bit
but 4 seed parameters make the initialization of each PCG stream easier. It also
allows for easier debugging as we can initialize a non-SIMD PCG32i and follow
each "thread" individually. Since RDRAND needs up to 10 retries, we have a nasty
but simple if-chain to perform the 80 total retries in 10-try blocks. 
*/

simd_random_t simd_rng_init
(
    const uint64_t seed_1,
    const uint64_t seed_2,
    const uint64_t seed_3,
    const uint64_t seed_4
)
{
    simd_random_t simd_rng;
    
    uint64_t LL;
    uint64_t LH;
    uint64_t HL;
    uint64_t HH;
    
    if (seed_1 != 0 && seed_2 != 0 && seed_3 != 0 && seed_4 != 0)
    {
        LL = rng_hash(seed_1);
        LH = rng_hash(seed_2);
        HL = rng_hash(seed_3);
        HH = rng_hash(seed_4);
        
        simd_rng.state.current = _mm256_set_epi64x
        (
            (int64_t) (LL >> 32),
            (int64_t) (LH >> 32),
            (int64_t) (HL >> 32),
            (int64_t) (HH >> 32)                          
        );
             
        LL = rng_hash(LL);
        LH = rng_hash(LH);
        HL = rng_hash(HL);
        HH = rng_hash(HH);
                         
        simd_rng.state.increment = _mm256_set_epi64x
        (
            (int64_t) ((LL >> 32) | 1),
            (int64_t) ((LH >> 32) | 1),
            (int64_t) ((HL >> 32) | 1),
            (int64_t) ((HH >> 32) | 1)
        );
    }
    else
    {
        if (rdrand(&LL))
        {
            if (rdrand(&LH))
            {
                if (rdrand(&HL))
                {
                    if (rdrand(&HH))
                    {
                        simd_rng.state.current = _mm256_set_epi64x
                        (
                            (int64_t) (LL >> 32),
                            (int64_t) (LH >> 32),
                            (int64_t) (HL >> 32),
                            (int64_t) (HH >> 32)                          
                        );
                        
                        goto first_pass_success;
                    }
                }
            }
        }
        goto fail;
        
        first_pass_success:
        
        if (rdrand(&LL))
        {
            if (rdrand(&LH))
            {
                if (rdrand(&HL))
                {
                    if (rdrand(&HH))
                    {
                        simd_rng.state.increment = _mm256_set_epi64x
                        (
                            (int64_t) ((LL >> 32) | 1),
                            (int64_t) ((LH >> 32) | 1),
                            (int64_t) ((HL >> 32) | 1),
                            (int64_t) ((HH >> 32) | 1)
                        );
                        
                        goto success;
                    }
                }
            }
        }
        
        fail:
            simd_rng.state.current = _mm256_setzero_si256();
            simd_rng.state.increment = _mm256_setzero_si256();
            goto terminate;
    }
    
    success:
        simd_rng.next = simd_rng_generator;
    
    terminate:
        return simd_rng;
}