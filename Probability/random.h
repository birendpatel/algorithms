/*
* Author: Biren Patel
* Description: Pseudo random number generator. This API is dependent on the GCC
* builtin functions and Intel x86 inline assembly. The user can use the API in
* one of two ways. Option one is to define a 64-bit state variable and pass it 
* by reference to all API calls. The variable must be seeded prior to generator
* calls. Option 2 is to define a variable of type random_t, initialize it with 
* rng_init(), and access the API through method calls on the struct members.
*/

#ifndef RANDOM_H
#define RANDOM_H

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
* struct: random_t
* @ state : must be seeded prior to any method calls.
* @ next : generate a 64-bit psuedo random number using the underlying PRNG. The
*          state is updated and the number is returned.
* @ rand : generate an unsigned unbiased integer in [min, max) using a rejection
*          sampling bitmask.
*******************************************************************************/

typedef struct
{
    uint64_t state;
    uint64_t (*next) (uint64_t *state);
    uint64_t (*rand) (uint64_t *state, uint64_t min, uint64_t max);
} random_t;

/*******************************************************************************
* rng_init
* purpose: initialize a variable of type random_t
* @ seed : set seed=0 for rng_rdseed64. Mixing function used on nonzero values.
* @ retry : rng_rdseed64 parameter, unused if seed != 0
* returns: random_t type with nonzero state on success. zero state indicates
* rng_rdseed64 failure. 
*******************************************************************************/

random_t rng_init(uint64_t seed, uint8_t retry);

/*******************************************************************************
* rng_verify_rdseed
* purpose: check that the hardware meets the API requirements.
* returns: YES_RDRAND_RDSEED on successful verification.
*******************************************************************************/

#define RNG_YES_RDRAND_RDSEED               0
#define RNG_NO_RDRAND                       1
#define RNG_NO_RDSEED                       2
#define RNG_NO_INTEL                        3
#define RNG_MAX_EAX_PARAMETER_TOO_LOW       4

int rng_verify_rdseed(void);

/*******************************************************************************
* rng_rdseed64
* purpose: generate 64-bit value using the x86 RDSEED instruction
* @ seed : contains a valid seed only if the function returns true
* @ retry : maximum attempts to retry instruction if the first attempt failed
* returns: false on failure
*******************************************************************************/

bool rng_rdseed64(uint64_t *seed, uint8_t retry);

/*******************************************************************************
* rng_generator
* purpose: generate a 64-bit psuedo random number using the default PRNG.
* returns: random number not guaranteed equal to the updated state parameter.
*******************************************************************************/

uint64_t rng_generator(uint64_t *state);

#endif