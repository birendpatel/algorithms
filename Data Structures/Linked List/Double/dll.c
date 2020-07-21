/*
* Author: Biren Patel
* Description: Generic doubly linked list implementation via void pointers.
*/

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include "dll.h"

/*******************************************************************************
* struct: dll_node
* purpose: list node
* @ prev : pointer to the previous node, NULL if at head node
* @ next : pointer to the next node, NULL if at tail node
* @ data : the piece of data stored in each node
*******************************************************************************/
struct dll_node
{
    struct dll_node *prev;
    struct dll_node *next;
    void *data;
};

/*******************************************************************************
* struct: dll
* purpose: client must declare pointer to this list structure to access the API
* @ destroy : pointer to function for void * destruction, else NULL
* @ head : pointer to the first node in the list
* @ tail : pointer to the final node in the list
* @ size : the total number of nodes in the list
*

          #======#  ---->  #======#  ---->  #======#  ---->  #======#
  X <---- # head #         # node #         # node #         # tail # ----> X
          #======#  <----  #======#  ---->  #======#  <----  #======#


*******************************************************************************/
struct dll
{
    void (*destroy)(void *data);
    struct dll_node *head;
    struct dll_node *tail;
    int size;
};

/*******************************************************************************
* public functions
*******************************************************************************/

struct dll *dll_create(void (*destroy)(void *data))
{
    struct dll *list = malloc(sizeof(struct dll));
    if (list == NULL) return NULL;

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

struct dll_node *dll_insert_pos(struct dll *list, int pos, void *data)
{
    assert(list != NULL && "input list pointer is null");
    assert(list->size < INT_MAX && "list is full");
    assert(pos <= list->size && pos >= 0 && "position out of bounds");

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
            for (int i = 1; i < pos; ++i) curr = curr->next;
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
    new_node->data = data;

    //update metadata
    ++list->size;

    return new_node;
}


/******************************************************************************/

void *dll_remove_pos(struct dll *list, int pos)
{
    assert(list != NULL && "input list pointer is null");
    assert(pos < list->size && pos >= 0 && "position out of bounds");

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
        for (int i = 1; i < pos; ++i) curr = curr->next;
        assert(curr != list->tail->next && "walked off end of list");
        removed_node = curr;

        //use found node to connect its prev and next nodes together
        curr->next->prev = curr->prev;
        curr->prev->next = curr->next;
    }

    //remove data from node, update metadata, and clean up memory
    void *data = removed_node->data;

    free(removed_node);

    --list->size;

    return data;
}

/******************************************************************************/

void *dll_access_pos(struct dll *list, int pos)
{   
    assert(list != NULL && "input list pointer is null");
    assert(pos < list->size && pos >= -1 * list->size && "invalid position");

    if (pos == 0 || pos == -1 * list->size)
    {
        return list->head->data;
    }
    else if (pos == -1 || pos == list->size -1)
    {
        return list->tail->data;
    }
    else
    {
        struct dll_node *curr;
        
        if (pos >= 1)
        {
            curr = list->head->next;
            for (int i = 1; i < pos; ++i) curr = curr->next;
        }
        else
        {
            curr = list->tail->prev;
            for (int i = -1; i > pos; --i) curr = curr->prev;
        }
        
        return curr->data;
    }
}

/******************************************************************************/

struct dll_node *dll_insert_node
(
    struct dll *list,
    struct dll_node *node,
    void *data,
    char method
)
{
    assert(list != NULL && "input list pointer is null");
    assert(list->size < INT_MAX && "list is full");
    assert((method == 1 || method == 2) && "invalid method");
    assert((node == NULL || dll_search_node(list, node, 1)) && "node DNE");

    //allocate memory for a new node
    struct dll_node *new_node = malloc(sizeof(struct dll_node));
    if (new_node == NULL) return NULL;

    //place data in the new node
    new_node->data = data;

    if (list->size == 0)
    {
        new_node->prev = NULL;
        new_node->next = NULL;
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        switch(method)
        {
            case 1: if (node == NULL || node == list->tail)
                    {
                        //insert at tail
                        new_node->prev = list->tail;
                        new_node->next = NULL;
                        list->tail->next = new_node;
                        list->tail = new_node;
                    }
                    else
                    {
                        //insert in middle after the input node
                        new_node->prev = node;
                        new_node->next = node->next;
                        node->next = new_node;
                        new_node->next->prev = new_node;
                    }

                    break;

            case 2: if (node == NULL || node == list->head)
                    {
                        //insert at head
                        new_node->prev = NULL;
                        new_node->next = list->head;
                        list->head->prev = new_node;
                        list->head = new_node;
                    }
                    else
                    {
                        //insert in middle before the input node
                        new_node->prev = node->prev;
                        new_node->next = node;
                        node->prev = new_node;
                        new_node->prev->next = new_node;
                    }

                    break;
        }
    }

    ++list->size;
    return new_node;
}

/******************************************************************************/

void *dll_remove_node(struct dll *list, struct dll_node *node, char method)
{
    assert(list != NULL && "input list pointer is null");
    assert((method == 0 || method == 1 || method == 2) && "invalid method");
    assert((node == NULL || dll_search_node(list, node, 1)) && "node DNE");
    assert(!(method == 0 && node == NULL) && "null node on middle removal");

    struct dll_node *del_node;
    void *ret_data;

    switch(method)
    {
        case 0: del_node = node;
                node->next->prev = node->prev;
                node->prev->next = node->next;

                break;

        case 1: del_node = list->tail;
                list->tail = list->tail->prev;

                if (list->tail == NULL) list->head = NULL;
                else list->tail->next = NULL;

                break;

        case 2: del_node = list->head;
                list->head = list->head->next;

                if (list->head == NULL) list->tail = NULL;
                else list->head->prev = NULL;

                break;
    }

    ret_data = del_node->data;
    free(del_node);
    --list->size;

    return ret_data;
}

/******************************************************************************/

bool dll_search_node(struct dll *list, struct dll_node *node, char method)
{
    assert(list != NULL && "input list pointer is null");
    assert(node != NULL && "input node pointer is null");
    assert((method == 1 || method == 2) && "invalid method type");
    
    struct dll_node *curr;

    switch(method)
    {
        case 1: for (curr = list->head; curr != NULL; curr=curr->next)
                {
                    if (curr == node) return true;
                }

                break;

        case 2: for (curr = list->tail; curr != NULL; curr=curr->prev)
                {
                    if (curr == node) return true;
                }

                break;
    }

    return false;
}

/******************************************************************************/

struct dll_node *dll_search(struct dll *list, void *data, char method)
{
    assert(list != NULL && "input list pointer is null");
    assert((method == 1 || method == 2) && "invalid method type");

    struct dll_node *curr = NULL;

    switch(method)
    {
        case 1: for (curr = list->head; curr != NULL; curr=curr->next)
                {
                    if (curr->data == data) return curr;
                }

                break;

        case 2: for (curr = list->tail; curr != NULL; curr=curr->prev)
                {
                    if (curr->data == data) return curr;
                }

                break;
    }

    return curr;
}

/******************************************************************************/

struct dll_node *dll_concat(struct dll *A, struct dll *B)
{
    assert(A != NULL && "input list pointer A is null");
    assert(B != NULL && "input list pointer B is null");
    assert(A->size != 0 && "nothing to concatenate to");
    assert(B->size != 0 && "nothing to concatenate from");

    //configure pointers between tail of A and head of B
    A->tail->next = B->head;
    B->head->prev = A->tail;
    A->tail = B->tail;

    //update the metadata on A 
    A->size += B->size;

    //get ret val
    struct dll_node *ret_node = B->head;

    //make B empty list, user responsibility to destroy if needed
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
        struct dll_node *push_node = dll_push_tail(A, curr->data);

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

/******************************************************************************/

int dll_size(struct dll *list)
{
    assert(list != NULL && "input list pointer is null");
    
    return list->size;
}