/*
* Author: Biren Patel
* Description: Unit tests for doubly linked list with Unity framework
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "list.h"
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

void test_insert_at_head_of_empty_list(void)
{
    //given an empty list
    struct list *list = list_create(NULL);

    //when an item is pushed to the front
    list_push_head(list, "A");

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("A", list_peek_head(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_insert_at_head_of_non_empty_list(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "B");

    //when an item is pushed to the front
    list_push_head(list, "A");

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("A", list_peek_head(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_insert_at_tail_of_non_empty_list(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "A");

    //when an item is pushed to the end
    list_push_tail(list, "B");

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("B", list_peek_tail(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_insert_at_tail_twice_in_non_empty_list(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "A");

    //when an item is pushed to the end twice
    list_push_tail(list, "B");
    list_push_tail(list, "C");

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("C", list_peek_tail(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_insert_in_middle_of_non_empty_list(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "A");
    list_push_tail(list, "C");

    //when an item is inserted into the middle
    list_insert_pos(list, 1, "B");

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("B", list_access_pos(list, 1));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_size_macro_is_correct(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "E");
    list_push_head(list, "D");
    list_push_head(list, "C");
    list_push_head(list, "B");
    list_push_head(list, "A");

    //when the size is requested
    int size = list_size(list);

    //then it is successful
    TEST_ASSERT_EQUAL_INT(5, size);

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_forward_pass_from_head_to_tail_is_not_broken(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "E");
    list_push_head(list, "D");
    list_push_head(list, "C");
    list_push_head(list, "B");
    list_push_head(list, "A");

    //when a forward pass is attempted
    struct list_node *SUT = list_search(list, "E", 1, strcmp);

    //then it is successful
    TEST_ASSERT_NOT_NULL(SUT);
    TEST_ASSERT_EQUAL_STRING("E", list_peek_tail(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_backward_pass_from_tail_to_head_is_not_broken(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "E");
    list_push_head(list, "D");
    list_push_head(list, "C");
    list_push_head(list, "B");
    list_push_head(list, "A");

    //when a backward pass is attempted
    struct list_node *SUT = list_search(list, "A", 2, strcmp);

    //then it is successful
    TEST_ASSERT_NOT_NULL(SUT);
    TEST_ASSERT_EQUAL_STRING("A", list_peek_head(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_removal_at_head_of_non_empty_list(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "E");
    list_push_head(list, "D");
    list_push_head(list, "C");
    list_push_head(list, "B");
    list_push_head(list, "A");

    //when a removal at the head is performed
    void *SUT = list_pop_head(list);

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("A", SUT);
    TEST_ASSERT_EQUAL_STRING("B", list_peek_head(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_removal_at_tail_of_non_empty_list(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "E");
    list_push_head(list, "D");
    list_push_head(list, "C");
    list_push_head(list, "B");
    list_push_head(list, "A");

    //when a removal at the tail is performed
    void *SUT = list_pop_tail(list);

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("E", SUT);
    TEST_ASSERT_EQUAL_STRING("D", list_peek_tail(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_removal_in_middle_of_list_does_not_break_list_links(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "E");
    list_push_head(list, "D");
    list_push_head(list, "C");
    list_push_head(list, "B");
    list_push_head(list, "A");

    //when a removal at the middle is performed
    void *SUT = list_remove_pos(list, 2);

    //then it is successful in retaining all forward and backward links
    TEST_ASSERT_EQUAL_STRING("C", SUT);
    TEST_ASSERT_NOT_NULL(list_search(list, "E", 1, strcmp));
    TEST_ASSERT_NOT_NULL(list_search(list, "A", 2, strcmp));

    //afterwards clean up memory
    list_destroy(list);
}


/******************************************************************************/

void test_access_in_middle_of_list(void)
{
    //given a list
    struct list *list = list_create(NULL);
    list_push_head(list, "E");
    list_push_head(list, "D");
    list_push_head(list, "C");
    list_push_head(list, "B");
    list_push_head(list, "A");

    //when an access attempt at the middle is performed
    void *SUT = list_access_pos(list, 2);

    //then it is successful without causing a removal
    TEST_ASSERT_EQUAL_STRING("C", SUT);
    TEST_ASSERT_NOT_NULL(list_search(list, "C", 1, strcmp));
    TEST_ASSERT_EQUAL_INT(5, list_size(list));

    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/

void test_concat_two_lists_is_successful(void)
{
    //given two lists
    struct list *list_1 = list_create(NULL);
    struct list *list_2 = list_create(NULL);

    list_push_head(list_1, "C");
    list_push_head(list_1, "B");
    list_push_head(list_1, "A");

    list_push_head(list_2, "F");
    list_push_head(list_2, "E");
    list_push_head(list_2, "D");

    //when list 2 is concatenated to list 1
    list_concat(list_1, list_2);
    list_destroy(list_2);

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("D", list_access_pos(list_1, 3));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "F", 1, strcmp));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "A", 2, strcmp));
    TEST_ASSERT_EQUAL_INT(6, list_size(list_1));

    //afterwards clean up memory
    list_destroy(list_1);

}

/******************************************************************************/

void test_deep_copy_two_lists_is_successful(void)
{
    //given two lists
    struct list *list_1 = list_create(NULL);
    struct list *list_2 = list_create(NULL);

    list_push_head(list_1, "C");
    list_push_head(list_1, "B");
    list_push_head(list_1, "A");

    list_push_head(list_2, "F");
    list_push_head(list_2, "E");
    list_push_head(list_2, "D");

    //when a copy of list 2 is performed
    list_copy(list_1, list_2);

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("D", list_access_pos(list_1, 3));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "F", 1, strcmp));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "A", 2, strcmp));
    TEST_ASSERT_EQUAL_INT(6, list_size(list_1));
    TEST_ASSERT_NOT_NULL(list_search(list_2, "F", 1, strcmp));
    TEST_ASSERT_NOT_NULL(list_search(list_2, "D", 2, strcmp));

    //afterwards clean up memory
    list_destroy(list_1);
    list_destroy(list_2);
}

/******************************************************************************/

void test_removal_of_first_new_node_after_concatenation(void)
{
    //given two lists
    struct list *list_1 = list_create(NULL);
    struct list *list_2 = list_create(NULL);

    list_push_head(list_1, "C");
    list_push_head(list_1, "B");
    list_push_head(list_1, "A");

    list_push_head(list_2, "F");
    list_push_head(list_2, "E");
    list_push_head(list_2, "D");

    //when the first new node after a concatenation is removed
    void *SUT = list_remove_node(list_1, list_concat(list_1, list_2), 0);

    //then it does not disrupt concatenation
    TEST_ASSERT_EQUAL_STRING("D", SUT);
    TEST_ASSERT_EQUAL_INT(5, list_size(list_1));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "F", 1, strcmp));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "A", 2, strcmp));

    //afterwards clean up memory
    list_destroy(list_1);
    list_destroy(list_2);
}

/******************************************************************************/

void test_insert_node_before_first_new_node_after_concat(void)
{
    //given two lists
    struct list *list_1 = list_create(NULL);
    struct list *list_2 = list_create(NULL);

    list_push_head(list_1, "C");
    list_push_head(list_1, "B");
    list_push_head(list_1, "A");

    list_push_head(list_2, "F");
    list_push_head(list_2, "E");
    list_push_head(list_2, "D");

    //when a new node is inserted before first new node after concatenation
    struct list_node *first_new_node = list_concat(list_1, list_2);
    list_insert_node(list_1, first_new_node, "Z", 2);

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("Z", list_access_pos(list_1, 3));
    TEST_ASSERT_EQUAL_INT(7, list_size(list_1));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "F", 1, strcmp));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "A", 2, strcmp));

    //afterwards clean up memory
    list_destroy(list_1);
    list_destroy(list_2);
}

/******************************************************************************/

void test_insert_node_after_first_new_node_after_concat(void)
{
    //given two lists
    struct list *list_1 = list_create(NULL);
    struct list *list_2 = list_create(NULL);

    list_push_head(list_1, "C");
    list_push_head(list_1, "B");
    list_push_head(list_1, "A");

    list_push_head(list_2, "F");
    list_push_head(list_2, "E");
    list_push_head(list_2, "D");

    //when a new node is inserted after first new node after concatenation
    struct list_node *first_new_node = list_concat(list_1, list_2);
    list_insert_node(list_1, first_new_node, "Z", 1);

    //then it is successful
    TEST_ASSERT_EQUAL_STRING("Z", list_access_pos(list_1, 4));
    TEST_ASSERT_EQUAL_INT(7, list_size(list_1));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "F", 1, strcmp));
    TEST_ASSERT_NOT_NULL(list_search(list_1, "A", 2, strcmp));

    //afterwards clean up memory
    list_destroy(list_1);
    list_destroy(list_2);
}

/******************************************************************************/

void test_typedef_lists_and_nodes_are_accepted(void)
{
    //given a list typedef'd
    List list = list_create(NULL);
    Node head = list_push_head(list, "A");
    
    //when a typedef'd node is inserted
    list_insert_node(list, head, "B", 1);
    
    //then it is successful
    TEST_ASSERT_EQUAL_STRING("B", list_peek_tail(list));
    
    //afterwards clean up memory
    list_destroy(list);
}

/******************************************************************************/
//integration test with DynamoRio, not Unity

struct point
{
    double x;
    double y;
};

int integration_test_does_not_result_in_a_memory_leak(void)
{
    //given a list
    struct list *list = list_create(free);

    //when a large amount of data is enqueued
    for (size_t i = 0; i < 2500; ++i)
    {
        struct point *p = malloc(sizeof(struct point));
        assert(p != NULL && "integration test malloc failure");

        p->x = 1.0;
        p->y = 2.0;

        i % 2 == 0
            ? list_push_head(list, p)
            : list_push_tail(list, p);
    }

    //when a large amount of dequeuing occurs
    for (size_t i = 0; i < 1000; ++i)
    {
        free(list_pop_tail(list));
        free(list_pop_head(list));
    }

    //when the list is destroyed
    list_destroy(list);

    //then DynamoRio will not detect a memory leak
    return 1;
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_insert_at_head_of_empty_list);
        RUN_TEST(test_insert_at_head_of_non_empty_list);
        RUN_TEST(test_insert_at_tail_of_non_empty_list);
        RUN_TEST(test_insert_at_tail_twice_in_non_empty_list);
        RUN_TEST(test_insert_in_middle_of_non_empty_list);
        RUN_TEST(test_size_macro_is_correct);
        RUN_TEST(test_forward_pass_from_head_to_tail_is_not_broken);
        RUN_TEST(test_backward_pass_from_tail_to_head_is_not_broken);
        RUN_TEST(test_removal_at_head_of_non_empty_list);
        RUN_TEST(test_removal_at_tail_of_non_empty_list);
        RUN_TEST(test_removal_in_middle_of_list_does_not_break_list_links);
        RUN_TEST(test_access_in_middle_of_list);
        RUN_TEST(test_concat_two_lists_is_successful);
        RUN_TEST(test_deep_copy_two_lists_is_successful);
        RUN_TEST(test_removal_of_first_new_node_after_concatenation);
        RUN_TEST(test_insert_node_before_first_new_node_after_concat);
        RUN_TEST(test_insert_node_after_first_new_node_after_concat);
        RUN_TEST(test_typedef_lists_and_nodes_are_accepted);
    UNITY_END();

    INTEGRATION_BEGIN();
        int status = integration_test_does_not_result_in_a_memory_leak();
    INTEGRATION_END();

    return EXIT_SUCCESS;
}