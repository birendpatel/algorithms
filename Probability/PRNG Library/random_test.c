/*
* Author: Biren Patel
* Description: Unit and integration tests for random number generator library.
* Most of these tests are slow as monte carlo simulations are required in many
* cases to approximately verify function behavior.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <immintrin.h>

#include "timeit.h"
#include "random.h"
#include "src\unity.h"

/******************************************************************************/

void setUp(void) {}
void tearDown(void) {}

#define self &rng.state
#define BIG_SIMULATION 2500000
#define MID_SIMULATION 500000
#define SMALL_SIMULATION 50000

/*******************************************************************************
Given a seed (the answer to life, the universe, everything), does PCG 64i output
the same stream on two different random_t variables?
*/

void test_deterministic_seed_pcg_output(void)
{
    //arrange
    random_t rng_1 = rng_init(42);
    assert(rng_1.state.current != 0 && "rdrand failure");
    
    random_t rng_2 = rng_init(42);
    assert(rng_2.state.current != 0 && "rdrand failure");
    
    //act-assert
    for (size_t i = 0; i < BIG_SIMULATION; i++)
    {
        uint64_t result_1 = rng_1.next(&rng_1.state);
        uint64_t result_2 = rng_2.next(&rng_2.state);
        
        TEST_ASSERT_EQUAL_UINT64(result_1, result_2);
    }
}

/*******************************************************************************
Check that rng_bias is correct by monte carlo simulation on probabilites of
1/256 through 255/256. At 1,000,000 simulations with floating precision, the
tolerance is set as +/- 0.0015.
*/

void test_monte_carlo_of_rng_bias_at_256_bits_of_resolution(void)
{
    //arrange
    struct tuple {float actual; float expected;} result[255];
    random_t rng = rng_init(0);
    assert(rng.state.current != 0 && "rdrand failure");
    float expected_counter = 0.0;
    
    //act
    for (size_t i = 0; i < 255; i++)
    {
        int success = 0;
        uint64_t numerator = i + 1;
        expected_counter += 0.00390625f;
        
        for (size_t j = 0; j < BIG_SIMULATION; j++)
        {
            if (rng.bias(self, numerator, 8) & 1) success++;
        }
        
        result[i].actual = ((float) success) / BIG_SIMULATION;
        result[i].expected = expected_counter;
    }
    
    //assert
    for (size_t i = 0; i < 255; ++i)
    {        
        TEST_ASSERT_FLOAT_WITHIN(.001f, result[i].expected, result[i].actual);
    }        
}

/*******************************************************************************
Given an input stream with bits biased to .125 probability of success, output
a stream of 135 bits with unbiased bits. The input stream has no autocorrelation
and 135 was selected as a non-multiple of 64.
*/

void test_von_neumann_debiaser_outputs_all_unbiased_bits(void)
{
    //arrange
    random_t rng = rng_init(0);
    assert(rng.state.current != 0 && "rdrand failure");
    
    uint64_t input_stream[35]; //2240 input bits should be enough
    uint64_t output_stream[3]; //space for 192 but only care about first 135
    float results[135] = {0}; //holds monte carlo result for each output bit
    
    stream_t info;
    double avg_use = 0;
    uint64_t min_use = UINT64_MAX;
    uint64_t max_use = 0;
    
    //act-assert
    for (size_t i = 0; i < MID_SIMULATION; i++)
    {
        //populate the input stream with biased bits
        for (size_t j = 0; j < 35; ++j)
        {
            input_stream[j] = rng.bias(self, 32, 8);
        }
        
        //populate the output stream with unbiased bits
        info = rng.vndb(input_stream, output_stream, 2240, 135);
        TEST_ASSERT_EQUAL_INT(135, info.filled);
        
        //ancillary points of interest
        avg_use += (double) info.used;
        if (info.used < min_use) min_use = info.used;
        if (info.used > max_use) max_use = info.used;
        
        //walk through the output stream and assess results of this round
        for (size_t k = 0; k < 135; k++)
        {
            if ((output_stream[k/64] >> (k % 64)) & 1) results[k]++;
        }
    }
    
    //assert
    for (size_t i = 0; i < 135; ++i)
    {
        results[i] /= MID_SIMULATION;
        TEST_ASSERT_FLOAT_WITHIN(.01f, 0.5f, results[i]);
    }
    
    //points of interest
    puts("POI: test_von_neumann_debiaser_outputs_all_unbiased_bits");
    printf("average input bits used: %-10g\n", avg_use/MID_SIMULATION);
    printf("maximum input bits used: %llu\n", max_use);
    printf("minimum input bits used: %llu\n", min_use);
}

/*******************************************************************************
Given a time series of 1010101010...10 the autocorrelation at any lag k should
alternate between 1 and -1.
*/

void test_cyclic_autocorrelation_of_alternating_bitstream(void)
{
    //arrange
    double results[64] = {0};
    random_t rng = rng_init(0);
    assert(rng.state.current != 0 && "rdrand failure");
    
    uint64_t input_stream[100000] = {0};
    uint64_t word = 0xAAAAAAAAAAAAAAAA;
    
    for (size_t i = 0; i < 100000; i++)
    {
        input_stream[i] = word;
    }
    
    //act
    for (size_t i = 0; i < 64; i++)
    {
        results[i] = rng.cycc(input_stream, 6400000, i);
    }
    
    //assert
    for (size_t i = 0; i < 64; i++)
    {
        if (i & 1) TEST_ASSERT_EQUAL_FLOAT(-1, (float) results[i]);
        else TEST_ASSERT_EQUAL_FLOAT(1, (float) results[i]);
    }
}

/*******************************************************************************
Benchmarks on 1 million draws.
*/

#define loop for (size_t i = 0; i < 1000000; i++)

void speed_test(void)
{
    random_t rng = rng_init(0);
    assert(rng.state.current != 0 && "rdrand failure");
    init_timeit();       
    puts("\n~~~~~ Speed Tests ~~~~~");
    
    //base PCG 64i generator
    start_timeit();
    loop { rng.next(self); }
    end_timeit();
    printf("PCG Generator: %llu us\n", result_timeit(MICROSECONDS));
    
    //rng bias at 8 generator calls
    start_timeit();
    loop { rng.bias(self, 1, 8); }
    end_timeit();
    printf("RNG Bias: %llu us\n", result_timeit(MICROSECONDS));
    
    //rng binomial at no additional generator calls (overhead only)
    start_timeit();
    loop { rng.bino(self, 64, 1, 8); }
    end_timeit();
    printf("RNG Binomial: %llu us\n", result_timeit(MICROSECONDS));
}

/******************************************************************************/

void test_simd_pcg_32_bit_insecure_generator(void)
{
    random_simd_t rng_simd = rng_simd_init(1,2,3,4);
    
    for (size_t j = 0; j < 10; j++)
    {     
        __m256i x = rng_simd.next(&rng_simd.state);
        
        uint64_t *fx = (uint64_t *) &x;
        
        printf("%llu\t%llu\t%llu\t%llu\n", fx[0], fx[1], fx[2], fx[3]);
    }
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_deterministic_seed_pcg_output);
        RUN_TEST(test_monte_carlo_of_rng_bias_at_256_bits_of_resolution);
        RUN_TEST(test_von_neumann_debiaser_outputs_all_unbiased_bits);
        RUN_TEST(test_cyclic_autocorrelation_of_alternating_bitstream);
    UNITY_END();
    
    speed_test();
    
    test_simd_pcg_32_bit_insecure_generator();
    
    return EXIT_SUCCESS;
}