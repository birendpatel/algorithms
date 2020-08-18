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
    htab ht = htab_create(10, 0.7);
    
    //assert
    TEST_ASSERT_EQUAL_DOUBLE(0.7, ht->LF_threshold);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, ht->LF);
    TEST_ASSERT_EQUAL_UINT32(10, ht->capacity);
    TEST_ASSERT_EQUAL_UINT32(0, ht->count);
}

/******************************************************************************/

void test_insert_into_empty_slot(void)
{
    //arrange
    htab ht = htab_create(10, 0.7);
    
    //act
    bool status = htab_insert(ht, "free the runcorn 4", 42);
    
    //assert
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_EQUAL_UINT32(1, ht->count);
    TEST_ASSERT_EQUAL_DOUBLE(0.1, ht->LF);
    TEST_ASSERT_EQUAL_STRING(ht->slots[4].key, "free the runcorn 4");
    TEST_ASSERT_EQUAL_INT64(ht->slots[4].value, 42);
    TEST_ASSERT_NULL(ht->slots[4].next);
}

/******************************************************************************/

void test_insert_into_slot_with_occupied_head(void)
{
    //arrange
    htab ht = htab_create(10, 0.7);
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
}

/******************************************************************************/

void test_override_head_slot_value(void)
{
    //arrange
    htab ht = htab_create(10, 0.7);
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
}

/******************************************************************************/

void test_override_tail_slot_value(void)
{
    //arrange
    htab ht = htab_create(10, 0.7);
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
}

/******************************************************************************/

void test_search_for_absent_key(void)
{
    //arrange
    htab ht = htab_create(10, 0.7);
    
    //act
    int64_t value = -1;
    bool status = htab_search(ht, "free the runcorn 4", &value);
    
    //assert
    TEST_ASSERT_FALSE(status);
    TEST_ASSERT_EQUAL_INT64(-1, value);
    TEST_ASSERT_EQUAL_STRING("\0", ht->slots[4].key);
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
    UNITY_END();
    
    return EXIT_SUCCESS;
}