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
//list heads embedded directly in the table for better cache locality.

typedef struct hash_table
{
    double LF_threshold;
    double LF;
    uint32_t capacity;
    uint32_t count;
    struct node slots[];
} * htab;

/******************************************************************************/
//API

struct hash_table *htab_create(uint32_t capacity, double LF_threshold);
void htab_destroy(struct hash_table *ht);
bool htab_insert(struct hash_table *ht, const char *key, const int64_t value);
bool htab_search(struct hash_table *ht, char *key, int64_t *value);
bool htab_remove(struct hash_table *ht, char *key);

#endif