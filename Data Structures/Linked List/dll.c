/*
* Author: Biren Patel
* Description: Doubly linked list implementation
*/

#include <stdlib.h>
#include <stdint.h>
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

struct dll_node *dll_insert_pos(struct dll *list, uint32_t pos, dll_item datum)
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

dll_item dll_remove_pos(struct dll *list, uint32_t pos)
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
    dll_item datum = removed_node->datum;
    
    free(removed_node);
    
    --list->size;
    
    return datum;
}

/******************************************************************************/

dll_item dll_access_pos(struct dll *list, uint32_t pos)
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

struct dll_node *dll_search(struct dll *list, dll_item datum, char method)
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