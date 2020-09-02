/*
* Author: Biren Patel
* Description: Memory Pool API. The user must call mempool_init() before using
* the dynamic allocation functions.
*/

#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stdbool.h>
#include <stddef.h>

/******************************************************************************/
//constructors

bool mempool_init(size_t size);
bool mempool_free(void);

/******************************************************************************/
//dynamic allocation functions 

void *pmalloc(size_t size);
void *pcalloc(size_t n, size_t size);
void pfree(void *ptr);

/******************************************************************************/
//stdout debugger, show pool memory from offset word x to x + y

void memmap(size_t start, size_t end);

#endif