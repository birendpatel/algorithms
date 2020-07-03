/*
* Author: Biren Patel
* Description: Implementation for dynamic array abstract data type
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "dynamic_array.h"

/*******************************************************************************
* macro: hsize
* purpose: size of struct darray_header, typically 16 bytes
*******************************************************************************/

#define HSIZE sizeof(struct darray_header)

/*******************************************************************************
* macro: darray_header_var
* purpose: get a pointer to darray_header given a pointer to the data member
* note: offset is typically 16 bytes
*******************************************************************************/

#define DARRAY_HEADER_VAR(d)                                                   \
        ((struct darray_header *)                                              \
        (((char*) (d)) - offsetof(struct darray_header, data)))

/*******************************************************************************
* public functions
*******************************************************************************/

darray darray_create(size_t init_capacity, void (*destroy)(void *ptr))
{
    struct darray_header *dh;
    
    //check conditions
    assert(init_capacity > 0 && "init_capacity is not positive int");

    //allocate memory for header + flexible array member
    dh = malloc(HSIZE + sizeof(array_item) * init_capacity);
    if (dh == NULL) return NULL;
    
    //define remaining header metadata
    dh->destroy = destroy;
    dh->capacity = init_capacity;
    dh->count = 0;

    //client receives the array but everything else remains hidden
    return dh->data;
}

/******************************************************************************/

void darray_destroy(darray d)
{
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    (*dh->destroy)(dh);
}

/******************************************************************************/

int darray_count(darray d)
{
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    return dh->count;
}


/******************************************************************************/

int darray_append(darray *d, array_item element)
{
    struct darray_header *dh = DARRAY_HEADER_VAR(*d);
    
    //no space left in the allocated block to push an element.
    if (dh->count == dh->capacity)
    {
        if (dh->count == UINT32_MAX)
        {
            return 1;
        }
        else
        {
            //determine new array capacity as a function of current capacity
            uint32_t new_capacity = INCREASE_CAPACITY(dh->capacity);
            
            //override growth with a ceiling if new capacity exceeds uint32
            if (new_capacity > UINT32_MAX) new_capacity = UINT32_MAX;

            assert(new_capacity > dh->count && "capacity fx not monotonic");

            //determine total bytes needed
            size_t new_size = HSIZE + sizeof(array_item) * new_capacity;

            //reallocate memory and redirect dh pointer
            struct darray_header *tmp = realloc(dh, new_size);
            
            if (tmp == NULL) return 2;
            else dh = tmp;
            
            //realloc success so update dh->capacity
            dh->capacity = new_capacity;
            
            //realloc may have moved dh to new section of memory.
            //if so, update dereferenced input so client has correct address
            *d = dh->data;
        }
    }

    //space available in allocated block, go ahead and push element    
    dh->data[dh->count++] = element;
    
    assert(dh->count <= dh->capacity && "count exceeds maximum capacity");
    
    return 0;
}

/******************************************************************************/

bool darray_pop(darray d, array_item *popped_item)
{
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    if (dh->count == 0)
    {
        return false;
    }
    else
    {        
        //decrement count for synthetic pop but only return item if requested
        --dh->count;
        
        if (popped_item != NULL) *popped_item = dh->data[dh->count];
        
        assert(dh->count >= 0 && "array count is negative");
        
        return true;
    }
}

/******************************************************************************/

bool darray_popleft(darray d, array_item *popped_item)
{
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    if (dh->count == 0)
    {
        return false;
    }
    else
    {
        //copy first item to client storage if requested
        if (popped_item != NULL) *popped_item = dh->data[0];

        //move all the elements in the data array back by one index
        if (--dh->count != 0)
        {
            memmove(dh->data, dh->data + 1, dh->count * sizeof(array_item));
        }

        return true;
    }
}

/******************************************************************************/

bool darray_peek(darray d, array_item *peeked_item)
{
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    if (dh->count == 0 || peeked_item == NULL)
    {
        return false;
    }
    else
    {   
        *peeked_item = dh->data[dh->count - 1];
        
        assert(dh->count >= 0 && "array count is negative");
        
        return true;
    }
}
