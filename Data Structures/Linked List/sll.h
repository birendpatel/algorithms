/*
* Author: Biren Patel
* Description: API for a singly linked list
*/

#ifndef SLL_H
#define SLL_H

#include <stdint.h>

/*******************************************************************************
* client-modifiable parameters
* @ list_item : the data type of items contained in the list
*******************************************************************************/
typedef int list_item;

/*******************************************************************************
* structure: node
* purpose: describes a single node within the list
* @ datum : the item stored at this node
* @ next : pointer to the next node in the list
*******************************************************************************/
struct node
{
    list_item datum;
    struct node *next;
};

/*******************************************************************************
* structure: sll
* purpose: client must declare pointer to this struct to access the API
* @ destroy : pointer to function used to clean up memory on destructor call
* @ head : pointer to the first node in the list
*
*
*       SLL  
*   *---------*
*   | destroy |     #------#      #------#      #------#      #------#
*   |  head   |---> # head # ---> # node # ---> # node # ---> # tail # ---> X
*   |  size   |     #------#      #------#      #------#      #------#
*   *---------*
*
*******************************************************************************/
struct sll
{
    void (*destroy)(void *object);
    struct node *head;
    uint32_t size;    
};

/*******************************************************************************
* public function: sll_create
* purpose: constructor
* @ destroy : pointer to function used to clean up memory on destructor call
* returns: pointer to struct sll
*******************************************************************************/
struct sll *sll_create(void (*destroy)(void *object));


/*******************************************************************************
* public function: sll_destroy
* purpose: destructor
* @ s : pointer to struct sll
*******************************************************************************/
void sll_destroy(struct sll *s);


/*******************************************************************************
* public function: sll_insert_idx
* purpose: insert a node at the specified index
* @ s : pointer to struct sll
* @ idx : index of insertion, where 0 is the head
* @ datum : list item to store at the new node
* returns: pointer to newly created struct node, or NULL on failure
*******************************************************************************/
struct node *sll_insert_idx(struct sll *s, uint32_t idx, list_item datum);

#endif