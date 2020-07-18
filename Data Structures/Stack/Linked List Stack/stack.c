/*
* Author: Biren Patel
* Description: Stack ADT implementation via doubly linked list of void pointers.
*/

#include <stdbool.h>
#include <stdint.h>
#include "stack.h"
#include "dll.h"

/*******************************************************************************
* public functions
*******************************************************************************/

stack stack_create(void (*destroy)(void *data))
{
    return dll_create(destroy);
}

/******************************************************************************/

void stack_destroy(stack s)
{
    dll_destroy(s);
}

/******************************************************************************/

bool stack_push(stack s, void *item)
{
    if (dll_push_head(s, item) == NULL) return false;
    else return true;
}

/******************************************************************************/

void *stack_pop(stack s)
{
    return dll_pop_head(s);
}

/******************************************************************************/

void *stack_peek(stack s)
{
    return dll_peek_head(s);
}

/******************************************************************************/

uint32_t stack_size(stack s)
{
    return dll_size(s);
}