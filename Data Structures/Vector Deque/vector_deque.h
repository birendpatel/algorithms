/*
* Author: Biren Patel
* Description: Vector Deque intended for faster dequeue/enqueue operations than 
* a dynamic array, but with much better locality of reference than linked lists.
* Avoiding a ring buffer implementation, so that vectors act like arrays act.
*/

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

/*******************************************************************************
* macro: vector_type
* purpose: vector metadata struct used to access the API functionality
* @ capacity : number of elements that vector can hold before resize triggers
* @ L_idx : index of the first element in the vector
* @ R_idx : index of the last element in the vector
* @ vector : the flexible array member that the client interacts with
*******************************************************************************/

#define vector_type(name, type)                                                \
                                                                               \
typedef struct name##_                                                         \
{                                                                              \
    uint64_t capacity;                                                         \
    uint64_t count;                                                            \
    uint64_t L_idx;                                                            \
    uint64_t R_idx;                                                            \
    type vector[];                                                             \
} * name;                                                                      \
        
/*******************************************************************************
* macro: vector_declarations
* purpose: prototypes
*******************************************************************************/

#define vector_declarations(scope, name, type)                                 \
                                                                               \
scope name name##_create (uint64_t n, uint64_t offset);                        \
scope void name##_destroy (name vector);                                       \
scope bool name##_push_back (name vec, type item);                             \

/*******************************************************************************
* function: name##_create
* purpose: constructor
* @ n : initial capacity of vector
* @ offset : offset of first element in vector, less than n
* returns: struct name_ or NULL if not successful
*******************************************************************************/

#define vector_definition_create(scope, name, type)                            \
                                                                               \
scope name name##_create (uint64_t n, uint64_t offset)                         \
{                                                                              \
    name vec = malloc(offsetof(struct name##_, vector) + n);                   \
    if (vec == NULL) return NULL;                                              \
                                                                               \
    vec->capacity = n;                                                         \
    vec->count = 0;                                                            \
    vec->L_idx = offset;                                                       \
    vec->R_idx = offset;                                                       \
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
    free(vec);                                                                 \
}                                                                              \

/*******************************************************************************
* function: name##_push_back
* purpose: push an element to the back of the vector
* @ vec : variable of type struct name__
* @ item : variable of macro param type
* returns: true if successful, false on failure
*******************************************************************************/

#define vector_defintion_push_back(scope, name, type)                          \
                                                                               \
scope bool name##_push_back (name vec, type item)                              \
{                                                                              \
    assert(vec != NULL && "input pointer is null");                            \
                                                                               \
    if (vec->R_idx == vec->capacity)                                           \
    {                                                                          \
        return false;                                                          \
    }                                                                          \
                                                                               \
    vec->vector[vec->R_idx++] = item;                                          \
    ++vec->count;                                                              \
                                                                               \
    return true;                                                               \
}                                                                              \

/*******************************************************************************
* name##_push_front
* purpose: push an element to the front of the vector
* @ vec: variable of type struct name__
* @ item : variable of macro param type
* returns: true if successful, false on failure
*******************************************************************************/

#define vector_definition_push_front(scope, name, type)                        \
                                                                               \
scope bool name##_push_front(name vec, type item)                              \
{                                                                              \
    assert(vec != NULL && "input vector is null");                             \
                                                                               \
    if (vec->L_idx == 0)                                                       \
    {                                                                          \
        return false;                                                          \
    }                                                                          \
                                                                               \
    vec->vector[--vec->L_idx] = item;                                          \
    ++vec->count;                                                              \
                                                                               \
    return true;                                                               \
}                                                                              \

/*******************************************************************************
* macro: make_vector
* purpose: general purpose wrapper over all other macros for easy API use
* note: the void-void prototype pollutes the namespace but allows a semicolon
*******************************************************************************/

#define make_vector(name, type, scope)                                         \
                                                                               \
vector_type(name, type)                                                        \
vector_declarations(scope, name, type)                                         \
vector_definition_create(scope, name, type)                                    \
vector_definition_destroy(scope, name, type)                                   \
vector_defintion_push_back(scope, name, type)                                  \
vector_definition_push_front(scope, name, type)                                \
void name##_vector_deque(void)                                                 \
        
#endif