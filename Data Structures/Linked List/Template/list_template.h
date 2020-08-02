/*
* Author: Biren Patel
* Description: doubly linked list mimic of a c++ template style data structure.
* The void generics in the other repository folders contain documentation and
* comments. This header should only serve for usage, not reference. 
*/

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#define LIST_TYPES(name, type)                                                 \
                                                                               \
typedef struct name##_node_                                                    \
{                                                                              \
    struct name##_node_ *prev;                                                 \
    struct name##_node_ *next;                                                 \
    type data;                                                                 \
} * name##_node;                                                               \
                                                                               \
                                                                               \
typedef struct name##_                                                         \
{                                                                              \
    void (*destroy)(type data);                                                \
    struct name##_node_ *head;                                                 \
    struct name##_node_ *tail;                                                 \
    int size;                                                                  \
} * name;                                                                      \


/******************************************************************************/

#define LIST_PROTOTYPES(name, type, scope)                                     \
                                                                               \
scope struct name##_ * name##_create (void (*destroy)(type data));             \
scope void name##_destroy (struct name##_ *list);                              \
scope struct name##_node_ * name##_insert_pos( struct name##_  *list, int pos, type data); \
scope type name##_remove_pos( struct name##_  *list, int pos);                 \
scope type name##_access_pos( struct name##_  *list, int pos);                 \
scope struct name##_node_ * name##_insert_node( struct name##_  *list,  struct name##_node_  *node, type data,char method); \
scope type name##_remove_node( struct name##_  *list,  struct name##_node_  *node, char method); \
scope bool name##_search_node( struct name##_  *list, struct name##_node_  *node,char method,int (*cmp)(const type item_1, const type item_2)); \
scope struct name##_node_ * name##_search( struct name##_  *list, type data,char method,int (*cmp)(const type item_1, const type item_2)); \
scope struct name##_node_ * name##_concat( struct name##_  *A,  struct name##_  *B); \
scope struct name##_node_ * name##_copy( struct name##_  *A,  struct name##_  *B); \
scope int name##_size( struct name##_  *list);                                 \
scope struct name##_node_ * name##_push_head(struct name##_ *list, type data);  \
scope struct name##_node_ * name##_push_tail(struct name##_ *list, type data);  \
scope type name##_pop_head(struct name##_ *list);                              \
scope type name##_pop_tail(struct name##_ *list);                              \
scope type name##_peek_head(struct name##_ *list);                             \
scope type name##_peek_tail(struct name##_ *list);                             \

/******************************************************************************/

#define LIST_DEFINITIONS(name, type, scope)                                    \
                                                                               \
scope struct name##_ * name##_create (void (*destroy)(type data))              \
{                                                                              \
    struct name##_ *list = malloc(sizeof(struct name##_));                     \
    if (list == NULL) return NULL;                                             \
                                                                               \
    list->destroy = destroy;                                                   \
    list->head = NULL;                                                         \
    list->tail = NULL;                                                         \
    list->size = 0;                                                            \
                                                                               \
    return list;                                                               \
}                                                                              \
                                                                               \
scope void name##_destroy (struct name##_ *list)                               \
{                                                                              \
    assert(list != NULL && "input list pointer is null");                      \
                                                                               \
    while (list->head != NULL)                                                 \
    {                                                                          \
        if (list->destroy == NULL) name##_pop_head  (list);                        \
        else (*list->destroy)(name##_pop_head  (list));                            \
    }                                                                          \
                                                                               \
    free(list);                                                                \
}                                                                              \
                                                                               \
scope struct name##_node_ * name##_insert_pos( struct name##_  *list, int pos, type data) \
{                                                                              \
    assert(list != NULL && "input list pointer is null");                      \
    assert(list->size < INT_MAX && "list is full");                            \
    assert(pos <= list->size && pos >= 0 && "position out of bounds");         \
                                                                               \
    struct name##_node_  *new_node = malloc(sizeof( struct name##_node_ ));    \
    if (new_node == NULL) return NULL;                                         \
                                                                               \
    if (list->size == 0)                                                       \
    {                                                                          \
        new_node->prev = NULL;                                                 \
        new_node->next = NULL;                                                 \
                                                                               \
        list->head = new_node;                                                 \
        list->tail = new_node;                                                 \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        if (pos == 0)                                                          \
        {                                                                      \
            new_node->prev = NULL;                                             \
            new_node->next = list->head;                                       \
                                                                               \
            list->head->prev = new_node;                                       \
                                                                               \
            list->head = new_node;                                             \
        }                                                                      \
        else if (pos == list->size)                                            \
        {                                                                      \
            new_node->prev = list->tail;                                       \
            new_node->next = NULL;                                             \
                                                                               \
            list->tail->next = new_node;                                       \
                                                                               \
            list->tail = new_node;                                             \
        }                                                                      \
        else                                                                   \
        {                                                                      \
             struct name##_node_  *curr = list->head->next;                    \
                                                                               \
            for (int i = 1; i < pos; ++i) curr = curr->next;                   \
            assert(curr != NULL && "walked off end of list");                  \
                                                                               \
            new_node->prev = curr->prev;                                       \
            new_node->next = curr;                                             \
                                                                               \
            new_node->prev->next = new_node;                                   \
            curr->prev = new_node;                                             \
        }                                                                      \
    }                                                                          \
                                                                               \
    new_node->data = data;                                                     \
                                                                               \
    ++list->size;                                                              \
                                                                               \
    return new_node;                                                           \
}                                                                              \
 \
scope type name##_remove_pos( struct name##_  *list, int pos) \
{ \
    assert(list != NULL && "input list pointer is null"); \
    assert(pos < list->size && pos >= 0 && "position out of bounds"); \
\
    struct name##_node_   *removed_node; \
 \
    if (pos == 0) \
    { \
        removed_node = list->head; \
 \
        list->head = list->head->next; \
 \
        if (list->head == NULL) list->tail = NULL; \
        else list->head->prev = NULL; \
 \
    } \
    else if (pos == list->size - 1) \
    { \
        removed_node = list->tail; \
 \
        list->tail = list->tail->prev; \
 \
        if (list->tail == NULL) list->head = NULL; \
        else list->tail->next = NULL; \
    } \
    else \
    { \
        struct name##_node_   *curr = list->head->next; \
 \
        for (int i = 1; i < pos; ++i) curr = curr->next; \
        assert(curr != list->tail->next && "walked off end of list"); \
        removed_node = curr; \
\
        curr->next->prev = curr->prev; \
        curr->prev->next = curr->next; \
    } \
\
    type data = removed_node->data; \
\
    free(removed_node); \
 \
    --list->size; \
 \
    return data; \
} \
\
scope type name##_access_pos( struct name##_  *list, int pos) \
{   \
    assert(list != NULL && "input list pointer is null"); \
    assert(pos < list->size && pos >= -1 * list->size && "invalid position"); \
 \
    if (pos == 0 || pos == -1 * list->size) \
    { \
        return list->head->data; \
    } \
    else if (pos == -1 || pos == list->size -1) \
    { \
        return list->tail->data; \
    } \
    else \
    { \
        struct name##_node_   *curr; \
        \
        if (pos >= 1) \
        { \
            curr = list->head->next; \
            for (int i = 1; i < pos; ++i) curr = curr->next; \
        } \
        else \
        { \
            curr = list->tail->prev; \
            for (int i = -1; i > pos; --i) curr = curr->prev; \
        } \
         \
        return curr->data; \
    } \
} \
 \
scope struct name##_node_ * name##_insert_node( struct name##_  *list,  struct name##_node_  *node, type data,char method) \
{ \
    assert(list != NULL && "input list pointer is null"); \
    assert(list->size < INT_MAX && "list is full"); \
    assert((method == 1 || method == 2) && "invalid method"); \
 \
    struct name##_node_   *new_node = malloc(sizeof(struct name##_node_  )); \
    if (new_node == NULL) return NULL; \
 \
    new_node->data = data; \
 \
    if (list->size == 0) \
    { \
        new_node->prev = NULL; \
        new_node->next = NULL; \
        list->head = new_node; \
        list->tail = new_node; \
    } \
    else \
    { \
        switch(method) \
        { \
            case 1: if (node == NULL || node == list->tail) \
                    { \
                        new_node->prev = list->tail; \
                        new_node->next = NULL; \
                        list->tail->next = new_node; \
                        list->tail = new_node; \
                    } \
                    else \
                    { \
                        new_node->prev = node; \
                        new_node->next = node->next; \
                        node->next = new_node; \
                        new_node->next->prev = new_node; \
                    } \
 \
                    break; \
 \
            case 2: if (node == NULL || node == list->head) \
                    { \
                        new_node->prev = NULL; \
                        new_node->next = list->head; \
                        list->head->prev = new_node; \
                        list->head = new_node; \
                    } \
                    else \
                    { \
                        new_node->prev = node->prev; \
                        new_node->next = node; \
                        node->prev = new_node; \
                        new_node->prev->next = new_node; \
                    } \
 \
                    break; \
        } \
    } \
 \
    ++list->size; \
    return new_node; \
} \
 \
scope type name##_remove_node( struct name##_  *list,  struct name##_node_  *node, char method) \
{ \
    assert(list != NULL && "input list pointer is null"); \
    assert((method == 0 || method == 1 || method == 2) && "invalid method"); \
    assert(!(method == 0 && node == NULL) && "null node on middle removal"); \
 \
    struct name##_node_   *del_node; \
    type ret_data; \
\
    switch(method) \
    { \
        case 0: del_node = node; \
                node->next->prev = node->prev; \
                node->prev->next = node->next; \
 \
                break; \
 \
        case 1: del_node = list->tail; \
                list->tail = list->tail->prev; \
 \
                if (list->tail == NULL) list->head = NULL; \
                else list->tail->next = NULL; \
 \
                break; \
 \
        case 2: del_node = list->head; \
                list->head = list->head->next; \
 \
                if (list->head == NULL) list->tail = NULL; \
                else list->head->prev = NULL; \
 \
                break; \
    } \
 \
    ret_data = del_node->data; \
    free(del_node); \
    --list->size; \
 \
    return ret_data; \
} \
 \
scope bool name##_search_node( struct name##_  *list, struct name##_node_  *node,char method,int (*cmp)(const type item_1, const type item_2)) \
{ \
    assert(list != NULL && "input list pointer is null"); \
    assert(node != NULL && "input node pointer is null"); \
    assert((method == 1 || method == 2) && "invalid method type"); \
    \
    struct name##_node_   *curr; \
 \
    switch(method) \
    { \
        case 1: for (curr = list->head; curr != NULL; curr=curr->next) \
                { \
                    if (cmp == NULL) \
                    { \
                        if (curr->prev == node->prev && \
                            curr->next == node->next && \
                            curr->data == node->data) return true; \
                    } \
                    else \
                    { \
                        if (curr->prev == node->prev && \
                            curr->next == node->next && \
                            (*cmp)(curr->data, node->data)) return true; \
                    } \
                } \
 \
                break; \
 \
        case 2: for (curr = list->tail; curr != NULL; curr=curr->prev) \
                { \
                    if (cmp == NULL) \
                    { \
                        if (curr->prev == node->prev && \
                            curr->next == node->next && \
                            curr->data == node->data) return true; \
                    } \
                    else \
                    { \
                        if (curr->prev == node->prev && \
                            curr->next == node->next && \
                            (*cmp)(curr->data, node->data)) return true; \
                    } \
                } \
 \
                break; \
    } \
 \
    return false; \
} \
 \
scope struct name##_node_ * name##_search( struct name##_  *list, type data,char method,int (*cmp)(const type item_1, const type item_2)) \
{ \
    assert(list != NULL && "input list pointer is null"); \
    assert((method == 1 || method == 2) && "invalid method type"); \
 \
    struct name##_node_   *curr = NULL; \
 \
    switch(method) \
    { \
        case 1: for (curr = list->head; curr != NULL; curr=curr->next) \
                { \
                    if (cmp == NULL) \
                    { \
                        if (curr->data == data) return curr; \
                    } \
                    else \
                    { \
                        if ((*cmp)(curr->data, data)) return curr; \
                    } \
                } \
 \
                break; \
 \
        case 2: for (curr = list->tail; curr != NULL; curr=curr->prev) \
                { \
                    if (cmp == NULL) \
                    { \
                        if (curr->data == data) return curr; \
                    } \
                    else \
                    { \
                        if ((*cmp)(curr->data, data)) return curr; \
                    } \
                } \
 \
                break; \
    } \
 \
    return curr; \
} \
 \
scope struct name##_node_ * name##_concat( struct name##_  *A,  struct name##_  *B) \
{ \
    assert(A != NULL && "input list pointer A is null"); \
    assert(B != NULL && "input list pointer B is null"); \
    assert(A->size != 0 && "nothing to concatenate to"); \
    assert(B->size != 0 && "nothing to concatenate from"); \
 \
    A->tail->next = B->head; \
    B->head->prev = A->tail; \
    A->tail = B->tail; \
 \
    A->size += B->size; \
    \
    struct name##_node_   *ret_node = B->head;\
 \
    B->head = NULL; \
    B->tail = NULL; \
    B->size = 0; \
 \
    return ret_node; \
} \
 \
scope struct name##_node_ * name##_copy( struct name##_  *A,  struct name##_  *B) \
{ \
    assert(A != NULL && "input list pointer A is null"); \
    assert(B != NULL && "input list pointer B is null"); \
    assert(B->size != 0 && "nothing to concatenate"); \
 \
    struct name##_node_   *ret_node = NULL; \
    struct name##_node_   *old_tail = A->tail; \
 \
    for (struct name##_node_   *curr = B->head; curr != NULL; curr = curr->next) \
    { \
        struct name##_node_   *push_node = name##_push_tail(A, curr->data); \
 \
        if (push_node == NULL) \
        { \
            while (A->tail != old_tail) \
            { \
                if (A->destroy == NULL) name##_pop_tail  (A); \
                else (*A->destroy)(name##_pop_tail  (A)); \
            } \
 \
            return NULL; \
        } \
        \
        if (curr == B->head) ret_node = push_node; \
    } \
 \
    return ret_node; \
} \
 \
scope int name##_size( struct name##_  *list) \
{ \
    assert(list != NULL && "input list pointer is null"); \
    \
    return list->size; \
} \
 \
 \
scope struct name##_node_ * name##_push_head(struct name##_ *list, type data) \
{ \
    return name##_insert_node (list, NULL, data, 2); \
} \
 \
scope struct name##_node_ * name##_push_tail(struct name##_ *list, type data) \
{ \
    return name##_insert_node (list, NULL, data, 1); \
} \
 \
scope type name##_pop_head(struct name##_ *list) \
{ \
    return name##_remove_node(list, NULL, 2); \
} \
 \
scope type name##_pop_tail(struct name##_ *list) \
{ \
    return name##_remove_node(list, NULL, 1); \
} \
 \
scope type name##_peek_head(struct name##_ *list) \
{ \
    return name##_access_pos(list, 0); \
} \
 \
scope type name##_peek_tail(struct name##_ *list) \
{ \
    return name##_access_pos(list, -1); \
} \

/******************************************************************************/

#define define_list(name, type, scope)                                         \
LIST_TYPES(name, type)                                                         \
LIST_PROTOTYPES(name, type, scope)                                             \
LIST_DEFINITIONS(name, type, scope)                                            \
char list_h = 0                                                                \

#endif