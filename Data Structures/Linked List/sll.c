/*
* Author: Biren Patel
* Description: Implementation for a singly linked list
*/

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#include "sll.h"

/*******************************************************************************
* public functions
*******************************************************************************/

struct sll *sll_create(void (*destroy)(void *object))
{
    assert(destroy != NULL && "function pointer destroy is null");
    
    struct sll *s = malloc(sizeof(struct sll));
    if (s == NULL) return NULL;
    
    s->destroy = destroy;
    s->head = NULL;
    s->size = 0;
    
    return s;
}

/******************************************************************************/

void sll_destroy(struct sll *s)
{
    assert(s != NULL && "input sll pointer is null");
    
    if (s->destroy == free)
    {               
        //free the individual list nodes first by popping off the heads
        while (s->head != NULL) sll_remove_head(s);
        
        //now we can free the sll struct
        free(s);
    }
    else //client has passed their own implementation
    {
        (*s->destroy)(s);
    }
}

/******************************************************************************/

struct node *sll_insert_idx(struct sll *s, uint32_t idx, sll_item datum)
{
    assert(s != NULL && "input sll pointer is null");
    assert(idx <= s->size && "index of insertion out of bounds");
    assert(s->size != UINT32_MAX);
    
    //create a new node
    struct node *new_node = malloc(sizeof(struct node));
    if (new_node == NULL) return NULL;
    
    //set up next pointer on node, branch depending on if insertion at head
    if (idx == 0)
    {
        new_node->next = s->head;
        s->head = new_node;
    }
    else
    {
        //start a node pointer at the head
        struct node *lag = s->head;
       
        //walk the list until the node is just before the requested index
        for (uint32_t i = 1; i < idx; ++i) lag = lag->next;
        
        //copy the next pointer on lag node to the new node
        new_node->next = lag->next;
        
        //copy the new node pointer back to the lag node
        lag->next = new_node;        
    }
    
    //wrap up: populate node with data and increment size
    new_node->datum = datum;
    ++s->size;   

    return new_node;
}


/******************************************************************************/

sll_item sll_remove_idx(struct sll *s, uint32_t idx)
{
    assert(s != NULL && "input sll pointer is null");
    assert(idx < s->size && "index of removal is out of bounds");
    assert(s->size > 0 && "nothing to remove");
    
    struct node *removed_node;
    
    //remove node, branch depending on if removal is at head
    if (idx == 0)
    {
        removed_node = s->head;
        s->head = (s->head)->next;
    }
    else
    {
        //set up lag and lead pointers to nodes next to each other
        struct node *lag = s->head;
        struct node *lead = (s->head)->next;
        
        //walk the list until lead is at the requested index
        for (uint32_t i = 1; i < idx; ++i)
        {
            lag = lead;
            lead = lead->next;
        }
        
        //remove the node by swapping pointers on lag
        removed_node = lead;
        
        lag->next = lead->next;
    }
    
    //give the data at the node to user and clean up node memory block
    sll_item datum = removed_node->datum;
    
    free(removed_node);
    
    --s->size;
    
    return datum;
}

/******************************************************************************/

struct node *sll_access_idx(struct sll *s, uint32_t idx)
{
    assert(s != NULL && "input sll pointer is null");
    assert(idx < s->size && "index of access out of bounds");
    
    struct node *curr = s->head;
    
    for (uint32_t i = 0; i < idx; ++i) curr = curr->next;
    
    return curr;
}

/******************************************************************************/

struct node *sll_search_data(struct sll *s, sll_item datum)
{
    assert(s != NULL && "input sll pointer is null");
    
    for (struct node *curr = s->head; curr != NULL; curr = curr->next)
    {
        if (curr->datum == datum) return curr;
    }
    
    return NULL;
}

/******************************************************************************/

struct node *sll_concat
(
    struct sll *to, 
    struct sll *from, 
    char method, 
    void (*destroy)(void *data)
)
{
    assert(to != NULL && "to pointer is null");
    assert(from != NULL && "from pointer is null");
    assert(from->size > 0 && "from is empty, nothing to concatenate");
    assert((method == 0 || method == 1 || method == 2) && "invalid method");
    
    struct node *first_new_node;
    
    switch(method)
    {
        case 0: //modify next pointer on tail of 'to' to point to head of 'from'
                first_new_node = from->head;
                sll_access_tail(to)->next = first_new_node;
                
                //update metadata for 'to' list
                to->size += from->size;
                
                //modify 'from' list to revert to empty state
                from->head = NULL;
                from->size = 0;
                
                break;
                
        case 1: //modify next pointer on tail of 'to' to point to head of 'from'
                first_new_node = from->head;
                sll_access_tail(to)->next = first_new_node;
                
                //update metadata for 'to' list
                to->size += from->size;
                
                break;
                
        case 2: //copy nodes one by one
                //until I create access/insert/remove by node, this is very slow
                
                for (uint32_t i = 0; i < from->size; ++i)
                {
                    sll_item copy = sll_access_idx(from, i)->datum;
                    
                    struct node *ins_node = sll_insert_tail(to, copy);
                    
                    //malloc failed, revert to state prior to function entry
                    if (ins_node == NULL)
                    {
                        //remove nodes that succesfully copied on previous iters
                        for (uint32_t j = 0; j < i; j++)
                        {
                            sll_item datum = sll_remove_tail(to);
                            
                            //if sll_item was allocated, free the memory
                            if (destroy != NULL)
                            {
                                (*destroy)(datum);
                            }
                        }
                        
                        return NULL;
                    }
                    //end revert block
                    
                    if (i == 0) first_new_node = ins_node;
                }
                
                break;
    }
    
    return first_new_node;
}