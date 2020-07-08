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
* @ datum : the item stored at this node
* @ next : pointer to the next node in the list
* purpose: describes a single node within the list
*******************************************************************************/
struct node
{
    list_item datum;
    struct node *next;
};

/*******************************************************************************
* structure: sll
* @ destroy : pointer to function used to clean up memory on destructor call
* @ head : pointer to the first node in the list
* purpose: client must declare variable of type sll to access API
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

#endif