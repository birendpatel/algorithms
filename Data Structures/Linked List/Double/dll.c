/*
* Author: Biren Patel
* Description: Generic doubly linked list implementation via void pointers.
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "dll.h"

/*******************************************************************************
* public functions
*******************************************************************************/

struct dll *dll_create(void (*destroy)(void *data))
{
    //allocate memory for returning struct
    struct dll *list = malloc(sizeof(struct dll));
    if (list == NULL) return NULL;
    
    //populate dll members
    list->destroy = destroy;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    
    return list;
}

/******************************************************************************/

void dll_destroy(struct dll *list)
{
    assert(list != NULL && "input list pointer is null");
    
    while (list->head != NULL)
    {
        if (list->destroy == NULL) dll_pop_head(list);
        else (*list->destroy)(dll_pop_head(list));
    }
    
    free(list);
}

/******************************************************************************/

struct dll_node *dll_insert_pos(struct dll *list, uint32_t pos, void *datum)
{
    assert(list != NULL && "input list pointer is null");
    assert(pos <= list->size && "position out of bounds");

    //allocate memory for a new node
    struct dll_node *new_node = malloc(sizeof(struct dll_node));
    if (new_node == NULL) return NULL;
    
    //configure pointers in metadata and the new node
    if (list->size == 0)
    {
        //insert into empty list, first set pointers on new node
        new_node->prev = NULL;
        new_node->next = NULL;
        
        //now set metadata pointers
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        if (pos == 0)
        {
            //insert at head, first set pointers on new node
            new_node->prev = NULL;
            new_node->next = list->head;
            
            //update old head to point back to new node
            list->head->prev = new_node;
            
            //update list metadata head pointer
            list->head = new_node;
        }
        else if (pos == list->size)
        {
            //insert at tail, first set pointers on new node
            new_node->prev = list->tail;
            new_node->next = NULL;
            
            //update old tail to point toward new node
            list->tail->next = new_node;
            
            //update list metadata tail pointer
            list->tail = new_node;
        }
        else
        {
            //insert within list starting at the first non-head node
            struct dll_node *curr = list->head->next;
            
            //walk the list to find the node to be shifted
            for (uint32_t i = 1; i < pos; ++i) curr = curr->next;
            assert(curr != NULL && "walked off end of list");
            
            //insert new node at this location, first update new node
            new_node->prev = curr->prev;
            new_node->next = curr;
            
            //update pointers on neighbor nodes
            new_node->prev->next = new_node;
            curr->prev = new_node;
        }
    }
    
    //place data in new node
    new_node->datum = datum;
    
    //update metadata
    ++list->size;
     
    return new_node;
}


/******************************************************************************/

void *dll_remove_pos(struct dll *list, uint32_t pos)
{
    assert(list != NULL && "input list pointer is null");
    assert(pos < list->size && "position out of bounds");
    
    struct dll_node *removed_node;
    
    if (pos == 0)
    {
        //remove head
        removed_node = list->head;
        
        list->head = list->head->next;
        
        if (list->head == NULL) list->tail = NULL;
        else list->head->prev = NULL;
        
    }
    else if (pos == list->size - 1)
    {
        //remove tail
        removed_node = list->tail;
        
        list->tail = list->tail->prev;
        
        if (list->tail == NULL) list->head = NULL;
        else list->tail->next = NULL;
    }
    else
    {
        //remove within list starting at the first non-head node
        struct dll_node *curr = list->head->next;
        
        //walk the list to find the node to be removed
        for (uint32_t i = 1; i < pos; ++i) curr = curr->next;
        assert(curr != list->tail->next && "walked off end of list");
        removed_node = curr;
        
        //use found node to connect its prev and next nodes together
        curr->next->prev = curr->prev;
        curr->prev->next = curr->next;        
    }
    
    //remove data from node, update metadata, and clean up memory
    void *datum = removed_node->datum;
    
    free(removed_node);
    
    --list->size;
    
    return datum;
}

/******************************************************************************/

void *dll_access_pos(struct dll *list, uint32_t pos)
{
    assert(list != NULL && "input list pointer is null");
    assert(pos < list->size && "position out of bounds");
    
    if (pos == 0)
    {
        return list->head->datum;
    }
    else if (pos == list->size - 1)
    {
        return list->tail->datum;
    }
    else
    {
        struct dll_node *curr = list->head->next;
        
        for (uint32_t i = 1; i < pos; ++i) curr = curr->next;
        
        return curr->datum;
    }
}

/******************************************************************************/

struct dll_node *dll_insert_node
(
    struct dll *list,
    struct dll_node *node,
    void *datum,
    char method
)
{
    assert(list != NULL && "input list pointer is null");
    assert((node == NULL || dll_search_node(list, node, 1)) && "no node");
    assert(!(node == NULL && list->size != 0) && "null node on non empty list");
    assert((method == 1 || method == 2) && "invalid method");
    
    struct dll_node *new_node = malloc(sizeof(struct dll_node));
    if (new_node == NULL) return NULL;
    
    if (list->size == 0)
    {
        //insert into empty list
        new_node->prev = NULL;
        new_node->next = NULL;
        
        list->head = new_node;
        list->tail = new_node;
        
    }
    else
    {
        if (method == 1)
        {
            //insert after the input node
            new_node->prev = node;
            new_node->next = node->next;
            node->next = new_node;
            
            if (new_node->next == NULL) list->tail = new_node;
            else new_node->next->prev = new_node;
        }
        else if (method == 2)
        {
            //insert before the input node
            new_node->prev = node->prev;
            new_node->next = node;
            node->prev = new_node;
            
            if (new_node->prev == NULL) list->head = new_node;
            else new_node->prev->next = new_node;
        }
    }
    
    //place data in new node
    new_node->datum = datum;
    
    //update metadata
    ++list->size;
    
    return new_node;
}

/******************************************************************************/

void *dll_remove_node(struct dll *list, struct dll_node *node)
{
    assert(list != NULL && "input list pointer is null");
    assert(node != NULL && "input node pointer is null");
    assert(dll_search_node(list, node, 1) == true && "node not in list");
    
    //same as dll_remove_pos but without list walk, so O(1) complexity
    if (node == list->head)
    {
        list->head = list->head->next;
        
        if (list->head == NULL) list->tail = NULL;
        else list->head->prev = NULL;
    }
    else if (node == list->tail)
    {
        list->tail = list->tail->prev;
        
        if (list->tail == NULL) list->head = NULL;
        else list->tail->next = NULL;
    }
    else
    {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
    
    void *datum = node->datum;
    
    free(node);
    
    --list->size;
    
    return datum;
}

/******************************************************************************/

bool dll_search_node(struct dll *list, struct dll_node *node, char method)
{
    assert(list != NULL && "input list pointer is null");
    assert(node != NULL && "input node pointer is null");
    assert((method == 1 || method == 2) && "invalid method type");
    
    switch(method)
    {
        case 1:
        
        for (struct dll_node *curr = list->head; curr != NULL; curr=curr->next)
        {
            if (curr == node) return true;
        }
        
        break;
        
        case 2:
        
        for (struct dll_node *curr = list->tail; curr != NULL; curr=curr->prev)
        {
            if (curr == node) return true;
        }
        
        break;
    }
    
    return false;
}

/******************************************************************************/

struct dll_node *dll_search(struct dll *list, void *datum, char method)
{
    assert(list != NULL && "input list pointer is null");
    assert((method == 1 || method == 2) && "invalid method type");
    
    switch(method)
    {
        case 1:
        
        for (struct dll_node *curr = list->head; curr != NULL; curr=curr->next)
        {
            if (curr->datum == datum) return curr;
        }
        
        break;
        
        case 2:

        for (struct dll_node *curr = list->tail; curr != NULL; curr=curr->prev)
        {
            if (curr->datum == datum) return curr;
        }   
            
        break;
    }
    
    return NULL;
}

/******************************************************************************/

struct dll_node *dll_concat(struct dll *A, struct dll *B)
{
    assert(A != NULL && "input list pointer A is null");
    assert(B != NULL && "input list pointer B is null");
    
    assert(A->size != 0 && "nothing to concatenate to");
    assert(B->size != 0 && "nothing to concatenate from");
    
    //set up new links between tail of A and head of B
    A->tail->next = B->head;
    B->head->prev = A->tail;
    A->tail = B->tail;
    
    //update metadata on A
    A->size += B->size;
    
    //get ret val
    struct dll_node *ret_node = B->head;
    
    //make B empty list
    B->head = NULL;
    B->tail = NULL;
    B->size = 0;
    
    return ret_node;
}

/******************************************************************************/

struct dll_node *dll_copy(struct dll *A, struct dll *B)
{
    assert(A != NULL && "input list pointer A is null");
    assert(B != NULL && "input list pointer B is null");
    assert(B->size != 0 && "nothing to concatenate");
    
    struct dll_node *ret_node = NULL;
    struct dll_node *old_tail = A->tail;
    
    //walk list B and copy node data to list A one by one    
    for (struct dll_node *curr = B->head; curr != NULL; curr = curr->next)
    {
        struct dll_node *push_node = dll_push_tail(A, curr->datum);
        
        //malloc failure on push
        if (push_node == NULL)
        {
            //revert list_1 to its state prior to the function call
            while (dll_peek_tail(A) != old_tail)
            {
                //use supplied destroy member on constructor if available
                if (A->destroy == NULL) dll_pop_tail(A);
                else (*A->destroy)(dll_pop_tail(A));
            }
            
            return NULL;
        }
        
        //capture return node on first iter
        if (curr == B->head) ret_node = push_node;
    }
    
    return ret_node;
}
