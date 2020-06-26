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
* macro: darray_trace
* purpose: debugging to stdout if macro DYNAMIC_ARRAY_DEBUG set to 1
*******************************************************************************/

#define darray_trace(fmt, ...)                                                 \
        do                                                                     \
        {                                                                      \
            if (DYNAMIC_ARRAY_DEBUG)                                           \
            {                                                                  \
                printf("\n>> %s (%d): " fmt, __func__, __LINE__, __VA_ARGS__); \
            }                                                                  \
        }                                                                      \
        while(0)


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
    dh->length = 0;

    //client receives the array but everything else remains hidden
    return dh->data;
}

/******************************************************************************/

void darray_destroy(darray d)
{
    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    (*dh->destroy)(dh);
}

/******************************************************************************/

int darray_len(darray d)
{
    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    return dh->length;
}


/******************************************************************************/

int darray_push(darray *d, array_item element)
{
    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(*d);

    assert(dh->data[0] == **d && "input pointer is not constructor pointer");

    //no space left in the allocated block to push an element.
    if (dh->length == dh->capacity)
    {
        if (dh->length == UINT32_MAX)
        {
            darray_trace("capacity cannot increase, push impossible%c\n", ' ');
            return 1;
        }
        else
        {
            //determine new array capacity as a function of current capacity
            dh->capacity = INCREASE_CAPACITY(dh->capacity);
            
            //override growth with a ceiling if new capacity exceeds uint32
            if (dh->capacity > UINT32_MAX) dh->capacity = UINT32_MAX;

            assert(dh->capacity > dh->length && "capacity fx not monotonic");
            darray_trace("increased capacity to %d\n", dh->capacity);

            //determine total bytes needed
            size_t new_size = HSIZE + sizeof(array_item) * dh->capacity;
            darray_trace("new memory block of %d bytes\n", (int) new_size);

            //reallocate memory and redirect dh pointer
            struct darray_header *tmp = realloc(dh, new_size);
            
            if (tmp == NULL) return 2;
            else dh = tmp;
            
            //realloc may have moved dh to new section of memory.
            //if so, update dereferenced input so client has correct address
            *d = dh->data;
        }
    }

    //space available in allocated block, go ahead and push element
    darray_trace("pushing element to dynamic array%c\n", ' ');
    
    dh->data[dh->length++] = element;
    
    assert(dh->length <= dh->capacity && "length exceeds maximum capacity");
    
    return 0;
}

/******************************************************************************/

bool darray_pop(darray d, array_item *popped_item)
{
    darray_trace("pop requested%c\n", ' ');

    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    if (dh->length == 0)
    {
        darray_trace("nothing to pop%c\n", ' ');
        
        return false;
    }
    else
    {        
        //decrement length for synthetic pop but only return item if requested
        --dh->length;
        
        if (popped_item != NULL) *popped_item = dh->data[dh->length];
        
        assert(dh->length >= 0 && "array length is negative");
        darray_trace("pop successful%c\n", ' ');
        
        return true;
    }
}

/******************************************************************************/

bool darray_popleft(darray d, array_item *popped_item)
{
    darray_trace("popleft requested%c\n", ' ');

    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    if (dh->length == 0)
    {
        darray_trace("nothing to popleft%c\n", ' ');
        
        return false;
    }
    else
    {
        //copy first item to client storage if requested
        if (popped_item != NULL) *popped_item = dh->data[0];

        //move all the elements in the data array back by one index
        if (--dh->length != 0)
        {
            memmove(dh->data, dh->data + 1, dh->length * sizeof(array_item));
        }

        darray_trace("popleft successful%c\n", ' ');
        
        return true;
    }
}

/******************************************************************************/

bool darray_peek(darray d, array_item *peeked_item)
{
    darray_trace("peek requested%c\n", ' ');

    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    if (dh->length == 0 || peeked_item == NULL)
    {
        darray_trace("nothing to peek at or peek used improperly%c\n", ' ');

        return false;
    }
    else
    {   
        *peeked_item = dh->data[dh->length - 1];
        
        assert(dh->length >= 0 && "array length is negative");
        darray_trace("peek successful%c\n", ' ');
        
        return true;
    }
}

/******************************************************************************/

void darray_show(darray d)
{
    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    //loop and print
    for(size_t i = 0; i < dh->length; ++i)
    {
        printf(FMT_STRING, d[i]);
    }

    puts("");
}
