/*
* Author: Biren Patel
* Description: Public API for dynamic array abstract data type with stack and
* queue functionality. Stack operations are fast, queue operations are not. 
*/

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
* user-modifiable parameters
* @ array_item : data type of item stored in array, set void* for a generic ADT
* @ increase_capacity : new array capacity as function of previous capacity n
* @ fmt_string : printf format string to print array, must match array_item
* @ dynamic_array_debug : set to 1 for verbose debugging output to stdout
*******************************************************************************/
typedef void* array_item;
#define INCREASE_CAPACITY(n) (2 * n)
#define FMT_STRING "%s "
#define DYNAMIC_ARRAY_DEBUG 0

/*******************************************************************************
* typedef: darray
* purpose: the client must declare a variable of type darray to access the API
* note: a variable of type darray acts exactly like a normal C array. Anything
*       that you can do to a normal C array, you can do to a darray, including
*       indexing with [] and passing to functions which expect a normal C array.
*******************************************************************************/
typedef array_item *darray;

/*******************************************************************************
* structure: darray_header
* purpose: dynamic array metadata
* @ destroy : pointer to function, used during destructor call to free memory
* @ capacity : maximum size of array
* @ length : number of elements held in array
* @ data : contents of the array
*
* note: For most standard array_items, those that appear in powers of 2 up to 
*       16 bytes, this structure should remain fairly efficent as the header
*       will pack to exactly 16 bytes and the malloc for the FLA will not
*       overcommit memory. There won't be a problem if you use a data type which
*       overrides the packing, but you will possibly have extra padding within
*       the header and after the data array. 
*
* diagram:
*
*         #-----------#------------#------------#-------------------#
*         #  destroy  #  capacity  #   length   #  data ----------> #
*         #-----------#------------#------------#-------------------#
*
*         \____________________________________/ \__________________/
*                     hidden metadata                exposed array
*               
*
* note: The client does not need to interact with this structure or declare any
*       variables of this type unless they need to define some sort of custom
*       free() function to be used during the destructor call. The flexible
*       array member data[] contains all pushed elements.
*
*       With a custom destructor, all responsibility of cleaning up dynamically
*       allocated memory is pushed to the client. Therefore, once all elements 
*       in data[] are appropriately destroyed, do not forget to free the struct 
*       pointer itself.
*
*       example:
*
*       void darray_free(void *header)
*       {
*           struct darray_header *dh = (struct darray_header *) header;
*           
*           for(size_t i = 0; i < dh->length; ++i)
*           {
*               free(dh->data[i]);
*           }
*           
*           free(dh);
*       }
*
*******************************************************************************/

struct darray_header
{
    void (*destroy)(void *ptr);
    uint32_t capacity;
    uint32_t length;
    array_item data[];
};

/*******************************************************************************
* public function: darray_create
* purpose: constructor
* @ init_capacity : initial capacity of array
* @ destroy : pointer to function, used during destructor call to free memory.
*             if a custom implementation is not needed, just pass stdlib free().
* returns: darray, pointer to first uninitialized element of an array. NULL if
*          malloc failed.
*******************************************************************************/
darray darray_create(size_t init_capacity, void (*destroy)(void *ptr));

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
* @ popped_item : pointer to storage location for popped item or NULL to discard
* returns: true if pop is successful, false otherwise
* note: if items on the array are dynamically allocated, you must be careful to
*       not pass NULL as the argument to popped_item, as this may result in a
*       memory leak.
*******************************************************************************/
bool darray_pop(darray d, array_item *popped_item);

/*******************************************************************************
* public function: darray_popleft
* purpose: pop element off front of array
* @ d : darray, the same darray returned by constructor
* @ popped_item : pointer to storage location for popped item or NULL to discard
* returns: true if pop is successful, false otherwise
* note: if items on the array are dynamically allocated, you must be careful to
*       not pass NULL as the argument to popped_item, as this may result in a
*       memory leak.
*******************************************************************************/
bool darray_popleft(darray d, array_item *popped_item);

/*******************************************************************************
* public function: darray_peek
* purpose: examine but do not pop the element at the end of the array
* @ d : darray, the same darray returned by constructor
* @ peeked_item : pointer to storage location for peeked item
* returns: true if peek is successful, false otherwise
*******************************************************************************/
bool darray_peek(darray d, array_item *peeked_item);

#endif
