#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sll.h"
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
//custom free function and a struct for more complicated tests

void custom_free(void *list)
{
    struct sll *s = list;
    
    while (s->head != NULL) free(sll_remove_head(s));
    
    free(s);
}

struct plane
{
    double x;
    double y;
};

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
    
    //afterwards clean up memory
    sll_destroy(list);
}

/******************************************************************************/

void test_removal_of_head_makes_the_second_node_the_new_head(void)
{
    //given a singly linked list with two nodes
    struct sll *list = sll_create(free);
    sll_insert_head(list, "methane");
    sll_insert_head(list, "propane");
    
    //when the head node is removed
    sll_remove_head(list);
    
    //then the second node becomes the new head
    TEST_ASSERT_EQUAL_STRING("methane", sll_access_head_data(list));
    TEST_ASSERT_EQUAL_INT(1, sll_size(list));
    
    //afterwards clean up memory
    sll_destroy(list);
}

/******************************************************************************/

void test_removal_of_all_nodes_retains_null_head_pointer(void)
{
    //given a singly linked list with two nodes
    struct sll *list = sll_create(free);
    sll_insert_head(list, "methane");
    sll_insert_head(list, "propane");
    
    //when both nodes are removed
    sll_remove_head(list);
    sll_remove_head(list);
    
    //then the struct sll members are set as expected
    TEST_ASSERT_EQUAL_INT(0, sll_size(list));
    TEST_ASSERT_NULL(list->head);
    
    //afterwards clean up memory
    sll_destroy(list);
}

/******************************************************************************/

void test_search_for_existing_data_is_successful(void)
{
    //given a singly linked list with a few nodes
    struct sll *list = sll_create(free);
    sll_insert_head(list, "butane");
    sll_insert_head(list, "methane");
    sll_insert_head(list, "propane");
    
    //when we search for a piece of data
    struct node *found_item = sll_search_data(list, "propane");
    
    //then we successfully find it
    TEST_ASSERT_NOT_NULL(found_item);
    TEST_ASSERT_EQUAL_STRING("propane", found_item->datum);
    
    //afterwards clean up memory
    sll_destroy(list);
}

/******************************************************************************/

void test_search_for_non_existent_data_is_a_failure(void)
{
    //given a singly linked list with a few nodes
    struct sll *list = sll_create(free);
    sll_insert_head(list, "butane");
    sll_insert_head(list, "methane");
    sll_insert_head(list, "propane");
    
    //when we search for a piece of data
    struct node *found_item = sll_search_data(list, "nitrogen");
    
    //then we do not succeed in finding it
    TEST_ASSERT_NULL(found_item);
    
    //afterwards clean up memory
    sll_destroy(list);
}

/******************************************************************************/
//integration test for memory leaks uses DynamoRio. unity framework not used.

int integration_test_does_not_result_in_a_memory_leak(void)
{
    //given a singly linked list
    struct sll *list = sll_create(custom_free);
    
    //when a large amount of insertions and removals is performed
    for (size_t i = 0; i < 5000; ++i)
    {
        struct plane *obj = malloc(sizeof(struct plane));
        assert(obj != NULL);
        obj->x = 1.0;
        obj->y = 2.0;
        
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
        RUN_TEST(test_removal_of_head_makes_the_second_node_the_new_head);
        RUN_TEST(test_removal_of_all_nodes_retains_null_head_pointer);
        RUN_TEST(test_search_for_existing_data_is_successful);
        RUN_TEST(test_search_for_non_existent_data_is_a_failure);
    UNITY_END();
    
    INTEGRATION_BEGIN();
        int status = integration_test_does_not_result_in_a_memory_leak();
    INTEGRATION_END();
    
    return EXIT_SUCCESS;
}