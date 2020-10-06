/*
* Author: Biren Patel
* Description: Pseudo random number generator. This API is dependent on the GCC
* builtin functions and uses Intel x86 inline assembly. Since the functions are
* performance critical, little to no error handling is performed. Although there
* are assertions, the user should check input parameters prior to all calls. 
*/

#ifndef RANDOM_H
#define RANDOM_H

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
* rng_verify_hardware: check that the hardware meets the API requirements.
* returns: YES_RDRAND_RDSEED on successful verification.
*******************************************************************************/
#define RNG_YES_RDRAND_RDSEED               0
#define RNG_NO_RDRAND                       1
#define RNG_NO_RDSEED                       2
#define RNG_NO_INTEL                        3
#define RNG_MAX_EAX_PARAMETER_TOO_LOW       4

int rng_verify_hardware(void);

/*******************************************************************************
* random_t: manage PRNG state and provide methods for API access
* @ state : must be seeded prior to any method calls.
* @ next : call to rng_generator()
* @ rand : call to rng_rand()
* @ bias : call to rng_bias()
*******************************************************************************/
typedef struct
{
    uint64_t state;
    uint64_t (*next) (uint64_t *state);
    uint64_t (*rand) (uint64_t *state, const uint64_t min, const uint64_t max);
    uint64_t (*bias) (uint64_t *state, const uint64_t n, const int m);
    uint64_t (*binom) (uint64_t *state, uint64_t k, const uint64_t n, const int m);
} random_t;

/*******************************************************************************
* rng_init: initialize a variable of type random_t
* returns: random_t type, where zero state indicates rng_rdseed64 failure. 
* @ seed : set seed=0 for rng_rdseed64. Mixing function used on nonzero values.
* @ retry : rng_rdseed64 parameter, unused if seed != 0
*******************************************************************************/
random_t rng_init(const uint64_t seed, const uint8_t retry);

/*******************************************************************************
* rng_rdseed64: generate 64-bit seed using the x86 RDSEED instruction
* returns: false on failure
* @ seed : contains a valid seed only if the function returns true
* @ retry : maximum attempts to retry instruction if the first attempt failed
*******************************************************************************/
bool rng_rdseed64(uint64_t *seed, const uint8_t retry);

/*******************************************************************************
* rng_generator: generate a psuedo random number via the default PRNG.
* returns: random number not guaranteed equal to the updated state parameter.
*******************************************************************************/
uint64_t rng_generator(uint64_t *state);

/*******************************************************************************
* rng_rand: generate an unbiased psuedo random number in [min, max)
* returns : 0 if null state. non-null state will be updated
*******************************************************************************/
uint64_t rng_rand(uint64_t *state, const uint64_t min, const uint64_t max);

/*******************************************************************************
* rng_bias: simultaneous generation of 64 iid bernoulli trials 
* returns: 64-bit word where each bit has probability p = n/2^m of success
* @ n : nonzero numerator of probability, not exceeding 2^m
* @ m : nonzero denominator indicating exponent of base 2 not exceeding 64.
*******************************************************************************/
uint64_t rng_bias (uint64_t *state, const uint64_t n, const int m);


/******************************************************************************/

uint64_t rng_binomial(uint64_t *state, uint64_t k, const uint64_t n, const int m);

#endif