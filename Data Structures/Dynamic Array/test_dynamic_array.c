/*
* Author: Biren Patel
* Description: unit tests for generic dynamic array ADT
* Note: uses the Unity framework from ThrowTheSwitch
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "dynamic_array.h"
#include "src\unity.h"


/******************************************************************************/

void setUp(void) {}
void tearDown(void) {}


/******************************************************************************/
//malloc check

#define verify_malloc(ptr)                                                     \
        if (ptr == NULL)                                                       \
        {                                                                      \
            printf("%s: malloc failed during unit test", __func__);            \
            exit(EXIT_FAILURE);                                                \
        }

/******************************************************************************/
//custom free function for generic dynamic array

void custom_free(void *header)
{
    struct darray_header *dh = (struct darray_header *) header;
    
    for (size_t i = 0; i < dh->count; ++i)
    {
        free(dh->data[i]);
    }
    
    free(dh);
}

/******************************************************************************/
//test 1

void appending_an_element_is_successful (void)
{
    //given a void pointer array and a dynamically allocated integer
    darray SUT = darray_create(1, custom_free);
    
    int *item = malloc(sizeof(int));
    verify_malloc(item);
    *item = 42;
    
    //when we push a pointer to the integer 42
    darray_append(&SUT, item);
    
    //then we expect the pointer was pushed successfully
    TEST_ASSERT_EQUAL_INT(42, * (int*) SUT[0]);
    TEST_ASSERT_EQUAL_INT(1, darray_count(SUT));
    
    //afterwards clean up memory
    darray_destroy(SUT);
}


/******************************************************************************/
//test 2

void pop_several_elements_off_array_is_successful (void)
{
    //given an array of five void pointers (but really int pointers)
    int ground_truth[] = {4,3,2};
    int actual[3];
    
    darray SUT = darray_create(1, custom_free);
    
    int *item;
    
    for (size_t i = 0; i < 5; ++i)
    {
        item = malloc(sizeof(int));
        verify_malloc(item);
        *item = i;
        darray_append(&SUT, item);
    }
    
    //when we pop several of those pointers off the array
    for (size_t i = 0; i < 3; ++i)
    {
        darray_pop(SUT, (void**) &item);
        actual[i] = *item;
        free(item);
    }
    
    //then we expect them to exit the array in the correct order
    TEST_ASSERT_EQUAL_INT_ARRAY(ground_truth, actual, 3);
    TEST_ASSERT_EQUAL_INT(2, darray_count(SUT));
    
    //afterwards clean up memory
    darray_destroy(SUT);    
}

/******************************************************************************/

void popleft_several_elements_off_array_is_successful (void)
{
    //given an array of five void pointers (but really int pointers)
    int ground_truth[] = {0,1,2};
    int actual[3];
    
    darray SUT = darray_create(1, custom_free);
    
    int *item;
    
    for (size_t i = 0; i < 5; ++i)
    {
        item = malloc(sizeof(int));
        verify_malloc(item);
        *item = i;
        darray_append(&SUT, item);
    }
    
    //when we pop several of those pointers off the array
    for (size_t i = 0; i < 3; ++i)
    {
        darray_popleft(SUT, (void**) &item);
        actual[i] = *item;
        free(item);
    }
    
    //then we expect them to exit the array in the correct order
    TEST_ASSERT_EQUAL_INT_ARRAY(ground_truth, actual, 3);
    TEST_ASSERT_EQUAL_INT(2, darray_count(SUT));
    
    //afterwards clean up memory
    darray_destroy(SUT);    
}

/******************************************************************************/

void calling_peek_does_not_accidentally_pop_element_off_array (void)
{
    //given an array with one void pointer (but really a string literal)
    char *pushed_item = "element 1";
    char *peeked_item;
    
    darray SUT = darray_create(1, free);
    darray_append(&SUT, pushed_item);
    
    //when we take a peek at the top pointer
    darray_peek(SUT, (void**) &peeked_item);
    
    //then the peek will not accidentally pop the pointer off of the array
    TEST_ASSERT_EQUAL_STRING(SUT[0], pushed_item);
    TEST_ASSERT_EQUAL_STRING(pushed_item, peeked_item);
    
    //afterwards clean up memory
    darray_destroy(SUT); 
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
    
    RUN_TEST(appending_an_element_is_successful);
    RUN_TEST(pop_several_elements_off_array_is_successful);
    RUN_TEST(popleft_several_elements_off_array_is_successful);
    RUN_TEST(calling_peek_does_not_accidentally_pop_element_off_array);
    
    return UNITY_END();
}