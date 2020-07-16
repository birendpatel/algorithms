/*
* Author: Biren Patel
* Description: Doubly linked list implementation
*/

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "dll.h"


/*******************************************************************************
* public functions
*******************************************************************************/

struct dll *dll_create(void (*destroy)(void *data))
{}


/******************************************************************************/

void dll_destroy(struct dll *list)
{}


/******************************************************************************/

struct dll_node *dll_insert_idx(struct dll *list, dll_item datum)
{}