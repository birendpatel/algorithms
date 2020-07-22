/*
* Author: Biren Patel
* Description: Vector Deque intended for faster dequeue/enqueue operations than 
* a dynamic array, but with much better locality of reference than linked lists.
*/

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/*******************************************************************************
* macro: vector_type
* purpose: vector metadata struct used to access the API functionality
* @ destroy : function pointer if required to destroy vector items, else NULL
* @ capacity : number of elements that vector can hold before resize triggers
* @ L_idx : index of the first element in the vector
* @ R_idx : index of the last element in the vector
* @ offset : hiding area for a pointer back to the start of the struct
* @ vector : the flexible array member that the client interacts with
*******************************************************************************/

#define vector_type(name, type)                                                \
                                                                               \
typedef struct name##_                                                         \
{                                                                              \
    void (*destroy)(void *data);                                               \
    uint64_t capacity;                                                         \
    uint64_t count;                                                            \
    uint64_t L_idx;                                                            \
    uint64_t R_idx;                                                            \
    type vector[];                                                             \
} * name;
        

/*******************************************************************************
* macro: vector_declarations
* purpose: prototypes
*******************************************************************************/

#define vector_declarations(scope, name, type)                                 \
                                                                               \
scope name name##_create (void (*des)(void *data), uint64_t n, uint64_t ofs);  \
scope void name##_destroy (name vector);                                       \


/*******************************************************************************
* function: name##_create
* purpose: constructor
* @ des : function pointer if required to destroy vector items, else NULL
* @ n : initial capacity of vector
* @ ofs : offset of first element in vector, less than n
* returns: struct name_ or NULL if not successful
*******************************************************************************/

#define vector_definition_create(scope, name, type)                            \
                                                                               \
scope name name##_create (void (*des)(void *data), uint64_t n, uint64_t ofs)   \
{                                                                              \
    name vec = malloc(offsetof(struct name##_, vector) + n);                   \
    if (vec == NULL) return NULL;                                              \
                                                                               \
    vec->destroy = des;                                                        \
    vec->capacity = n;                                                         \
    vec->count = 0;                                                            \
    vec->L_idx = ofs;                                                          \
    vec->R_idx = ofs;                                                          \
                                                                               \
    return vec;                                                                \
}                                                                              \

/*******************************************************************************
* function: name##_destroy
* purpose: destructor
* @ vec : variable of type struct name__
*******************************************************************************/

#define vector_definition_destroy(scope, name, type)                           \
                                                                               \
scope void name##_destroy (name vec)                                           \
{                                                                              \
    assert(vec != NULL && "input vector is null");                             \
                                                                               \
    if (vec->destroy != NULL)                                                  \
    {                                                                          \
                                                                               \
    }                                                                          \
                                                                               \
    free(vec);                                                                 \
}                                                                              \

/*******************************************************************************
* macro: make_vector
* purpose: general purpose wrapper over the three definitions for easy API use
*******************************************************************************/

#define define_vector(scope, name, type)                                       \
                                                                               \
vector_type(name, type)                                                        \
vector_declarations(scope, name, type)                                         \
vector_definition_create(scope, name, type)                                    \
vector_definition_destroy(scope, name, type)                                   \
void name##_vector_deque(void)
        
#endif