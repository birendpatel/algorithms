/*
* author: Biren Patel
* description: interface for fixed-length stack data structure.
*/

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

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
*******************************************************************************/
void stack_push(struct stack *s, int datum);

/*******************************************************************************
* function: stack_pop
* purpose: operation to pop an item last-in-first-out
* @ s : pointer to struct stack
* returns: the popped item
*******************************************************************************/
int stack_pop(struct stack *s);

/*******************************************************************************
* function: stack_clear
* purpose: operation to clear the stack, resets index but does not remove items
* @ s : pointer to struct stack
*******************************************************************************/
void stack_clear(struct stack *s);

/*******************************************************************************
* function: stack_print
* purpose: operation, print contents of stack to stdin
* @ s : pointer to struct stack
*******************************************************************************/
void stack_print(struct stack *s);

#endif
