/*
* NAME: Copyright (c) 2020, Biren Patel
* DESC: subroutines for psuedo random number generation
* LISC: MIT License
*/

#ifndef SCIPACK_RANDOM_H
#define SCIPACK_RANDOM_H

#include <stdint.h>

/*******************************************************************************
* Compiler Checks
*******************************************************************************/
#ifndef __GNUC__
    #error "sprng.h: requires GNU C Compiler"
#endif

#ifndef __RDRND__
    #error "scipack random module requires x86 RDRAND instruction"
#endif

/*******************************************************************************
* Error Codes
*******************************************************************************/
enum RANDOM_ERROR_CODES
{
    RANDOM_SUCCESS              = 0,
    RANDOM_RDRAND_FAIL          = 1,
    RANDOM_MALLOC_FAIL          = 2,
};

/*******************************************************************************
* NAME: generator_(128/64/32)bit
* DESC: abstract interface for psuedo random number generator
* OUTP: this object must be returned by some random_init_* function

* @ state : internal state of the generator initialized by random_init_*

* @ next  : generate raw random integers directly from the underlying PRNG
    * @ state : self internal state
    
* @ rint  : unbiased integers between min and max inclusive
    * @ min : lower bound inclusive
    * @ max : upper bound inclusive
    
* @ bern  : generate 64 iid bernoulli trials with probability p = n/(2^m)
    * @ n : numerator of p = n/(2^m) where 0 < n < 2^m
    * @ m : denominator of p = n/(2^m) where 0 < m <= 64
    
* @ bino  : sample from a binomial distribution X~(k, p) where p = n/(2^m)
    * @ k : total trials
    * @ n : numerator of p = n/(2^m) where 0 < n < 2^m
    * @ m : denominator of p = n/(2^m) where 0 < m <= 64
*******************************************************************************/
typedef struct generator_64bit * generator_64bit;

struct generator_64bit
{
    void *state;
    uint64_t (*next) (void *state);
    uint64_t (*rint) (generator_64bit self, const uint64_t min, const uint64_t max);
    uint64_t (*bern) (generator_64bit self, const uint64_t n, const int m);
    uint64_t (*bino) (generator_64bit self, uint64_t k, const uint64_t n, const int m);    
};

/*******************************************************************************
* NAME: random_init_*
* DESC: initialize a statistical psuedo random number generator
* OUTP: null on error, check error argument for details
* NOTE: nonzero seeds are deterministically hashed for increased entropy
* @ seed : zero for non-deterministic seeding
* @ error : can be passed as null, else one of enum RANDOM_ERROR_CODES
*******************************************************************************/
generator_64bit random_init_pcg64_insecure(uint64_t seed, int *error);


#endif
