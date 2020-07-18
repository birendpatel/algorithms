/*
* Author: Biren Patel
* Description: Unit tests for stack ADT via Unity framework
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"
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

void test_push_item_onto_stack_returns_true(void)
{
    //given a stack
    stack s = stack_create(NULL);
    
    //when an item is pushed
    bool status = stack_push(s, "A");
    
    //then it returns true
    TEST_ASSERT_TRUE(status);
    
    //afterwards clean up memory
    stack_destroy(s);
}

/******************************************************************************/

void test_push_several_items_onto_stack_updates_size(void)
{
    //given a stack
    stack s = stack_create(NULL);
    
    //when several items are pushed
    stack_push(s, "D");
    stack_push(s, "C");
    stack_push(s, "B");
    stack_push(s, "A");
    
    //then it is successful
    TEST_ASSERT_EQUAL_INT(4, stack_size(s));
    
    //afterwards clean up memory
    stack_destroy(s);
}

/******************************************************************************/

void test_pop_item_off_stack(void)
{
    //given a stack
    stack s = stack_create(NULL);
    stack_push(s, "D");
    stack_push(s, "C");
    stack_push(s, "B");
    stack_push(s, "A");
    
    //when an item is popped off
    char *str = stack_pop(s);
    
    //then it is successful
    TEST_ASSERT_EQUAL_STRING("A", str);
    TEST_ASSERT_EQUAL_INT(3, stack_size(s));
    
    //afterwards clean up memory
    stack_destroy(s);
}

/******************************************************************************/

void test_peek_at_stack_does_not_pop_top_off(void)
{
    //gievn a stack
    stack s = stack_create(NULL);
    stack_push(s, "D");
    stack_push(s, "C");
    stack_push(s, "B");
    stack_push(s, "A");
    
    //when we peek at the top item
    char *str = stack_peek(s);
    
    //then it is not removed
    TEST_ASSERT_EQUAL_STRING("A", str);
    TEST_ASSERT_EQUAL_INT(4, stack_size(s));
    
    //afterwards clean up memory
    stack_destroy(s);
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_push_item_onto_stack_returns_true);
        RUN_TEST(test_push_several_items_onto_stack_updates_size);
        RUN_TEST(test_pop_item_off_stack);
        RUN_TEST(test_peek_at_stack_does_not_pop_top_off);
    UNITY_END();
    
    return EXIT_SUCCESS;
}