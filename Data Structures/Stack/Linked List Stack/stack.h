/*
* Author: Biren Patel
* Description: Generic stack ADT API via void pointers
*/

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
* typedef: stack
* purpose: client must declare variable of stack type to access API
*******************************************************************************/
typedef struct dll *stack;

/*******************************************************************************
* public function: stack_create
* purpose: constructor
* @ destroy : pointer to function for void * item destruction, else NULL
* returns: stack
*******************************************************************************/
stack stack_create(void (*destroy)(void *data));

/*******************************************************************************
* public function: stack_destroy
* purpose: destructor
* @ s : stack
*******************************************************************************/
void stack_destroy(stack s);

/*******************************************************************************
* public function: stack_push
* purpose: push an item onto the stack
* @ s : stack
* @ item : void * item to push
* returns: true if successful push, false otherwise due to malloc failure
*******************************************************************************/
bool stack_push(stack s, void *item);

/*******************************************************************************
* public function: stack_pop
* purpose: pop an item off the stack
* @ s : stack
* returns: void * item at the top of the stack
*******************************************************************************/
void *stack_pop(stack s);

/*******************************************************************************
* stack_peek
* purpose: peek at the top item of the stack but do not remove it
* @ s : stack
* returns: void * item at the top of the stack
*******************************************************************************/
void *stack_peek(stack s);

/*******************************************************************************
* public function: stack_size
* purpose: determine the number of items on the stack
* @ s : stack
* returns: total number of items
*******************************************************************************/
uint32_t stack_size(stack s);

#endif