/*
* Author: Biren Patel
* Description: implementation for chained hash table with dynamic resizing.
*/

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "hash.h"

/******************************************************************************/
//Jenkin's one-at-a-time with biased integer multiplication mapping

static inline uint32_t hash(const char *key, uint32_t m)
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

struct hash_table *htab_create(uint32_t capacity, double LF_threshold)
{
    //no need to worry about offset of slots b/c fully packed
    size_t bytes = sizeof(struct hash_table) + sizeof(struct node) * capacity;
    struct hash_table *ht = malloc(bytes);
    if (ht == NULL) return NULL;
    
    //set metadata
    if (LF_threshold > 0) ht->LF_threshold = LF_threshold;
    else ht->LF_threshold = 1.0;
    
    //current load factor, once it is above threshold, it only triggers the
    //dynamic resizing on the next insert call.
    ht->LF = 0.0;
    
    ht->capacity = capacity;
    ht->count = 0;
    
    //zero out the first char in each node as an indicator that it is open
    for (size_t i = 0; i < capacity; ++i)
    {
        ht->slots[i].key[0] = '\0';
    }
    
    return ht;
}

/******************************************************************************/

bool htab_insert(struct hash_table *ht, const char *key, const int64_t value)
{
    assert(ht != NULL && "hash table pointer is null");
    
    //table index at which to place kv pair
    uint32_t idx = hash(key, ht->capacity);
    
    if (ht->slots[idx].key[0] == '\0')
    {
        //slot is available, add kv pair here
        strcpy(ht->slots[idx].key, key);
        ht->slots[idx].value = value;
        ht->slots[idx].next = NULL;
    }
    else
    {
        if (strcmp(ht->slots[idx].key, key) == 0)
        {
            //kv pair already in table at list head, do nothing
            return true;
        }
        else
        {
            //slot is unavailable, add kv pair to linked list
            //walk list, check for key at each node, and finally add to tail
            return false;
        }
    }
    
    //update load factor for the next insert call
    ht->LF = (double) ++ht->count / (double) ht->capacity;
    
    return true;
}