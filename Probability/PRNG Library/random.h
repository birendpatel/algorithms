/*
* Author: Biren Patel
* Description: PRNG library for non-cryptographic purposes. This library depends
* on GCC builtins and x86 RDRAND. Since the functions are performance critical,
* little to no error handling is performed. Assertion density is high, but in
* optimized mode the user should check the input parameters wherever necessary.
*/

#ifndef RANDOM_H
#define RANDOM_H

#include <stdbool.h>
#include <stdint.h>

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
* @ state : must be seeded prior to any method calls.
* @ next : call to rng_generator()
* @ rand : call to rng_rand()
* @ bias : call to rng_bias()
* @ bino : call to rng_binomial()
* @ vndb : call to rng_vndb()
*******************************************************************************/
typedef struct
{
    uint64_t state;
    uint64_t (*next) (uint64_t *state);
    uint64_t (*rand) (uint64_t *state, const uint64_t min, const uint64_t max);
    uint64_t (*bias) (uint64_t *state, const uint64_t n, const int m);
    uint64_t (*bino) (uint64_t *state, uint64_t k, const uint64_t n, const int m);
    stream_t (*vndb) (const void *src, void *dest, const uint64_t n, const uint64_t m);
    double   (*cycc) (const void *src, const uint64_t n, const uint64_t k);
} random_t;

/*******************************************************************************
* NAME: rng_init
* DESC: initialize a variable of type random_t
* OUTP: type random_t where zero state indicates rdrand failure.
* @ seed : set seed = 0 to use the x86 rdrand instruction.
*******************************************************************************/
random_t rng_init(const uint64_t seed);

/*******************************************************************************
* NAME: rng_rdseed64
* DESC: generate 64-bit seed using the x86 RDSEED instruction
* OUTP: false on failure
* @ seed : contains a valid seed only if the function returns true
* @ retry : maximum attempts to retry instruction if the first attempt failed
*******************************************************************************/
bool rng_rdseed64(uint64_t *seed, const uint8_t retry);

/*******************************************************************************
* NAME: rng_generator
* DESC: generate a psuedo random number via the default PRNG.
* OUTP: random number not guaranteed equal to the updated state parameter.
*******************************************************************************/
uint64_t rng_generator(uint64_t *state);

/*******************************************************************************
* NAME: rng_rand
* DESC: generate an unbiased psuedo random number
* OUTP : 0 if null state. non-null state will be updated
* @ min : inclusive lower bound
* @ max : inclusive upper bound
*******************************************************************************/
uint64_t rng_rand(uint64_t *state, const uint64_t min, const uint64_t max);

/*******************************************************************************
* NAME: rng_bias
* DESC: simultaneous generation of 64 iid bernoulli trials 
* OUTP: 64-bit word where each bit has probability p = n/2^m of success
* @ n : nonzero numerator of probability, less than 2^m
* @ m : nonzero denominator indicating exponent of base 2 not exceeding 64.
*******************************************************************************/
uint64_t rng_bias (uint64_t *state, const uint64_t n, const int m);

/*******************************************************************************
* NAME: rng_vndb
* DESC: Von Neumann Debiaser for iid biased bits with zero autocorrelation
* OUTP: dest is filled with stream_t.filled bits, which used stream_t.used bits
* @ src : bitstream containing a capacity of at least n bits
* @ dest : bitstream containing a capacity of at least m bits
*******************************************************************************/
stream_t rng_vndb (const void *src, void *dest, const uint64_t n, const uint64_t m);

/*******************************************************************************
* NAME: rng_cyclic_autocorr
* DESC: calculation cyclic autocorrelation of an n-bit binary bitstream.
* OUTP: lag-k cyclic correlation in inclusive range [-1.0, 1.0].
* @ k : autocorrelation lag
*******************************************************************************/
double rng_cyclic_autocorr(const void *src, const uint64_t n, const uint64_t k);

/*******************************************************************************
* NAME: rng_binomial
* DESC: generate random numbers from a binomial distribution
* OUTP: number of successful trials
* @ k : total trials
* @ n : nonzero numberator probability, p = n/2^m, not exceeding 2^m
* @ m : nonzero denominator indicating exponent of base 2 not exceeding 64.
*******************************************************************************/
uint64_t rng_binomial(uint64_t *state, uint64_t k, const uint64_t n, const int m);

#endif