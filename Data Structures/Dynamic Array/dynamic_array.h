/*
* Author: Biren Patel
* Description: Public API for dynamic array abstract data type with stack and
* queue functionality. Stack operations are fast, queue operations are not. See
* the comments in the implementation file for more details on memory usage.
*/

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

/*******************************************************************************
* user-modifiable parameters
* @ array_item : data type of item stored in array
* @ increase_capacity : new array capacity as function of previous capacity n
* @ fmt_string : format string to print array contents, must match array_item
* @ dynamic_array_debug : set to 1 for verbose debugging output to stdout
*******************************************************************************/
typedef int array_item;
#define INCREASE_CAPACITY(n) (2 * n)
#define FMT_STRING "%d "
#define DYNAMIC_ARRAY_DEBUG 0

/*******************************************************************************
* typedef: darray
* purpose: client must declare variable of type darray
*******************************************************************************/
typedef array_item *darray;

/*******************************************************************************
* public function: darray_create
* purpose: constructor
* @ init_capacity : initial capacity of array
* @ destroy : pointer to function, used during destructor call to free memory
* returns: darray, pointer to first uninitialized element of an array. NULL if
*          malloc failed.
*******************************************************************************/
darray darray_create(size_t init_capacity, void (*destroy)(void *data));

/*******************************************************************************
* public function: darray_destroy
* purpose: destructor
* @ d : darray, the same darray returned by constructor
*******************************************************************************/
void darray_destroy(darray d);

/*******************************************************************************
* public function: darray_len
* purpose: get length of array
* @ d : darray, the same darray returned by constructor
* returns: total number of elements currently in array
*******************************************************************************/
int darray_len(darray d);

/*******************************************************************************
* public function: darray_append
* purpose: push element onto end of array
* @ d : pointer to darray, the same darray returned by constructor
* @ element : array item to append
* returns: error codes. 0 if successful push. 1 if failure because the array
           currently contains 4,294,967,295 elements. This cannot be exceeded.
           2 if memory reallocation fails during realloc call. 
*******************************************************************************/
int darray_push(darray *d, array_item element);

/*******************************************************************************
* public function: darray_pop
* purpose: pop element off end of array
* @ d : darray, the same darray returned by constructor
* @ popped_item : pointer to storage location for popped item
* returns: true if pop is successful, false otherwise
*******************************************************************************/
bool darray_pop(darray d, array_item *popped_item);

/*******************************************************************************
* public function: darray_popleft
* purpose: pop element off front of array
* @ d : darray, the same darray returned by constructor
* @ popped_item : pointer to storage location for popped item
* returns: true if pop is successful, false otherwise
*******************************************************************************/
bool darray_popleft(darray d, array_item *popped_item);

/*******************************************************************************
* public function: darray_peek
* purpose: examine but do not pop the element at the end of the array
* @ d : darray, the same darray returned by constructor
* @ peeked_item : pointer to storage location for peeked-at item
* returns: true if peek is successful, false otherwise
*******************************************************************************/
bool darray_peek(darray d, array_item *peeked_item);

/*******************************************************************************
* public function: darray_show
* purpose: print array contents to stdout
* @ d : darray, the same darray returned by constructor
*******************************************************************************/
void darray_show(darray d);

#endif
