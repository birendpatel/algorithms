/*
* author: Biren Patel
* description: fixed-length stack data structure
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "array_stack.h"

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

    if (stack_ptr == NULL)
    {
        exit(EXIT_FAILURE);
    }

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
int stack_push(struct stack *s, stack_item datum)
{
    if (!is_full(s))
    {
        s->data[s->top_of_stack++] = datum;
        assert(*stack_peek(s) == datum); //check push is good
        assert(s->top_of_stack <= STACK_SIZE); //check no stack index issues

        return 1;
    }
    else
    {
        return 0;
    }
}

//pop an item off the stack if not empty
stack_item *stack_pop(struct stack *s)
{
    if (!is_empty(s))
    {
        stack_item *top = s->data + (--s->top_of_stack);
        assert(s->top_of_stack >= 0); //check no stack index issues

        return top;
    }
    else
    {
        return NULL;
    }
}

//retrieve but do not remove the top item from the stack
stack_item *stack_peek(struct stack *s)
{
    if (!is_empty(s))
    {
        stack_item *top = s->data + (s->top_of_stack - 1);
        assert(s->top_of_stack >= 0); //check no stack index issues

        return top;
    }
    else
    {
        return NULL;
    }
}

//clear the stack by resetting index, but don't delete the existing data
int stack_clear(struct stack *s)
{
    int total_removed = s->top_of_stack;

    if (total_removed != 0)
    {
        s->top_of_stack = 0;
        assert(stack_peek(s) == NULL); //check clearance is good
    }

    return total_removed;
}

//print contents of stack to stdin
void stack_print(struct stack *s)
{
    int top = s->top_of_stack;

    for(int i = 0; i < top; ++i)
    {
        printf(STACK_PRINT_FMT, s->data[i]);
    }
    puts("");

    assert(s->top_of_stack == top); //check invariant
}
