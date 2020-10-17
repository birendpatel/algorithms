/*
* Author: Biren Patel
* Description: PRNG library implementation
*/

#include "random.h"

/*******************************************************************************
Permuted Congruential Generator from Melissa O'Neill. This is the insecure 32
bit output PCG extracted from O'Neill's C implementation on pcg_random.org.
The major change that I have committed is to refactor the generator to use Intel
AVX2 instruction set intrinsics. In this manner, four independent streams can be
executed simultaneously. A decorator runs each stream twice so that in total we
output 256 bits. Each 64 bit block is generated by one stream, where the first
call is contained in the lower 32 bits, and the second call in the upper 32.
*/

static __m256i simd_rng_generator_partial
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

__m256i simd_rng_generator
(
    simd_state_t * const state
)
{
    __m256i lower;
    __m256i upper;
    __m256i output;
    
    lower = simd_rng_generator_partial(state);
    lower = _mm256_slli_epi64(lower, 32);
    
    upper = simd_rng_generator_partial(state);
    
    output = _mm256_or_si256(upper, lower);
    
    return output;
}

/*******************************************************************************
This it the initialization function for the AVX API. ALmost the same as 64-Bit
but 4 seed parameters make the initialization of each PCG stream easier. It also
allows for easier debugging as we can initialize a non-SIMD PCG32i and follow
each "thread" individually. See the unit tests for an example. Since RDRAND 
needs up to 10 retries, we have a nasty but simple if-chain to perform the 80 
total retries across 10-try blocks. 
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
    const __m256i mask = _mm256_set1_epi64x((int64_t) 0xFFFFFFFFU);
    
    uint64_t LL;
    uint64_t LH;
    uint64_t HL;
    uint64_t HH;
    
    if (seed_1 != 0 && seed_2 != 0 && seed_3 != 0 && seed_4 != 0)
    {
        LL = rng_hash(seed_4);
        LH = rng_hash(seed_3);
        HL = rng_hash(seed_2);
        HH = rng_hash(seed_1);
        
        simd_rng.state.current = _mm256_set_epi64x
        (
            (int64_t) LL,
            (int64_t) LH,
            (int64_t) HL,
            (int64_t) HH                          
        );
             
        LL = rng_hash(LL);
        LH = rng_hash(LH);
        HL = rng_hash(HL);
        HH = rng_hash(HH);
                         
        simd_rng.state.increment = _mm256_set_epi64x
        (
            (int64_t) (LL | 1),
            (int64_t) (LH | 1),
            (int64_t) (HL | 1),
            (int64_t) (HH | 1)
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
                            (int64_t) LL,
                            (int64_t) LH,
                            (int64_t) HL,
                            (int64_t) HH                          
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
                            (int64_t) (LL | 1),
                            (int64_t) (LH | 1),
                            (int64_t) (HL | 1),
                            (int64_t) (HH | 1)
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
        simd_rng.state.current = _mm256_and_si256(simd_rng.state.current, mask);
        simd_rng.state.increment = _mm256_and_si256(simd_rng.state.increment, mask);
        simd_rng.next = simd_rng_generator;
    
    terminate:
        return simd_rng;
}