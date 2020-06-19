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
        ((struct darray_header *)                                              \
        (((int8_t*) (d)) - offsetof(struct darray_header, data)))

/*******************************************************************************
* structure: darray_header
* purpose: hidden structure contains array metadata
* @ cache : unused reserved block of 8 bytes
* @ capacity : current maximum size of array
* @ length : current number of elements held in array
* @ data : stores elements contained in array

        #-----------#------------#------------#-------------------#
        #   cache   #  capacity  #   length   #  data ----------> #
        #-----------#------------#------------#-------------------#

        \___________________________________/  \_________________/
                    hidden metadata              exposed array

* note: 16 byte header to ensure 0 pad in most specifications of array_item and
* to prevent data member from occupying trailing padding after length member.
* The cache is unused thus far.
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

    //16 bytes is absolute minimum if fully packed, > not >= b/c 1 element
    assert(yes_pad > 16 && no_pad >  16);
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

    //define pointer to containing structure via the input pointer
    struct darray_header *dh = DARRAY_HEADER_VAR(d);

    //check that we have the stucture
    assert(dh->data[0] == *d);

    DARRAY_TRACE("destroying dynamic array "
                 "with attributes\n\tcache: %g\n\tcapacity: %d\n\tlength: %d\n",
                 dh->cache, dh->capacity, dh->length);

    //pointers are checking out okay, lets free the memory block
    free(dh);
    return;
}

/******************************************************************************/

void darray_append(darray *d, array_item element)
{
    assert(d != NULL && *d != NULL);

    //define pointer to structure via the dereferenced input pointer
    struct darray_header *dh = DARRAY_HEADER_VAR(*d);

    //check that we have the stucture and check length in bounds
    assert(dh->data[0] == **d);
    assert(dh->length >= 0 && dh->length <= dh->capacity);

    DARRAY_TRACE("appending element%c\n", ' ');

    if (dh->length == dh->capacity)
    {
        DARRAY_TRACE("inc capacity to %d\n", INCREASE_CAPACITY(dh->capacity));

        dh->capacity = INCREASE_CAPACITY(dh->capacity);
        assert(dh->capacity > dh->length);

        //repeat the same check from constructor. SHOULD BE A FLAG IN STRUCT
        size_t array_size = sizeof(array_item) * dh->capacity;
        size_t yes_pad = offsetof(struct darray_header, data) + array_size;
        size_t no_pad = sizeof(struct darray_header) + array_size;

        DARRAY_TRACE("yes_pad: %d, no_pad: %d bytes\n",
                     (int)yes_pad, (int)no_pad);

        struct darray_header *tmp;
        tmp = yes_pad < no_pad ? realloc(dh, yes_pad) : realloc(dh, no_pad);
        VERIFY_POINTER(realloc, tmp);
        dh = tmp;
    }

    dh->data[dh->length++] = element;

    //by now, its possible that realloc moved dh to a new section of memory.
    //if so, the original darray d input would be pointing somwhere invalid.
    //so let's update it just in case.
    //this is why we accepted a pointer to a darray (array_item **)
    *d = dh->data;

    DARRAY_TRACE("length now at %d\n", dh->length);
    assert(dh->length >= 0 && dh->length <= dh->capacity);
    assert(dh->capacity >= dh->length); //not eq unless client changes growth
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
