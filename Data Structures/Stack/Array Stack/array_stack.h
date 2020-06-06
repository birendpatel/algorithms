/*
* author: Biren Patel
* description: API for fixed-length stack data structure.
*/

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

/*******************************************************************************
* purpose: pre-compilation modifiable parameters
* @ typedef : change data type in stack without resorting to a generic ADT.
* @ STACK_PRINT_FMT : adjust the format string for the stack_print function.
* @ STACK_SIZE : maximum number of elements held in stack
*******************************************************************************/
typedef int stack_item;
#define STACK_PRINT_FMT "%d "
#define STACK_SIZE 10

/*******************************************************************************
* structure: struct stack
* purpose: client must declare pointer to struct stack
*******************************************************************************/
struct stack;

/*******************************************************************************
* function: stack_create
* purpose: constructor to initialize a stack on heap
* returns: pointer to struct stack
*******************************************************************************/
struct stack *stack_create(void);

/*******************************************************************************
* function: stack_destroy
* purpose: destructor to free memory used by stack
* @ s : pointer to struct stack
*******************************************************************************/
void stack_destroy(struct stack *s);

/*******************************************************************************
* function: is_full
* purpose: operation to check if stack is full
* @ s : pointer to struct stack
* returns: true if full
*******************************************************************************/
bool is_full(struct stack *s);

/*******************************************************************************
* function: is_empty
* purpose: operation to check if stack is empty
* @ s : pointer to struct stack
* returns: true if empty
*******************************************************************************/
bool is_empty(struct stack *s);

/*******************************************************************************
* function: stack_push
* purpose: operation to push item onto stack
* @ s : pointer to struct stack
* @ datum : item to push
* returns: 1 for successful push, 0 for failed push.
*******************************************************************************/
int stack_push(struct stack *s, stack_item datum);

/*******************************************************************************
* function: stack_pop
* purpose: operation to pop an item last-in-first-out
* @ s : pointer to struct stack
* returns: pointer to the popped item, or null pointer if stack is empty.
*******************************************************************************/
stack_item *stack_pop(struct stack *s);

/*******************************************************************************
* function: stack_peek
* purpose: retrieve but do not remove the top item from the stack
* @ s : pointer to struct stack
* returns: pointer to the top item in stack, or null pointer if stack is empty.
*******************************************************************************/
stack_item *stack_peek(struct stack *s);

/*******************************************************************************
* function: stack_clear
* purpose: operation to clear the stack, resets index but does not remove items
* @ s : pointer to struct stack
* returns: number of items cleared out of the stack.
*******************************************************************************/
int stack_clear(struct stack *s);

/*******************************************************************************
* function: stack_print
* purpose: operation, print contents of stack to stdin
* @ s : pointer to struct stack
*******************************************************************************/
void stack_print(struct stack *s);

#endif
