/*
* Author: Biren Patel
* Description: Unit and integration tests for random number generator library.
* Most of these tests are slow as monte carlo simulations are required in many
* cases to approximately verify function behavior.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "timeit.h"
#include "random.h"
#include "src\unity.h"

/******************************************************************************/

void setUp(void) {}
void tearDown(void) {}
#define self &rng.state

/*******************************************************************************
Check that rng_bias is correct by monte carlo simulation on probabilites of
1/256 through 255/256. At 1,000,000 simulations with floating precision, the
tolerance is set as +/- 0.0015.
*/

void test_monte_carlo_of_rng_bias_at_256_bits_of_resolution(void)
{
    //arrange
    struct tuple {float actual; float expected;} result[255];
    random_t rng = rng_init(0, 255);
    assert(rng.state != 0 && "rdseed failure");
    float expected_counter = 0.0;
    
    //act
    for (size_t i = 0; i < 255; i++)
    {
        int success = 0;
        int numerator = i + 1;
        expected_counter += 0.00390625;
        
        for (size_t j = 0; j < 1000000; j++)
        {
            if (rng.bias(self, numerator, 8) & 1) success++;
        }
        
        result[i].actual = ((float) success) / 1000000;
        result[i].expected = expected_counter;
    }
    
    //assert
    for (size_t i = 0; i < 255; ++i)
    {        
        TEST_ASSERT_FLOAT_WITHIN(.0015f, result[i].expected, result[i].actual);
    }        
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_monte_carlo_of_rng_bias_at_256_bits_of_resolution);
    UNITY_END();
    
    return EXIT_SUCCESS;
}