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
*******************************************************************************/
typedef void* array_item;
#define INCREASE_CAPACITY(n) (2 * n)

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
* @ cache : an empty void pointer pocket for any strange end-user needs
* @ queue : pointer to popleft array items to allow implementation details
* @ destroy : pointer to function, used during destructor call to free memory
* @ capacity : maximum size of array
* @ count : number of elements held in array
* @ data : contents of the array
*
* note: For most standard array_items, those that appear in powers of 2, this 
*       structure should remain fairly space-efficent because the header
*       will pack to exactly 32 bytes and the malloc for the FLA will not
*       overcommit memory. There won't be a problem if you use a data type which
*       overrides the packing, but you will possibly have extra padding within
*       the header and after the data array. 
*
* diagram:
*
*   #---------#---------#-----------#------------#---------#-----------------#
*   #  cache  #  queue  #  destroy  #  capacity  #  count  #  data --------> #
*   #---------#---------#-----------#------------#---------#-----------------#
*
*   \_____________________________________________________/ \________________/
*                        hidden metadata                       exposed array
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
*       pointer itself and the cache/queue members.
*
*       example:
*
*       void darray_free(void *header)
*       {
*           struct darray_header *dh = (struct darray_header *) header;
*           
*           for(size_t i = 0; i < dh->count; ++i)
*           {
*               free(dh->data[i]);
*           }
*
*           free(dh->cache);
*           free(dh->queue);           
*           free(dh);
*       }
*
*******************************************************************************/

struct darray_header
{
    void *cache;
    array_item *queue;
    void (*destroy)(void *ptr);
    uint32_t capacity;
    uint32_t count;
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
* public function: darray_count
* purpose: count total number of elements currently in array
* @ d : darray, the same darray returned by constructor
* returns: total number of elements currently in array
*******************************************************************************/
int darray_count(darray d);

/*******************************************************************************
* public function: darray_append
* purpose: push element onto end of array
* @ d : pointer to darray, the same darray returned by constructor
* @ element : array item to append
* returns: error codes. 0 if successful push. 1 if failure because the array
           currently contains 4,294,967,295 elements. This cannot be exceeded.
           2 if memory reallocation fails during realloc call. 
*******************************************************************************/
int darray_append(darray *d, array_item element);

/*******************************************************************************
* public function: darray_pop
* purpose: pop element off end of array
* @ d : darray, the same darray returned by constructor
* returns: pointer to popped item
*******************************************************************************/
array_item darray_pop(darray d);

/*******************************************************************************
* public function: darray_popleft
* purpose: pop element off front of array
* @ d : darray, the same darray returned by constructor
* returns: pointer to popped item
*******************************************************************************/
array_item darray_popleft(darray d);

/*******************************************************************************
* public function: darray_peek
* purpose: examine but do not pop the element at the end of the array
* @ d : darray, the same darray returned by constructor
* returns: pointer to peeked item
*******************************************************************************/
array_item darray_peek(darray d);

#endif
