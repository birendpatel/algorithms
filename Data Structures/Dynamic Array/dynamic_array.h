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
#define INIT_CAPACITY 4
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
* returns: pointer to array_item
*******************************************************************************/
darray darray_create(void);

/*******************************************************************************
* public function: darray_destroy
* purpose: destructor
* @ d : pointer to first array_item in a dynamic array
*******************************************************************************/
void darray_destroy(darray d);

/*******************************************************************************
* public function: vector_append
* purpose: append element to end of array
* @ d : pointer to array item
* @ element : array item to append
*******************************************************************************/
void darray_append(darray d, array_item element);

/*******************************************************************************
* public function: vector_len
* purpose: get total number of elements currently in vector
* @ d : pointer to array item
* returns: total number of elements currently in vector
*******************************************************************************/
int darray_len(darray d);

/*******************************************************************************
* public function: vector_show
* purpose: print vector contents to stdout
* @ d : pointer to array item
*******************************************************************************/
void darray_show(darray d);

#endif
