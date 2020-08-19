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
//Jenkin's one-at-a-time hash with biased integer multiplication mapping

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

struct hash_table *htab_create(uint32_t capacity)
{
    //no need to worry about offset of slots b/c fully packed
    size_t bytes = sizeof(struct hash_table) + sizeof(struct node) * capacity;
    struct hash_table *ht = malloc(bytes);
    if (ht == NULL) return NULL;
    
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

void htab_destroy(struct hash_table *ht)
{
    assert(ht != NULL && "hash table pointer is null");
    
    //whenever a slot is not available destroy the linked list stored there
    for (uint32_t i = 0; i < ht->capacity; ++i)
    {
        if (ht->slots[i].key[0] != '\0' && ht->slots[i].next != NULL)
        {
            struct node *curr = ht->slots[i].next;
            
            while(curr != NULL)
            {
                struct node *next_node = curr->next;
                free(curr);
                curr = next_node;
            }
        }
    }
    
    free(ht);
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
    
    //update load factor
    ht->LF = (double) ++ht->count / (double) ht->capacity;
    
    skip_metadata_update:
        return true;
}

/******************************************************************************/

bool htab_search(struct hash_table *ht, const char *key, int64_t *value)
{
    assert(ht != NULL && "hash table pointer is null");
    assert(value != NULL && "value pointer is null");
    
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

/******************************************************************************/

bool htab_remove(struct hash_table *ht, const char *key, int64_t *value)
{
    //value arg is optional
    assert(ht != NULL && "hash table pointer is null");
    
    uint32_t idx = hash(key, ht->capacity);
    
    //walk the list with lead and lag pointers for removal
    struct node *lead = &ht->slots[idx];
    
    if (lead->key[0] == '\0') return false; //slot is entirely open
    
    struct node *lag = NULL;
    uint32_t i = 0; //indicates head slot replacement when 0
    
    while (lead != NULL)
    {
        if (strcmp(lead->key, key) == 0)
        {
            //first things first, pass back the value if required
            if (value != NULL) *value = lead->value;
            
            if (i == 0) //head removal
            {
                if (lead->next == NULL)
                {
                    //head is the only element. zero out first byte in slot.
                    lead->key[0] = '\0';
                    goto update_lf;
                }
                else
                {
                    //option 2, move next node directly into the table.
                    lag = lead;
                    lead = lead->next;
                    
                    strcpy(lag->key, lead->key);
                    lag->value = lead->value;
                    lag->next = lead->next;
                    
                    free(lead);
                    
                    goto update_lf;
                }
            }
            else //non-head removal
            {
                lag->next = lead->next;
                free(lead);
                goto update_lf;
            }
        }
        
        //no match here
        lag = lead;
        lead = lead->next;
        ++i;
    }
    
    //couldn't find key in list, ignore value pointer
    return false;
    
    //load factor and count updates on successful removal
    update_lf:
        ht->LF = (double) --ht->count / ht->capacity;
        return true;
}

/******************************************************************************/

bool htab_resize(struct hash_table **ht)
{
    struct hash_table *old_ht = *ht;
    
    uint32_t new_capacity = grow(old_ht->capacity);
    assert(new_capacity > old_ht->capacity && "capacity overflow");
    
    //instead of reallocation, a new hash table receives rehashed contents
    struct hash_table *new_ht = htab_create(new_capacity);
    
    if (new_ht == NULL) return false;
    
    //walk old table and insert contents into new table
    for (uint32_t i = 0; i < old_ht->capacity; ++i)
    {
        if (old_ht->slots[i].key[0] != '\0')
        {
            struct node *curr = &old_ht->slots[i];
            
            while (curr != NULL)
            {
                bool status = htab_insert(new_ht, curr->key, curr->value);
                if (status == false) return false;
                curr = curr->next;
            }
        }
    }
    
    htab_destroy(old_ht);
    
    *ht = new_ht;
    return true;
}