/*
* Author: Biren Patel
* Description: Implementation for dynamic array abstract data type
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vector.h"

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
        }                                                                      \

/*******************************************************************************
* structure: vector
* @ data : stores elements contained in vector
* @ capacity : tracks the maximum size of the vector
* @ length : tracks the number of elements held in the vector
*******************************************************************************/
struct vector
{
    array_item *data;
    int capacity;
    int length;
};

/*******************************************************************************
* public functions
*******************************************************************************/

struct vector *vector_create(void)
{
    struct vector *vec = malloc(sizeof(struct vector));
    VERIFY_POINTER(malloc, vec);

    vec->data = malloc(VECTOR_START_CAPACITY * sizeof(array_item));
    VERIFY_POINTER(malloc, vec->data);

    vec->capacity = VECTOR_START_CAPACITY;
    vec->length = 0;

    #ifdef VECTOR_DEBUG
    printf("--> vector created\n");
    #endif

    return vec;
}

/******************************************************************************/

void vector_destroy(struct vector *vec)
{
    assert(vec != NULL);

    free(vec->data);
    free(vec);

    #ifdef VECTOR_DEBUG
    printf("--> destroyed vector\n");
    #endif
}

/******************************************************************************/

int vector_append(struct vector *vec, array_item element)
{
    assert(vec != NULL);
    assert(vec->capacity >= vec->length);
    assert(vec->length >= 0 && vec->length <= vec->capacity);

    #ifdef VECTOR_DEBUG
    printf("--> appending element\n");
    #endif

    if (vec->length == vec->capacity)
    {
        #ifdef VECTOR_DEBUG
        printf("--> increasing capacity to %d\n", VECTOR_GROW(vec->capacity));
        #endif

        vec->capacity = VECTOR_GROW(vec->capacity);
        assert(vec->capacity >= vec->length);

        vec->data = realloc(vec->data,  vec->capacity * sizeof(array_item));
        VERIFY_POINTER(realloc, vec->data);
    }

    vec->data[vec->length++] = element;

    #ifdef VECTOR_DEBUG
    printf("--> length now at %d\n", vec->length);
    #endif

    assert(vec->capacity >= vec->length);
    assert(vec->length >= 0 && vec->length <= vec->capacity);

    return vec->length;
}

/******************************************************************************/

array_item *vector_get(struct vector *vec, int index)
{
    assert(vec != NULL);
    assert(index >= 0);

    #ifdef VECTOR_DEBUG
    printf("--> fetching pointer to element at index %d\n", index);
    #endif

    if (index >= vec->length)
    {
        #ifdef VECTOR_DEBUG
        printf("--> index is out of bounds of current length\n");
        #endif

        return NULL;
    }
    else
    {
        return vec->data + index;
    }
}

/******************************************************************************/

int vector_set(struct vector *vec, int index, array_item element)
{
    assert(vec != NULL);
    assert(index >= 0);

    if (index >= vec->length)
    {
        #ifdef VECTOR_DEBUG
        printf("--> attempted to change value at index %d, failed\n", index);
        #endif

        return 0;
    }
    else
    {
        #ifdef VECTOR_DEBUG
        printf("--> attempted to change value at index %d, success\n", index);
        #endif

        vec->data[index] = element;
        return 1;
    }
}

/******************************************************************************/

void vector_clear(struct vector *vec)
{
    assert(vec != NULL);

    #ifdef VECTOR_DEBUG
    printf("--> resetting length but memory allocation remains\n");
    #endif

    vec->length = 0;

    assert(vec->length == 0);
}

/******************************************************************************/

int vector_len(struct vector *vec)
{
    #ifdef VECTOR_DEBUG
    printf("--> getting current length of vector\n");
    #endif

    assert(vec != NULL);
    return vec->length;
}

/******************************************************************************/

void vector_show(struct vector *vec)
{
    assert(vec != NULL);

    #ifdef VECTOR_DEBUG
    printf("--> printing vector contents to stdout\n");
    #endif

    for (size_t i = 0; i < vec->length; ++i)
    {
        printf(FMT_STRING, vec->data[i]);
    }

    puts("");
}
