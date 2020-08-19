/*
* Author: Biren Patel
* Description: Unit tests for chained hash table using Unity framework
*/

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "src/unity.h"
#include "hash.h"

/******************************************************************************/

void setUp(void) {}
void tearDown(void) {}

/******************************************************************************/

uint32_t hash(const char *key, uint32_t m)
{
    uint32_t hash = 0;
    
    while (*key)
    {
        hash += *key++;
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    
    return (uint32_t) (((uint64_t) hash * (uint64_t) m) >> 32);
}

/******************************************************************************/

void test_hash_table_initialization(void)
{
    //arrange
    htab ht = htab_create(10);
    
    //assert
    TEST_ASSERT_EQUAL_DOUBLE(0.0, ht->LF);
    TEST_ASSERT_EQUAL_UINT32(10, ht->capacity);
    TEST_ASSERT_EQUAL_UINT32(0, ht->count);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_insert_into_empty_slot(void)
{
    //arrange
    htab ht = htab_create(10);
    
    //act
    bool status = htab_insert(ht, "free the runcorn 4", 42);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_UINT32(1, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(0.1, ht->LF);
    TEST_ASSERT_EQUAL_STRING(ht->slots[4].key, "free the runcorn 4");
    TEST_ASSERT_EQUAL_INT64(ht->slots[4].value, 42);
    TEST_ASSERT_NULL(ht->slots[4].next);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_insert_into_slot_with_occupied_head(void)
{
    //arrange
    htab ht = htab_create(10);
    htab_insert(ht, "robin hood and little john", 99);
    
    //act
    bool status = htab_insert(ht, "earwax jimmies", 11);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_UINT32(2, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(0.2, ht->LF);
    TEST_ASSERT_EQUAL_STRING(ht->slots[6].next->key, "earwax jimmies");
    TEST_ASSERT_EQUAL_INT64(ht->slots[6].next->value, 11);
    TEST_ASSERT_NULL(ht->slots[6].next->next);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_override_head_slot_value(void)
{
    //arrange
    htab ht = htab_create(10);
    htab_insert(ht, "vote pedro", 7);
    
    //act
    bool status = htab_insert(ht, "vote pedro", 8);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_UINT32(1, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(0.1, ht->LF);
    TEST_ASSERT_EQUAL_STRING(ht->slots[0].key, "vote pedro");
    TEST_ASSERT_EQUAL_INT64(ht->slots[0].value, 8);
    TEST_ASSERT_NULL(ht->slots[0].next);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_override_tail_slot_value(void)
{
    //arrange
    htab ht = htab_create(10);
    htab_insert(ht, "robin hood and little john", 99);
    htab_insert(ht, "earwax jimmies", 11);
    
    //act
    bool status = htab_insert(ht, "earwax jimmies", 39);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_UINT32(2, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(0.2, ht->LF);
    TEST_ASSERT_EQUAL_STRING(ht->slots[6].next->key, "earwax jimmies");
    TEST_ASSERT_EQUAL_INT64(ht->slots[6].next->value, 39);
    TEST_ASSERT_NULL(ht->slots[6].next->next);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_search_for_absent_key(void)
{
    //arrange
    htab ht = htab_create(10);
    
    //act
    int64_t value = -1;
    bool status = htab_search(ht, "free the runcorn 4", &value);
    
    //assert
    TEST_ASSERT_FALSE(status);
    TEST_ASSERT_EQUAL_INT64(-1, value);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_search_for_present_key_not_at_head(void)
{
    //arrange
    htab ht = htab_create(10);
    htab_insert(ht, "robin hood and little john", 99);
    htab_insert(ht, "earwax jimmies", 11);
    
    //act
    int64_t value;
    bool status = htab_search(ht, "earwax jimmies", &value);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT64(11, value);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_remove_head_from_head_only_slot(void)
{
    //arrange
    htab ht = htab_create(1);
    htab_insert(ht, "ABC", 10);
    
    //act
    int64_t rem_value;
    bool status = htab_remove(ht, "ABC", &rem_value);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT64(10, rem_value);
    TEST_ASSERT_EQUAL_STRING("\0", ht->slots[0].key);
    TEST_ASSERT_EQUAL_UINT32(0, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, ht->LF);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/
//as above but need to check that a chain pushes the next element back into the
//table slot

void test_remove_head_from_slot_with_multiple_elements(void)
{
    //arrange
    htab ht = htab_create(1);
    htab_insert(ht, "ABC", 10);
    htab_insert(ht, "DEF", 20);
    htab_insert(ht, "GHI", 30);
    
    //act
    int64_t rem_value;
    bool status = htab_remove(ht, "ABC", &rem_value);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT64(10, rem_value);
    TEST_ASSERT_EQUAL_STRING("DEF", ht->slots[0].key);
    TEST_ASSERT_EQUAL_INT64(20, ht->slots[0].value);
    TEST_ASSERT_EQUAL_STRING("GHI", ht->slots[0].next->key);
    TEST_ASSERT_EQUAL_UINT32(2, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, ht->LF);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_remove_non_head_from_slot_with_multiple_elements(void)
{
    //arrange
    htab ht = htab_create(1);
    htab_insert(ht, "ABC", 10);
    htab_insert(ht, "DEF", 20);
    htab_insert(ht, "GHI", 30);
    
    //act
    int64_t rem_value;
    bool status = htab_remove(ht, "GHI", &rem_value);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_INT64(30, rem_value);
    TEST_ASSERT_NULL(ht->slots[0].next->next);
    TEST_ASSERT_EQUAL_UINT32(2, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(2.0, ht->LF);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_remove_empty_slot_is_false(void)
{
    //arrange
    htab ht = htab_create(10);
    htab_insert(ht, "shrimp and grits", 20);
    
    //act
    bool status = htab_remove(ht, "shrimp gumbo", NULL);
    
    //assert
    TEST_ASSERT_FALSE(status);
    TEST_ASSERT_EQUAL_UINT32(1, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(0.1, ht->LF);
    
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

void test_dynamic_resize(void)
{
    //arrange
    htab ht = htab_create(1);
    htab_insert(ht, "cat", 10);
    htab_insert(ht, "pig", 20);
    
    //act
    bool status = htab_resize(&ht);
    
    //arrange
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_UINT32(2, ht->count);
    TEST_ASSERT_EQUAL_UINT32(2, ht->capacity);
    TEST_ASSERT_EQUAL_DOUBLE(1, ht->LF);
    TEST_ASSERT_EQUAL_STRING("cat", ht->slots[0].key);
    TEST_ASSERT_EQUAL_STRING("pig", ht->slots[1].key);
    TEST_ASSERT_NULL(ht->slots[0].next);
    TEST_ASSERT_NULL(ht->slots[1].next);
    
    //clean
    htab_destroy(ht);
}

/******************************************************************************/

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_hash_table_initialization);
        RUN_TEST(test_insert_into_empty_slot);
        RUN_TEST(test_insert_into_slot_with_occupied_head);
        RUN_TEST(test_override_head_slot_value);
        RUN_TEST(test_override_tail_slot_value);
        RUN_TEST(test_search_for_absent_key);
        RUN_TEST(test_search_for_present_key_not_at_head);
        RUN_TEST(test_remove_head_from_head_only_slot);
        RUN_TEST(test_remove_head_from_slot_with_multiple_elements);
        RUN_TEST(test_remove_non_head_from_slot_with_multiple_elements);
        RUN_TEST(test_remove_empty_slot_is_false);
        RUN_TEST(test_dynamic_resize);
    UNITY_END();
    
    return EXIT_SUCCESS;
}