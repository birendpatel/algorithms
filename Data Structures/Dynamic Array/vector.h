/*
* Author: Biren Patel
* Description: Public API for dynamic array abstract data type
*/

#ifndef VECTOR_H
#define VECTOR_H

/*******************************************************************************
* user-modifiable parameters
* @ array_item : data type of item stored in array
* @ vector_start_capacity : capacity of array on initialization
* @ vector_grow : new vector capacity as function of previous capacity n
* @ fmt_string : format string used to print vector contents
* @ VECTOR_DEBUG : set to 1 for verbose debugging output to stdout
*******************************************************************************/
typedef int array_item;
#define VECTOR_START_CAPACITY 1
#define VECTOR_GROW(n) (2 * n)
#define FMT_STRING "%d "
#define VECTOR_DEBUG 1

/*******************************************************************************
* structure: vector
* purpose: client declares pointer to vector
*******************************************************************************/
struct vector;

/*******************************************************************************
* public function: vector_create
* purpose: constructor
* returns: pointer to struct vector
*******************************************************************************/
struct vector *vector_create(void);

/*******************************************************************************
* public function: vector_destroy
* purpose: destructor
* @ vec : pointer to struct vector
*******************************************************************************/
void vector_destroy(struct vector *vec);

/*******************************************************************************
* public function: vector_append
* purpose: append element to end of array
* @ vec : pointer to struct vector
* @ element : array item to append
* returns: number of elements in vector after append is complete.
*******************************************************************************/
int vector_append(struct vector *vec, array_item element);

/*******************************************************************************
* public function: vector_get
* purpose: get pointer to element at index
* @ vec : pointer to struct vector
* @ index : index of desired element
* returns: pointer to element, NULL if index to uninitialized or out of bounds
*******************************************************************************/
array_item *vector_get(struct vector *vec, int index);

/*******************************************************************************
* public function: vector_clear
* purpose: clear elements from a vector but keep gained attributes like capacity
* @ vec : pointer to struct vector
*******************************************************************************/
void vector_clear(struct vector *vec);

/*******************************************************************************
* public function: vector_show
* purpose: print vector contents to stdout
* @ vec : pointer to struct vector
*******************************************************************************/
void vector_show(struct vector *vec);

#endif
