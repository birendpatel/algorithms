/*
* Author: Biren Patel
* Description: Unit and integration tests for random number generator library.
*/

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "timeit.h"
#include "random.h"

bool test_monte_carlo_of_all_rng_bias_resolutions_is_approximately_correct(void)
{
    //arrange: test 8-bit resolutions with 1 million simulations each
    enum {SAMPLES = 1000000};
    enum {RESOLUTION_MAX = 255};
    double result[RESOLUTION_MAX];
    
    random_t rng = rng_init(0, 100);
    if (rng.state == 0)
    {
        fprintf(stderr, "rdseed instruction failed\n");
        return false;
    }
    
    //act: at each resolution, generate a biased number and check any bit.
    //since the operation is "vectorized" and the PRNG is assumed "unbiased",
    //it doesn't matter (in theory) which bit is checked, so I use bit 0.
    
    for (size_t i = 1; i < RESOLUTION_MAX; ++i)
    {
        int success = 0;
        
        for (size_t j = 0; j < SAMPLES; ++j)
        {
            if (rng.bias(&rng.state, i, 8) & 0x1) ++success;
        }
        
        result[i] = ((double) success) / ((double) SAMPLES);
    }
    
    //assert (just interested in seeing the visuals for now)
    double base = 0.0;
    
    for (size_t i = 1; i < RESOLUTION_MAX; ++i)
    {
        printf("%llu: (actual) %-10g \t (true) %10g\n", i, result[i], base);
        base += 0.00390625; //a little floating imprecision but good enough
    }
    
    return true;
}

/******************************************************************************/

void test_compare_execution_time_of_rng_generator_to_rng_bias(void)
{
    random_t rng = rng_init(0, 100);
    if (rng.state == 0)
    {
        fprintf(stderr, "rdseed instruction failed\n");
        abort();
    }
    
    init_timeit();
    
    start_timeit();
    
    for (size_t i = 0; i < 1000000; ++i)
    {
        rng.next(&rng.state);
    }
    
    end_timeit();
    printf("xorshift: %lld microseconds\n", TIMEIT_RESULT_MICROSECONDS);
    
    start_timeit();
    
    for (size_t i = 0; i < 1000000; ++i)
    {
        rng.bias(&rng.state, 1ULL, 64);
    }
    
    end_timeit();
    printf("biased: %lld microseconds\n", TIMEIT_RESULT_MICROSECONDS);
    
    start_timeit();
    
    for (size_t i = 0; i < 1000000; ++i)
    {
        rng.binom(&rng.state, 640, 1ULL, 64);
    }
    
    end_timeit();
    printf("biased: %lld microseconds\n", TIMEIT_RESULT_MICROSECONDS);
    
}

/******************************************************************************/

bool test_monte_carlo_of_64_bit_bias_is_approximately_correct(void)
{
    //arrange: test all 256 resolutions with 1 million simulations each
    enum {SAMPLES = 1000000};
    
    random_t rng = rng_init(0, 100);
    if (rng.state == 0)
    {
        fprintf(stderr, "rdseed instruction failed\n");
        return false;
    }
    
    //act: at each resolution, generate a biased number and check any bit.
    //since the operation is "vectorized" and the PRNG is assumed "unbiased",
    //it doesn't matter (in theory) which bit is checked, so I use bit 0.
    
    int success = 0;
    
    for (size_t j = 0; j < SAMPLES; ++j)
    {
        if(rng.bias(&rng.state, 1908374987592347239ULL, 64) & 0x1ULL)
        {
            ++success;
        }
    }
    
    printf("64 bit test: %f\n", ((double) success) / ((double) SAMPLES));
    
    return true;
}

/******************************************************************************/

void test_binomial(void)
{
    random_t rng = rng_init(0, 100);
    
    for (size_t i = 0; i < 100; ++i)
    {
        printf("%llu\n", rng.binom(&rng.state, 250, 64, 8));
    }
}

/******************************************************************************/

int main(void)
{
    
    //test_monte_carlo_of_all_rng_bias_resolutions_is_approximately_correct();
    
    test_compare_execution_time_of_rng_generator_to_rng_bias();
    
    //test_monte_carlo_of_64_bit_bias_is_approximately_correct();
    
    //test_binomial();
    
    return EXIT_SUCCESS;
}