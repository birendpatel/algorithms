/*
* Author: Biren Patel
* Description: unit tests for generic dynamic array ADT
* Note: uses the Unity framework from ThrowTheSwitch
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
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
//custom integration test headers to stdout

#define INTEGRATION_BEGIN()                                                    \
        printf("\n\nIntegration Test: RUNNING...")

#define INTEGRATION_END()                                                      \
        printf("\b\b\b\b\b\b\b\b\b\b%s\n",                                     \
        status == 1 ? "OK        " : "FAIL      ");

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
    //given a void pointer array
    darray SUT = darray_create(1, custom_free);
        
    //when we push a pointer to the integer 42
    int *item = malloc(sizeof(int));
    verify_malloc(item);
    *item = 42;
    
    int error_code = darray_append(&SUT, item);
    
    //then we expect the pointer was pushed successfully
    TEST_ASSERT_EQUAL_INT(42, * (int*) SUT[0]);
    TEST_ASSERT_EQUAL_INT(1, darray_count(SUT));
    TEST_ASSERT_EQUAL_INT(0, error_code);
    
    //afterwards clean up memory
    darray_destroy(SUT);
}


/******************************************************************************/
//test 2

void pop_several_elements_off_array_is_successful (void)
{
    //given an array of five void pointers (but really int pointers)   
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
    int actual[3];
    
    for (size_t i = 0; i < 3; ++i)
    {
        darray_pop(SUT, (void**) &item);
        actual[i] = *item;
        free(item);
    }
    
    //then we expect them to exit the array in the correct order
    int ground_truth[] = {4,3,2};
    TEST_ASSERT_EQUAL_INT_ARRAY(ground_truth, actual, 3);
    TEST_ASSERT_EQUAL_INT(2, darray_count(SUT));
    
    //afterwards clean up memory
    darray_destroy(SUT);    
}

/******************************************************************************/

void popleft_several_elements_off_array_is_successful (void)
{
    //given an array of five void pointers (but really int pointers) 
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
    int actual[3];
    
    for (size_t i = 0; i < 3; ++i)
    {
        darray_popleft(SUT, (void**) &item);
        actual[i] = *item;
        free(item);
    }
    
    //then we expect them to exit the array in the correct order
    int ground_truth[] = {0,1,2};
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
    darray SUT = darray_create(1, free);
    darray_append(&SUT, pushed_item);
    
    //when we take a peek at the top pointer
    char *peeked_item;
    bool status = darray_peek(SUT, (void**) &peeked_item);
    
    //then the peek will not accidentally pop the pointer off of the array
    TEST_ASSERT_EQUAL_STRING((char*) SUT[0], pushed_item);
    TEST_ASSERT_EQUAL_STRING(pushed_item, peeked_item);
    TEST_ASSERT_TRUE(status);
    
    //afterwards clean up memory
    darray_destroy(SUT); 
}

/******************************************************************************/

void alternating_series_of_pop_and_popleft_is_possible (void)
{
    //given an array with several void pointers (but really string literals)
    darray SUT = darray_create(1, free);
    
    darray_append(&SUT, "this");
    darray_append(&SUT, "is");
    darray_append(&SUT, "just");
    darray_append(&SUT, "a");
    darray_append(&SUT, "test");
    
    //when an alternating series of pop and popleft operations occur
    darray_pop(SUT, NULL);
    darray_popleft(SUT, NULL);
    darray_pop(SUT, NULL);
    darray_popleft(SUT, NULL);
    
    //then we expect the array to result in an appropriate state
    TEST_ASSERT_EQUAL_STRING("just", (char*) SUT[0]);
    TEST_ASSERT_EQUAL_INT(1, darray_count(SUT));
    
    //afterwards clean up memory
    darray_destroy(SUT);
}

/******************************************************************************/

void attempt_to_pop_from_empty_array_does_nothing (void)
{
    //given an array with no data
    darray SUT = darray_create(1, free);
    
    //when a pop operation is requested
    void *result = NULL;
    bool status = darray_pop(SUT, &result);
    
    //then nothing happens
    TEST_ASSERT_FALSE(status);
    TEST_ASSERT_NULL(result);
    
    //afterwards clean up memory
    darray_destroy(SUT);
}


/******************************************************************************/

void attempt_to_popleft_from_empty_array_does_nothing (void)
{
    //given an array with no data
    darray SUT = darray_create(1, free);
    
    //when a pop operation is requested
    void *result = NULL;
    bool status = darray_popleft(SUT, &result);
    
    //then nothing happens
    TEST_ASSERT_FALSE(status);
    TEST_ASSERT_NULL(result);
    
    //afterwards clean up memory
    darray_destroy(SUT);
}

/******************************************************************************/

void attempt_to_peek_from_empty_array_does_nothing (void)
{
    //given an array with no data
    darray SUT = darray_create(1, free);
    
    //when a pop operation is requested
    void *result = NULL;
    bool status = darray_peek(SUT, &result);
    
    //then nothing happens
    TEST_ASSERT_FALSE(status);
    TEST_ASSERT_NULL(result);
    
    //afterwards clean up memory
    darray_destroy(SUT);
}


/******************************************************************************/
//this section is tested with DynamoRio, not Unity

struct object
{
    int i;
    double x;
    char c;    
};

int integration_test_does_not_result_in_a_memory_leak (void)
{
    //given an array of void pointers (but really pointers to struct object)
    darray SUT = darray_create(512, custom_free);
    
    struct object *obj;
    int status;
    
    //perform a large amount of appends
    for (size_t i = 0; i < 10000; ++i)
    {
        obj = malloc(sizeof(struct object));
        verify_malloc(obj);
        
        obj->i = 42;
        obj->x = 3.14;
        obj->c = 'z';
        
        status = darray_append(&SUT, obj);
        assert(status == 0);
    }
    
    //perform a large mixture of pop, popleft, and peek
    for (size_t i = 0; i < 2500; ++i)
    {
        status = darray_pop(SUT, (void**) &obj);
        assert(status == 1);
        free(obj);
        
        status = darray_popleft(SUT, (void**) &obj);
        assert(status == 1);
        free(obj);
        
        status = darray_peek(SUT, (void**) &obj);
        assert(status == 1);
    }
    
    //afterwards clean up memory
    darray_destroy(SUT);
    
    return 1;
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(appending_an_element_is_successful);
        RUN_TEST(pop_several_elements_off_array_is_successful);
        RUN_TEST(popleft_several_elements_off_array_is_successful);
        RUN_TEST(calling_peek_does_not_accidentally_pop_element_off_array);
        RUN_TEST(alternating_series_of_pop_and_popleft_is_possible);
        RUN_TEST(attempt_to_pop_from_empty_array_does_nothing);
        RUN_TEST(attempt_to_popleft_from_empty_array_does_nothing);
        RUN_TEST(attempt_to_peek_from_empty_array_does_nothing);
    UNITY_END();
    
    
    INTEGRATION_BEGIN();
        int status;
        status = integration_test_does_not_result_in_a_memory_leak();
    INTEGRATION_END();
    
    
    return EXIT_SUCCESS;
}