/*
* Author: Biren Patel
* Description: Public API for dynamic array abstract data type
*/

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

/*******************************************************************************
* user-modifiable parameters
* @ array_item : data type of item stored in array
* @ increase_capacity : new array capacity as function of previous capacity n
* @ fmt_string : format string used to print array contents
* @ dyanmic_array_debug : set to 1 for verbose debugging output to stdout
*******************************************************************************/
typedef int array_item;
#define INCREASE_CAPACITY(n) (2 * n)
#define INIT_CAPACITY 1
#define FMT_STRING "%d "
#define DYNAMIC_ARRAY_DEBUG 0

/*******************************************************************************
* typedef: darray
* purpose: client must declare var of type darray to access API functionality
* note: for the client, this is just a pointer to the first array element
*******************************************************************************/
typedef array_item *darray;

/*******************************************************************************
* public function: darray_create
* purpose: constructor
* returns: darray
*******************************************************************************/
darray darray_create(void);

/*******************************************************************************
* public function: darray_destroy
* purpose: destructor
* @ d : darray
*******************************************************************************/
void darray_destroy(darray d);

/*******************************************************************************
* public function: darray_len
* purpose: get total number of elements currently in vector
* @ d : darray
* returns: total number of elements currently in vector
*******************************************************************************/
int darray_len(darray d);

/*******************************************************************************
* public function: darray_append
* purpose: push element onto end of array
* @ d : pointer to darray
* @ element : array item to append
*******************************************************************************/
void darray_push(darray *d, array_item element);

/*******************************************************************************
* public function: darray_pop
* purpose: pop element off end of array
* @ d : darray
* returns: pointer to array item (darray but not necessarily the first element)
*******************************************************************************/
array_item *darray_pop(darray d);

/*******************************************************************************
* public function: darray_peek
* purpose: examine but do not pop the elment at the end of the array
* @ d : darray
* returns: pointer to array item (darray but not necessarily the first element)
*******************************************************************************/
array_item *darray_peek(darray d);

/*******************************************************************************
* public function: darray_show
* purpose: print vector contents to stdout
* @ d : darray
*******************************************************************************/
void darray_show(darray d);

#endif
