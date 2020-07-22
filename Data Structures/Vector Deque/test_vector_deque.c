/*
* Author: Biren Patel
* Description: Unit tests for vector deque via the Unity framework.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vector_deque.h"
#include "src/unity.h"

/******************************************************************************/

void setUp(void) {}
void tearDown(void) {}

/******************************************************************************/
//custom integration test headers to stdout

#define INTEGRATION_BEGIN()                                                    \
        printf("\n\nIntegration Test: RUNNING...")

#define INTEGRATION_END()                                                      \
        printf("\b\b\b\b\b\b\b\b\b\b%s\n",                                     \
        status == 1 ? "OK        " : "FAIL      ");

/******************************************************************************/

make_vector(SUT, int, static inline);

void test_push_back_on_empty_vector(void)
{
    //given an empty vector
    SUT x = SUT_create(100, 0);
    
    //when we push an element to the back
    bool status = SUT_push_back(x, 42);
    
    //then it is successful
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT(42, x->vector[0]);
    TEST_ASSERT_EQUAL_INT(1, x->count);
    
    //afterwards clean up memory
    SUT_destroy(x);
}

/******************************************************************************/

void test_push_back_on_empty_vector_with_offset(void)
{
    //given an empty vector with an offset from the start of its array member
    SUT x = SUT_create(100, 50);
    
    //when we push an element to the back
    bool status = SUT_push_back(x, 42);
    
    //then it is successful
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT(42, x->vector[x->L_idx]);
    TEST_ASSERT_EQUAL_INT(42, x->vector[50]);
    TEST_ASSERT_EQUAL_INT(1, x->count);
    
    //afterwards clean up memory
    SUT_destroy(x);
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_push_back_on_empty_vector);
        RUN_TEST(test_push_back_on_empty_vector_with_offset);
    UNITY_END();
    
    return EXIT_SUCCESS;
}