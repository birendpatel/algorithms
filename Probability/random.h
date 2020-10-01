/*
* Author: Biren Patel
* Description: Pseudo random number generator. This API is dependent on the GCC
* builtin functions and Intel x86 inline assembly. Type random_t manages PRNG
* state and provides convenient API access through its methods. Optionally, the
* user may define and seed a 64-bit state variable.
*/

#ifndef RANDOM_H
#define RANDOM_H

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
* rng_verify_hardware
* purpose: check that the hardware meets the API requirements.
* returns: YES_RDRAND_RDSEED on successful verification.
*******************************************************************************/

#define RNG_YES_RDRAND_RDSEED               0
#define RNG_NO_RDRAND                       1
#define RNG_NO_RDSEED                       2
#define RNG_NO_INTEL                        3
#define RNG_MAX_EAX_PARAMETER_TOO_LOW       4

int rng_verify_hardware(void);

/*******************************************************************************
* struct: random_t
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
    uint64_t (*bias) (uint64_t *state, const uint8_t resolution);
} random_t;

/*******************************************************************************
* rng_init
* purpose: initialize a variable of type random_t
* @ seed : set seed=0 for rng_rdseed64. Mixing function used on nonzero values.
* @ retry : rng_rdseed64 parameter, unused if seed != 0
* returns: random_t type with nonzero state on success. zero state indicates
* rng_rdseed64 failure. 
*******************************************************************************/

random_t rng_init(const uint64_t seed, const uint8_t retry);

/*******************************************************************************
* rng_rdseed64
* purpose: generate 64-bit seed using the x86 RDSEED instruction
* @ seed : contains a valid seed only if the function returns true
* @ retry : maximum attempts to retry instruction if the first attempt failed
* returns: false on failure
*******************************************************************************/

bool rng_rdseed64(uint64_t *seed, const uint8_t retry);

/*******************************************************************************
* rng_generator
* purpose: generate 1 unsigned 64-bit psuedo random number via the default PRNG.
* returns: random number not guaranteed equal to the updated state parameter.
*******************************************************************************/

uint64_t rng_generator(uint64_t *state);

/*******************************************************************************
* rng_rand
* purpose: generate an unsigned unbiased 64-bit psuedo random number in a range
* @ min : inclusive lower bound
* @ max : exclusive upper bound
* returns : 0 if null state. non-null state will be updated.
*******************************************************************************/

uint64_t rng_rand(uint64_t *state, const uint64_t min, const uint64_t max);


/*******************************************************************************
* rng_bias
* purpose: generate an unsigned biased 64-bit number. In other words, perform 
* a total of 64 i.i.d. bernoulli trials where each trial maps to a single bit.
* @ resolution : probability of success calculated as resolution divided by
* 256. e.g., resolution = 1 -> 1/256 = 0.00390625 probability.
*******************************************************************************/

uint64_t rng_bias (uint64_t *state, const uint8_t resolution);

#endif