/*
* Author: Biren Patel
* Description: Public API for dynamic array abstract data type
*/

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>

/*******************************************************************************
* user-modifiable parameters
* @ array_item : data type of item stored in array
* @ increase_capacity : new array capacity as function of previous capacity n
* @ fmt_string : format string to print array contents, must match array_item
* @ dyanmic_array_debug : set to 1 for verbose debugging output to stdout
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
* returns: darray, pointer to first uninitialized element of an array
*******************************************************************************/
darray darray_create(size_t init_capacity);

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
*******************************************************************************/
void darray_push(darray *d, array_item element);

/*******************************************************************************
* public function: darray_pop
* purpose: pop element off end of array
* @ d : darray, the same darray returned by constructor
* returns: pointer to array item, NULL if pop not possible
*******************************************************************************/
array_item *darray_pop(darray d);

/*******************************************************************************
* public function: darray_popleft
* purpose: pop element off front of array
* @ d : darray, the same darray returned by constructor
* returns: pointer to array_item, NULL if popleft not possible
*******************************************************************************/
array_item *darray_popleft(darray d);

/*******************************************************************************
* public function: darray_peek
* purpose: examine but do not pop the element at the end of the array
* @ d : darray, the same darray returned by constructor
* returns: pointer to array item, NULL if peek not possible
*******************************************************************************/
array_item *darray_peek(darray d);

/*******************************************************************************
* public function: darray_show
* purpose: print array contents to stdout
* @ d : darray, the same darray returned by constructor
*******************************************************************************/
void darray_show(darray d);

#endif
