/*
* author: Biren Patel
* description: fixed-length stack data structure
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

#define STACK_SIZE 10

/*******************************************************************************
* structure: struct stack
* @ data : contains items pushed by client
* @ top_of_stack : provides an index for data array
*******************************************************************************/
struct stack
{
    int data[STACK_SIZE];
    int top_of_stack;
};

/*******************************************************************************
* private functions
*******************************************************************************/

//print debugging information to stdin and exit
static void terminate(const char *fx_name, const char *f_name, int ln)
{
    printf("\nFAILURE: %s : %d - %s operation failed\n", f_name, ln, fx_name);
    exit(EXIT_FAILURE);
}

/*******************************************************************************
* public functions
*******************************************************************************/

//allocate memory for stack on heap and initialize
struct stack *stack_create(void)
{
    struct stack *stack_ptr = malloc(sizeof(struct stack));

    if (stack_ptr == NULL)
    {
        terminate(__func__, __FILE__, __LINE__);
    }
    else
    {
        stack_ptr->top_of_stack = 0;
        return stack_ptr;
    }
}

//free memory used by stack
void stack_destroy(struct stack *s)
{
    free(s);
}

//check if stack is empty
bool is_empty(struct stack *s)
{
    if (s->top_of_stack == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//check if stack is full
bool is_full(struct stack *s)
{
    if (s->top_of_stack == STACK_SIZE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//push an item onto the stack if not full
void stack_push(struct stack *s, int datum)
{
    if (is_full(s))
    {
        terminate(__func__, __FILE__, __LINE__);
    }
    else
    {
        s->data[s->top_of_stack++] = datum;
    }
}

//pop an item off the stack if not empty
int stack_pop(struct stack *s)
{
    if (is_empty(s))
    {
        terminate(__func__, __FILE__, __LINE__);
    }
    else
    {
        return s->data[--s->top_of_stack];
    }
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
        printf("%3d ", s->data[i]);
    }
    puts("");
}
