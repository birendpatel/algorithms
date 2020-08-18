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
    
    //nul char at key[0] used as an indicator for next action
    if (ht->slots[idx].key[0] == '\0')
    {
        //head node at slot is available, add kv pair here
        strcpy(ht->slots[idx].key, key);
        ht->slots[idx].value = value;
        ht->slots[idx].next = NULL;
    }
    else
    {
        //head node not available, must navigate the linked list
        struct node *curr = &ht->slots[idx];
        assert(curr != NULL && "curr ptr assignment to available slot");
        
        while(1)
        {
            if (strcmp(curr->key, key) == 0)
            {
                //found matching key, replace the value
                curr->value = value;
                goto skip_metadata_update;
            }
            else if (curr->next == NULL)
            {
                //reached tail without match, add new tail node
                struct node *new = malloc(sizeof(struct node));
                if (new == NULL) return false;
                
                strcpy(new->key, key);
                new->value = value;
                new->next = NULL;
                curr->next = new;
                
                break;
            }
            else curr = curr->next;
            
            assert(curr != NULL && "broken list link in table slot");
        }
    }
    
    //update load factor for the next insert call
    ht->LF = (double) ++ht->count / (double) ht->capacity;
    
    skip_metadata_update:
    return true;
}

/******************************************************************************/

bool htab_search(struct hash_table *ht, const char *key, int64_t *value)
{
    assert(ht != NULL && "hash table pointer is null");
    
    uint32_t idx = hash(key, ht->capacity);
    
    //nul char at key[0] indicates entire slot is empty
    if (ht->slots[idx].key[0] == '\0') return false;
    
    //otherwise walk the list to scan for the key
    struct node *curr = &ht->slots[idx];

    while (curr != NULL)
    {
        if (strcmp(curr->key, key) == 0)
        {
            *value = curr->value;
            return true;
        }
        
        curr = curr->next;
    }
    
    //couldn't find key in list, ignore value pointer
    return false;
}