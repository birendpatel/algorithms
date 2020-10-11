/*
* Author: Biren Patel
* Description: PRNG library API. Depends on GCC builtins and Intel x86 inline
* assembly. Since the functions are performance critical, little to no error
* handling is performed. Assertion density is high, but in optimized mode the
* user should check input parameters where necessary.
*/

#ifndef RANDOM_H
#define RANDOM_H

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
* NAME: rng_verify_hardware
* DESC: check that the hardware meets the API requirements
* OUTP: YES_RDRAND_RDSEED on successful verification.
*******************************************************************************/
#define RNG_YES_RDRAND_RDSEED               0
#define RNG_NO_RDRAND                       1
#define RNG_NO_RDSEED                       2
#define RNG_NO_INTEL                        3
#define RNG_MAX_EAX_PARAMETER_TOO_LOW       4

int rng_verify_hardware(void);

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
*******************************************************************************/
typedef struct
{
    uint64_t state;
    uint64_t (*next) (uint64_t *state);
    uint64_t (*rand) (uint64_t *state, const uint64_t min, const uint64_t max);
    uint64_t (*bias) (uint64_t *state, const uint64_t n, const int m);
    uint64_t (*bino) (uint64_t *state, uint64_t k, const uint64_t n, const int m);
    stream_t (*vndb) (const void *src, void *dest, const uint64_t n, const uint64_t m);
} random_t;

/*******************************************************************************
* NAME: rng_init
* DESC: initialize a variable of type random_t
* OUTP: type random_t where zero state indicates rdseed failure.
* @ seed : set seed=0 for rng_rdseed64. Mixing function used on nonzero values.
* @ retry : rng_rdseed64 parameter, unused if seed != 0
*******************************************************************************/
random_t rng_init(const uint64_t seed, const uint8_t retry);

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
* NAME: rng_binomial
* DESC: generate random numbers from a binomial distribution
* OUTP: number of successful trials
* @ k : total trials
* @ n : nonzero numberator probability, p = n/2^m, not exceeding 2^m
* @ m : nonzero denominator indicating exponent of base 2 not exceeding 64.
*******************************************************************************/
uint64_t rng_binomial(uint64_t *state, uint64_t k, const uint64_t n, const int m);

#endif