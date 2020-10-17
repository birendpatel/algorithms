/*
* Author: Biren Patel
* Description: 64-Bit SISD API
*/

#ifndef SISD_RANDOM_H
#define SISD_RANDOM_H

#include <stdint.h>
#include <immintrin.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>

/*******************************************************************************
* NAME: state_t
* DESC: internal state of the default PRNG
* @ current : state value used to generate PRNG values
* @ increment : stream identifier
*******************************************************************************/
typedef struct
{
    uint64_t current;
    uint64_t increment;
} state_t;
    
/*******************************************************************************
* NAME: stream_t
* DESC: return type for some of the functions which process bitstreams
* @ used : generally, the number of bits used from the input stream
* @ filled : generally, the number of bits written into the output stream
*******************************************************************************/
typedef struct
{
    uint64_t used;
    uint64_t filled;
} stream_t;

/*******************************************************************************
* NAME: random_t
* DESC: manage PRNG state and provide methods for API access
* @ state : must be seeded with rng_init() prior to any method calls.
* @ next : call to rng_generator()
* @ rand : call to rng_rand()
* @ bias : call to rng_bias()
* @ vndb : call to rng_vndb()
* @ cycc : call to rng_cyclic_autocorr()
* @ bino : call to rng_binomial()
*******************************************************************************/
typedef struct
{
    state_t state; 
    
    uint64_t (*next) 
    (
        state_t * const state
    );
    
    uint64_t (*rand) 
    (
        state_t * const state,
        const uint64_t min,
        const uint64_t max
    );
    
    uint64_t (*bias) 
    (
        state_t * const state,
        const uint64_t n,
        const int m
    );
    
    stream_t (*vndb) 
    (
        const uint64_t * restrict src, 
        uint64_t * restrict dest, 
        const uint64_t n, 
        const uint64_t m
    );
    
    double (*cycc) 
    (
        const uint64_t *src, 
        const uint64_t n, 
        const uint64_t k
    );
    
    uint64_t (*bino) 
    (
        state_t * const state, 
        uint64_t k, 
        const uint64_t n, 
        const int m
    );
    
} random_t;

/*******************************************************************************
* NAME: rng_init
* DESC: initialize a variable of type random_t
* OUTP: type random_t where zero state and increment indicates rdrand failure.
* @ seed : set seed = 0 to use the x86 rdrand instruction.
*******************************************************************************/
random_t rng_init(const uint64_t seed);

/*******************************************************************************
* NAME: rng_generator
* DESC: generate a psuedo random number via the default PRNG.
* OUTP: random number not guaranteed equal to the updated state parameter.
*******************************************************************************/
uint64_t rng_generator(state_t * const state);

/*******************************************************************************
* NAME: rng_rand
* DESC: generate an unbiased psuedo random number
* OUTP : 0 if null state. non-null state will be updated
* @ min : inclusive lower bound
* @ max : inclusive upper bound
*******************************************************************************/
uint64_t rng_rand(state_t * const state, const uint64_t min, const uint64_t max);

/*******************************************************************************
* NAME: rng_bias
* DESC: simultaneous generation of 64 iid bernoulli trials 
* OUTP: 64-bit word where each bit has probability p = n/2^m of success
* NOTE: m limits the total calls to rng_generator, so smaller m is faster code
* @ n : nonzero numerator of probability, strictly less than 2^m
* @ m : nonzero base 2 exponent less than or equal to 64
*******************************************************************************/
uint64_t rng_bias (state_t * const state, const uint64_t n, const int m);

/*******************************************************************************
* NAME: rng_vndb
* DESC: Von Neumann Debiaser for iid biased bits with zero autocorrelation
* OUTP: dest is filled with stream_t.filled bits, which used stream_t.used bits
* NOTE: dest is not guaranteed to be filled to capacity
* @ src : binary bit stream of length n bits
* @ dest : binary bit stream of length m bits
*******************************************************************************/
stream_t rng_vndb 
(
    const uint64_t * restrict src, 
    uint64_t * restrict dest, 
    const uint64_t n, 
    const uint64_t m
);

/*******************************************************************************
* NAME: rng_cyclic_autocorr
* DESC: calculate the cyclic autocorrelation of an n-bit binary bitstream
* OUTP: lag-k cyclic correlation in inclusive range [-1.0, 1.0]
* @ src : binary bit stream of length n bits
* @ k : autocorrelation lag not exceeding total bits n
*******************************************************************************/
double rng_cyclic_autocorr
(
    const uint64_t *src, 
    const uint64_t n, 
    const uint64_t k
);

/*******************************************************************************
* NAME: rng_binomial
* DESC: sample from a binomial distribution X~(k,p) where p = n/2^m
* OUTP: number of successful trials
* @ k : total trials
* @ n : nonzero numerator of probability, strictly less than 2^m
* @ m : nonzero base 2 exponent less than or equal to 64
*******************************************************************************/
uint64_t rng_binomial
(
    state_t * const state, 
    uint64_t k, 
    const uint64_t n, 
    const int m
);

#endif