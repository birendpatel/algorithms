/*
* author: Biren Patel
* description: fixed-length stack data structure
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "stack.h"

/*******************************************************************************
* structure: struct stack
* @ data : contains items pushed by client
* @ top_of_stack : provides an index for data array
*******************************************************************************/
struct stack
{
    stack_item data[STACK_SIZE];
    int top_of_stack;
};

/*******************************************************************************
* public functions
*******************************************************************************/

//allocate memory for stack on heap and initialize
struct stack *stack_create(void)
{
    struct stack *stack_ptr = malloc(sizeof(struct stack));
    assert(stack_ptr != NULL);

    stack_ptr->top_of_stack = 0;
    return stack_ptr;
}

//free memory used by stack
void stack_destroy(struct stack *s)
{
    free(s);
}

//check if stack is empty
bool is_empty(struct stack *s)
{
    return s->top_of_stack == 0;
}

//check if stack is full
bool is_full(struct stack *s)
{
    return s->top_of_stack == STACK_SIZE;
}

//push an item onto the stack if not full
void stack_push(struct stack *s, stack_item datum)
{
    assert(!is_full(s));
    s->data[s->top_of_stack++] = datum;
}

//pop an item off the stack if not empty
stack_item stack_pop(struct stack *s)
{
    assert(!is_empty(s));
    return s->data[--s->top_of_stack];
}

//retrieve but do not remove the top item from the stack
stack_item stack_peek(struct stack *s)
{
    assert(!is_empty(s));
    return s->data[s->top_of_stack - 1];
}

//clear the stack by resetting index, but don't delete the existing data
void stack_clear(struct stack *s)
{
    s->top_of_stack = 0;
}

//print contents of stack to stdin
void stack_print(struct stack *s)
{
    for(int i = 0; i < s->top_of_stack; ++i)
    {
        printf(STACK_PRINT_FMT, s->data[i]);
    }
    puts("");
}
