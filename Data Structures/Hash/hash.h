/*
* Author: Biren Patel
* Description: API for chained hash table with dynamic resizing, keys are 64
* byte strings and values are 8 byte integer.
*/

#ifndef CHAINED_HASH_H
#define CHAINED_HASH_H

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************/
//TBH the reason for 64 bit value is to completely pack the struct in this order

struct node
{
    char key[64];
    int64_t value;
    struct node *next;
};

/******************************************************************************/
//list heads embedded directly in the table for better cache locality

typedef struct hash_table
{
    double LF;
    uint32_t capacity;
    uint32_t count;
    struct node slots[];
} * htab;

/******************************************************************************/
//API

struct hash_table *htab_create(uint32_t capacity);

//ht is dangling after destruction
void htab_destroy(struct hash_table *ht);

//duplicate key insertion overrides previous key-value pair. returns false
//if malloc fails during chain node creation.
bool htab_insert(struct hash_table *ht, const char *key, const int64_t value);

//value pointer must not be null, returns false if key DNE.
bool htab_search(struct hash_table *ht, const char *key, int64_t *value);

//use null value to ignore storage of removed kv pair. false if key DNE.
bool htab_remove(struct hash_table *ht, const char *key, int64_t *value);

//dynamic resize must be called explicitly by user and its grow macro must be
//monotonic increasing. returns false if any malloc call fails.
#define grow(n) (2 * n)
bool htab_resize(struct hash_table **ht);

#endif