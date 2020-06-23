/*
* Author: Biren Patel
* Description: Implementation for dynamic array abstract data type
* Note: uses non-portable GCC features, assumes x64, best with std data types.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
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
* macro: verify_pointer
* purpose: exit with failure if a pointer is null
* @ test : name of the test being performed
* @ pointer : the pointer you wish to test
*******************************************************************************/

#define verify_pointer(test, pointer)                                          \
        if (pointer == NULL)                                                   \
        {                                                                      \
            fprintf(stderr, #test " fail: %s in %s\n", __func__, __FILE__);    \
            exit(EXIT_FAILURE);                                                \
        }

/*******************************************************************************
* macro: darray_header_var
* purpose: get a pointer to darray_header given a pointer to the data member
*******************************************************************************/

#define DARRAY_HEADER_VAR(d) ((struct darray_header *) (((char*) (d)) - 16))

/*******************************************************************************
* structure: darray_header
* purpose: dynamic array metadata
* @ queue cache : pointer to item removed in previous popleft operation
* @ capacity : maximum size of array
* @ length : number of elements held in array
* @ data : contents of the array

     #---------------#------------#------------#-------------------#
     #  queue cache  #  capacity  #   length   #  data ----------> #
     #---------------#------------#------------#-------------------#

     \_______________________________________/  \_________________/
             hidden metadata (16 bytes)            exposed array

*******************************************************************************/

struct __attribute__ ((packed)) darray_header
{
    array_item *queue_cache;
    uint32_t capacity;
    uint32_t length;
    array_item data[];
};

/*******************************************************************************
* public functions
*******************************************************************************/

darray darray_create(void)
{
    struct darray_header *dh;
    
    //check conditions
    assert(INIT_CAPACITY > 0 && "init_capacity macro is not positive int");
    assert(sizeof(struct darray_header) == 16 && "header is not 16 bytes");

    //allocate memory for 16 byte header + flexible array member
    dh = malloc(16 + sizeof(array_item) * INIT_CAPACITY);
    verify_pointer(malloc, dh);

    //allocate memory for queue cache but nothing to store just yet
    dh->queue_cache = malloc(sizeof(array_item));
    verify_pointer(malloc, dh->queue_cache);
    
    //define remaining header metadata
    dh->capacity = INIT_CAPACITY;
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

    //free the queue cache before the header
    free(dh->queue_cache);
    free(dh);
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

void darray_push(darray *d, array_item element)
{
    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(*d);

    assert(dh->data[0] == **d && "input pointer is not constructor pointer");

    //no space left in the allocated block to push the element
    if (dh->length == dh->capacity)
    {
        //determine new array capacity as a function of current capacity
        dh->capacity = INCREASE_CAPACITY(dh->capacity);

        assert(dh->capacity > dh->length && "capacity function not monotonic");
        darray_trace("increased capacity to %d\n", dh->capacity);

        //determine total bytes needed
        size_t new_size = 16 + sizeof(array_item) * dh->capacity;
        darray_trace("new darray memory block of %d bytes\n", (int) new_size);

        //reallocate memory and redirect dh pointer
        struct darray_header *tmp = realloc(dh, new_size);
        verify_pointer(realloc, tmp);
        dh = tmp;
        
        //realloc may have moved dh to new section of memory.
        //if so, update dereferenced input so client has the correct location
        *d = dh->data;
    }

    //space available in allocated block, go ahead and push element
    darray_trace("pushing element to dynamic array%c\n", ' ');
    
    dh->data[dh->length++] = element;
    
    assert(dh->length <= dh->capacity && "length exceeds maximum capacity");
}

/******************************************************************************/

array_item *darray_pop(darray d)
{
    darray_trace("pop requested%c\n", ' ');

    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    if (dh->length == 0)
    {
        darray_trace("nothing to pop%c\n", ' ');
        
        return NULL;
    }
    else
    {
        array_item *top = dh->data + (--dh->length);
        
        assert(dh->length >= 0 && "array length is negative");
        assert(top != NULL && "popped item is null");
        darray_trace("pop successful%c\n", ' ');

        return top;
    }
}

/******************************************************************************/

array_item *darray_popleft(darray d)
{
    darray_trace("popleft requested%c\n", ' ');

    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    if (dh->length == 0)
    {
        //reset queue cache to NULL
        darray_trace("nothing to pop%c\n", ' ');
        
        dh->queue_cache = NULL;
        
        return NULL;
    }
    else
    {
        //copy first item in array to cache block
        *dh->queue_cache = dh->data[0];
        
        assert(*dh->queue_cache == dh->data[0] && "queue copy to cache failed");

        //move all the elements in the data array back by one index
        if (--dh->length != 0)
        {
            size_t n_bytes = dh->length * sizeof(array_item);

            array_item *start = memmove(dh->data, dh->data + 1, n_bytes);

            verify_pointer(memmove, start);
        }

        //cache has popped item and memory moved back, good to proceed
        darray_trace("popleft successful%c\n", ' ');
        return dh->queue_cache;
    }
}

/******************************************************************************/

array_item *darray_peek(darray d)
{
    darray_trace("peek requested%c\n", ' ');

    //define pointer to array header
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d && "input pointer is not constructor pointer");

    if (dh->length == 0)
    {
        darray_trace("nothing to peek at%c\n", ' ');

        return NULL;
    }
    else
    {
        array_item *top = dh->data + (dh->length - 1);
        
        assert(dh->length >= 0 && "array length is negative");
        assert(top != NULL && "top item is null");
        darray_trace("peek successful%c\n", ' ');

        return top;
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
