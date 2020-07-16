/*
* Author: Biren Patel
* Description: Doubly linked list API
*/

#ifndef DLL_H
#define DLL_H

#include <stdint.h>

/*******************************************************************************
* purpose: user-modifiable hyperparameters
* @ dll_item : the datum stored in each node, set to void* for a generic list
*******************************************************************************/
typedef void* dll_item;

/*******************************************************************************
* struct: dll_node
* purpose: list node 
* @ prev : pointer to the previous node, NULL if at head node
* @ next : pointer to the next node, NULL if at tail node
* @ datum : the piece of data stored in each node
*******************************************************************************/
struct dll_node
{
    struct dll_node *prev;
    struct dll_node *next;
    dll_item datum;
};

/*******************************************************************************
* struct: dll
* purpose: client must declare pointer to this list structure to access the API
* @ destroy : pointer to function for dll_item destruction, else NULL
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
    uint32_t size;
};

/*******************************************************************************
* public function: dll_create
* purpose: constructor
* @ destroy : pointer to function for dll_item destruction, else NULL
* returns: pointer to struct dll
*******************************************************************************/
struct dll *dll_create(void (*destroy)(void *data));

/*******************************************************************************
* public function: dll_destroy
* purpose: destructor, calls destroy passed on constructor unless NULL
* @ list : pointer to struct dll
*******************************************************************************/
void dll_destroy(struct dll *list);

/*******************************************************************************
* public function: dll_insert_idx
* purpose: insert a new node at the specified index
* @ list : pointer to struct dll
* @ datum : the piece of data to store in the new node
* returns: pointer to the new node if successful, else NULL
*******************************************************************************/
struct dll_node *dll_insert_idx(struct dll *list, dll_item datum);

#endif