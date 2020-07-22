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
//systems under test with various data types

make_vector(int_sut, int, static inline);

make_vector(str_sut, char*, static inline);

/******************************************************************************/

void test_push_back_on_empty_vector(void)
{
    //given an empty vector
    int_sut x = int_sut_create(100, 0);
    
    //when we push an element to the back
    bool status = int_sut_push_back(x, 42);
    
    //then it is successful
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT(42, x->vector[0]);
    TEST_ASSERT_EQUAL_INT(1, x->count);
    
    //afterwards clean up memory
    int_sut_destroy(x);
}

/******************************************************************************/

void test_push_back_on_empty_vector_with_offset(void)
{
    //given an empty vector with an offset from the start of its array member
    int_sut x = int_sut_create(100, 50);
    
    //when we push an element to the back
    bool status = int_sut_push_back(x, 42);
    
    //then it is successful
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT(42, x->vector[x->L_idx]);
    TEST_ASSERT_EQUAL_INT(42, x->vector[50]);
    TEST_ASSERT_EQUAL_INT(1, x->count);
    
    //afterwards clean up memory
    int_sut_destroy(x);
}

/******************************************************************************/

void test_push_front_on_empty_vector_with_offset(void)
{
    //given an empty vector with an offset from the start of its array member
    int_sut x = int_sut_create(100, 50);
    
    //when we push an element to the front
    bool status = int_sut_push_front(x, 42);
    
    //then it is successful
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT(42, x->vector[x->L_idx]);
    TEST_ASSERT_EQUAL_INT(42, x->vector[49]);
    TEST_ASSERT_EQUAL_INT(1, x->count);
    
    //afterwards clean up memory
    int_sut_destroy(x);
}

/******************************************************************************/

void test_push_back_and_front_on_empty_vector_with_offset(void)
{
    //given an empty vector with an offset from the start of its array member
    str_sut x = str_sut_create(100, 1);
    
    //when we push an element to the back and then to the front
    bool status_push_back = str_sut_push_back(x, "B");
    bool status_push_front = str_sut_push_front(x, "A");
    
    //then it is successful
    TEST_ASSERT_TRUE(status_push_back);
    TEST_ASSERT_TRUE(status_push_front);
    TEST_ASSERT_EQUAL_STRING("A", x->vector[0]);
    TEST_ASSERT_EQUAL_STRING("B", x->vector[1]);
    TEST_ASSERT_EQUAL_INT(2, x->count);

    //afterwards clean up memory
    str_sut_destroy(x);
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_push_back_on_empty_vector);
        RUN_TEST(test_push_back_on_empty_vector_with_offset);
        RUN_TEST(test_push_front_on_empty_vector_with_offset);
        RUN_TEST(test_push_back_and_front_on_empty_vector_with_offset);
    UNITY_END();
    
    return EXIT_SUCCESS;
}