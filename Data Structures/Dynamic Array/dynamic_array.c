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
#include "dynamic_array.h"

/*******************************************************************************
* macro: darray_trace
* purpose: debugging output if DYNAMIC_ARRAY_DEBUG == 1 from dynamic_array.h
*******************************************************************************/

#define DARRAY_TRACE(fmt, ...)                                                 \
        do                                                                     \
        {                                                                      \
            if (DYNAMIC_ARRAY_DEBUG)                                           \
            printf("\n--> %s (%d): " fmt, __func__, __LINE__, __VA_ARGS__);    \
        } while(0)

/*******************************************************************************
* macro: verify_pointer
* purpose: exit if a pointer is null
* @ test : one word name of the test being performed
* @ pointer : pointer returned by some function
*******************************************************************************/

#define VERIFY_POINTER(test, pointer)                                          \
        if (pointer == NULL)                                                   \
        {                                                                      \
            fprintf(stderr, #test " fail: %s in %s\n", __func__, __FILE__);    \
            exit(EXIT_FAILURE);                                                \
        }

/*******************************************************************************
* macro: darray_header_var
* purpose: given pointer to struct member, get pointer to containing structure
*******************************************************************************/

#define DARRAY_HEADER_VAR(d) ((struct darray_header *) (((int8_t*) (d)) - 16))

/*******************************************************************************
* structure: darray_header
* purpose: hidden 16 byte structure contains array metadata
* @ stats cache : pointer to double for computed statistics cache
* @ capacity : current maximum size of array
* @ length : current number of elements held in array
* @ data : stores elements contained in array

     #---------------#------------#------------#-------------------#
     #  stats cache  #  capacity  #   length   #  data ----------> #
     #---------------#------------#------------#-------------------#

     \_______________________________________/  \_________________/
                  hidden metadata                  exposed array

*******************************************************************************/

struct __attribute__ ((packed)) darray_header
{
    double *stats_cache;
    uint32_t capacity;
    uint32_t length;
    array_item data[];
};

/*******************************************************************************
* public functions
*******************************************************************************/

darray darray_create(void)
{
    DARRAY_TRACE("creating dynamic array%c\n", ' ');
    assert(INIT_CAPACITY > 0);

    //client will receive dh->data but the rest of the struct will be hidden
    struct darray_header *dh;
    assert(sizeof(struct darray_header) == 16);

    //allocate block of memory for struct + FLA
    dh = malloc(16 + sizeof(array_item) * INIT_CAPACITY);
    VERIFY_POINTER(malloc, dh);

    //define header metadata
    dh->stats_cache = NULL;
    dh->capacity = INIT_CAPACITY;
    dh->length = 0;

    //expose array to client but keep the header hidden
    DARRAY_TRACE("dynamic array created, %d element capacity\n", dh->capacity);
    return dh->data;
}

/******************************************************************************/

void darray_destroy(darray d)
{
    assert(d != NULL);

    //define pointer to containing structure via the input pointer
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    //check that we have the stucture
    assert(dh->data[0] == *d);

    DARRAY_TRACE("destroying dynamic array "
                 "with members\n\tcache: %p\n\tcapacity: %d\n\tlength: %d\n",
                 (void*) dh->stats_cache, dh->capacity, dh->length);

    //pointers are checking out okay, lets free the memory block
    free(dh);
    return;
}

/******************************************************************************/

int darray_len(darray d)
{
    assert(d != NULL);

    //define pointer to containing structure via the input pointer
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    //check that we have the stucture and check length in bounds
    assert(dh->data[0] == *d);
    assert(dh->length >= 0 && dh->length <= dh->capacity);

    DARRAY_TRACE("returning data length to client: %d elements", dh->length);
    return dh->length;
}


/******************************************************************************/

void darray_push(darray *d, array_item element)
{
    assert(d != NULL && *d != NULL);

    //define pointer to structure via the dereferenced input pointer
    struct darray_header *dh = DARRAY_HEADER_VAR(*d);

    //check that we have the stucture and check length in bounds
    assert(dh->data[0] == **d);
    assert(dh->length >= 0 && dh->length <= dh->capacity);

    DARRAY_TRACE("pushing element to dynamic array%c\n", ' ');

    //no space left in the allocated block to push the element
    if (dh->length == dh->capacity)
    {
        //determine new array capacity as a function of current capacity
        dh->capacity = INCREASE_CAPACITY(dh->capacity);

        assert(dh->capacity > dh->length);
        DARRAY_TRACE("increased capacity to %d\n", dh->capacity);

        //determine total bytes needed for header and data elements
        size_t new_size = 16 + sizeof(array_item) * dh->capacity;
        DARRAY_TRACE("new darray memory block of %d bytes\n", (int) new_size);

        //reallocate memory and redirect dh pointer
        struct darray_header *tmp;
        tmp = realloc(dh, new_size);
        VERIFY_POINTER(realloc, tmp);
        dh = tmp;
    }

    //space available in allocated block, go ahead and push element
    dh->data[dh->length++] = element;

    //if realloc called, dh may have moved to a new section of memory.
    //if so, need to dereference the input pointer and update address for client
    *d = dh->data;

    DARRAY_TRACE("length now at %d\n", dh->length);
    assert(dh->length >= 0 && dh->length <= dh->capacity);
    assert(dh->capacity >= dh->length); //only equal if client changed growth fx
}

/******************************************************************************/

array_item *darray_pop(darray d)
{
    assert(d != NULL);
    DARRAY_TRACE("pop requested%c\n", ' ');

    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d);
    assert(dh->length >= 0 && dh->length <= dh->capacity);

    if (dh->length == 0)
    {
        DARRAY_TRACE("nothing to pop%c\n", ' ');

        return NULL;
    }
    else
    {
        array_item *top = dh->data + (--dh->length);
        assert(dh->length >= 0);
        assert(top != NULL);

        DARRAY_TRACE("pop successful%c\n", ' ');

        return top;
    }
}

/******************************************************************************/

array_item *darray_peek(darray d)
{
    assert(d != NULL);
    DARRAY_TRACE("peek requested%c\n", ' ');

    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    assert(dh->data[0] == *d);
    assert(dh->length >= 0 && dh->length <= dh->capacity);

    if (dh->length == 0)
    {
        DARRAY_TRACE("nothing to peek at%c\n", ' ');

        return NULL;
    }
    else
    {
        array_item *top = dh->data + (dh->length - 1);
        assert(dh->length >= 0);
        assert(top != NULL);

        DARRAY_TRACE("peek successful%c\n", ' ');

        return top;
    }
}

/******************************************************************************/

void darray_show(darray d)
{
    assert(d != NULL);

    //define pointer to containing structure via the input pointer
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    //check that we have the structure
    assert(dh->data[0] == *d);

    //loop and print
    for(size_t i = 0; i < dh->length; ++i)
    {
        printf(FMT_STRING, d[i]);
    }

    puts("");
}
