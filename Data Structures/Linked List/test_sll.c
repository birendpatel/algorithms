#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sll.h"
#include "src/unity.c"


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

void test_insert_at_head_of_empty_list_is_successful(void)
{
    //given an empty singly linked list
    struct sll *list = sll_create(free);
    
    //when an element is inserted at the head
    sll_insert_head(list, "butane");
    
    //then the insertion is successful
    TEST_ASSERT_EQUAL_STRING("butane", sll_access_head_data(list));
    TEST_ASSERT_EQUAL_INT(1, sll_size(list));
    
    //afterwards clean up memory
    sll_destroy(list);    
}


/******************************************************************************/

void test_insert_at_tail_of_non_empty_list_is_successful(void)
{
    //given a singly linked list with one node
    struct sll *list = sll_create(free);
    sll_insert_head(list, "propane");
    
    //when an element is inserted at the tail
    sll_insert_tail(list, "butane");
    
    //then the insertion is successful
    TEST_ASSERT_EQUAL_STRING("propane", sll_access_head_data(list));
    TEST_ASSERT_EQUAL_STRING("butane", sll_access_tail_data(list));
    TEST_ASSERT_EQUAL_INT(2, sll_size(list));
    
    //afterwards clean up memory
    sll_destroy(list);
}


/******************************************************************************/

void test_insert_in_middle_of_non_empty_list_is_successful(void)
{
    //given a singly linked list with two nodes
    struct sll *list = sll_create(free);
    sll_insert_head(list, "butane");
    sll_insert_head(list, "propane");
    
    //when an element is inserted in between
    sll_insert_idx(list, 1, "methane");
    
    //then the insertion is successful
    TEST_ASSERT_EQUAL_STRING("propane", sll_access_head_data(list));
    TEST_ASSERT_EQUAL_STRING("methane", sll_access_idx(list, 1)->datum);
    TEST_ASSERT_EQUAL_STRING("butane", sll_access_tail_data(list));
    TEST_ASSERT_EQUAL_INT(3, sll_size(list));
    
    //afterwards clean up 
    sll_destroy(list);
}

/******************************************************************************/
//integration test for memory leaks uses DynamoRio. unity framework not used.

void custom_free(void *list)
{
    struct sll *s = list;
    
    while (s->head != NULL) free(sll_remove_idx(s, 0));
    
    free(s);
}

struct plane
{
    double x;
    double y;
};


int integration_test_does_not_result_in_a_memory_leak(void)
{
    //given a singly linked list
    struct sll *list = sll_create(custom_free);
    
    //when a large amount of insertions and removals is performed
    for (size_t i = 0; i < 5000; ++i)
    {
        struct plane *obj = malloc(sizeof(struct plane));
        assert(obj != NULL);
        
        sll_insert_head(list, obj);
    }
    
    for (size_t i = 0; i < 2500; ++i)
    {
        free(sll_remove_tail(list));
    }
        
    //then no memory leaks are found by DynamoRio after the list is destroyed
    sll_destroy(list);
    
    return 1;
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_insert_at_head_of_empty_list_is_successful);
        RUN_TEST(test_insert_at_tail_of_non_empty_list_is_successful);
        RUN_TEST(test_insert_in_middle_of_non_empty_list_is_successful);
    UNITY_END();
    
    INTEGRATION_BEGIN();
        int status = integration_test_does_not_result_in_a_memory_leak();
    INTEGRATION_END();
    
    return EXIT_SUCCESS;
}