#include <stdio.h>
#include <stdlib.h>

#include "sll.h"
#include "src/unity.c"


/******************************************************************************/

void setUp(void) {}
void tearDown(void) {}

/******************************************************************************/

void test_insert_at_head_of_empty_list_is_successful(void)
{
    //given an empty singly linked list
    struct sll *list = sll_create(free);
    
    //when an element is inserted at the head
    sll_insert_head(list, "butane");
    char *str = sll_access_head_data(list);
    
    //then the insertion is successful
    TEST_ASSERT_EQUAL_STRING("butane", str);
    
    //afterwards clean up memory
    sll_destroy(list);    
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_insert_at_head_of_empty_list_is_successful);
    UNITY_END();
    
    return EXIT_SUCCESS;
}