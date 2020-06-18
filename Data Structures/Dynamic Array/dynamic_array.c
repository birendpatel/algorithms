/*
* Author: Biren Patel
* Description: Implementation for dynamic array abstract data type
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
#define DARRAY_HEADER_VAR(d)                                                   \
        struct darray_header *dh = (struct darray_header *)                    \
        ((int8_t*) d - offsetof(struct darray_header, data))

/*******************************************************************************
* structure: darray_header
* purpose: hidden structure contains array metadata
* @ cache : unused reserved block of 8 bytes
* @ capacity : current maximum size of array
* @ length : current number of elements held in array
* @ data : stores elements contained in array

            ----------#----------#----------#------------------
            |  cache  # capacity #  length  #  data ---------->
            ----------#----------#----------#------------------

            \_____________________________/  \_______________/
                    hidden metadata            exposed array

* note: 16 byte header to ensure 0 pad in most specifications of array_item and
* to prevent data member from occupying trailing padding after length. The cache
* isn't necessary, but maintains the header packing up to 16 bit array items.
*******************************************************************************/

struct darray_header
{
    double cache;
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

    //if some compiler left trailing padding after the header,
    //then the flexible array may push back onto the trailing padding.
    size_t array_size = sizeof(array_item) * INIT_CAPACITY;
    size_t yes_pad = offsetof(struct darray_header, data) + array_size;
    size_t no_pad = sizeof(struct darray_header) + array_size;

    DARRAY_TRACE("yes_pad: %d, no_pad: %d bytes\n", (int)yes_pad, (int)no_pad);

    //malloc the cheaper option, but most of the time no_pad = yes_pad
    struct darray_header *dh;
    dh = yes_pad < no_pad ? malloc(yes_pad) : malloc(no_pad);
    VERIFY_POINTER(malloc, dh);

    //set members
    dh->cache = 0.0;
    dh->capacity = INIT_CAPACITY;
    dh->length = 0;

    DARRAY_TRACE("dynamic array created, %d elements\n", dh->capacity);
    //expose array to client but keep the header hidden
    return dh->data;
}

/******************************************************************************/

void darray_destroy(darray d)
{
    assert(d != NULL);

    //creates pointer to containing structure by manipulating the input pointer
    /* dh = */ DARRAY_HEADER_VAR(d);

    //just check that we actually arrived at the structure correctly
    assert(dh->capacity);
    assert(dh->data[0] == *d);

    DARRAY_TRACE("destroying dynamic array "
                 "with attributes\n\tcache: %g\n\tcapacity: %d\n\tlength: %d\n",
                 dh->cache, dh->capacity, dh->length);

    //pointers are checking out okay, lets free the memory block
    free(dh);
    return;
}
