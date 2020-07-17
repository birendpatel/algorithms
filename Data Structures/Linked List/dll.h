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

//constructors

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

//positional functions

/*******************************************************************************
* public function: dll_insert_pos
* purpose: insert a new node at the specified position
* @ list : pointer to struct dll
* @ pos : position of insertion, equal to size of list for tail entry
* @ datum : the piece of data to store in the new node
* returns: pointer to the new node if successful, else NULL
*******************************************************************************/
struct dll_node *dll_insert_pos(struct dll *list, uint32_t pos, dll_item datum);

/*******************************************************************************
* public function: dll_remove_pos
* purpose: remove a node at the specified position
* @ list : pointer to struct dll
* @ pos : position of removal, equal to size of list - 1 for tail removal
* returns: datum stored at removed node 
*******************************************************************************/
dll_item dll_remove_pos(struct dll *list, uint32_t pos);

/*******************************************************************************
* public function: dll_access_pos
* purpose: peek data in node at specified position
* @ list : pointer to struct dll
* @ pos : position of access, equal to size of list - 1 for tail peek
* returns : item at node, of type dll_item.
*******************************************************************************/
dll_item dll_access_pos(struct dll *list, uint32_t pos);

//nodal functions



//utilities

/*******************************************************************************
* public function: dll_search
* purpose: search for data within the list
* @ list : pointer to struct dll
* @ datum : search criterion
* @ method : 1 to begin search at head, 2 to begin at tail.
* returns: the first dll_node containing the data, null if not found
*******************************************************************************/
struct dll_node *dll_search(struct dll *list, dll_item datum, char method);

/*******************************************************************************
* public function: dll_concat
* purpose: concatenate nodes from list B to tail of list A, list B becomes empty
* @ A : pointer to struct dll
* @ B : pointer to struct dll
* returns: pointer to first new node in list A
*******************************************************************************/
struct dll_node *dll_concat(struct dll *A, struct dll *B);


/*******************************************************************************
* public function: dll_copy
* purpose: deep copy nodes from list B to tail of list A, list B is preserved
* @ A : pointer to struct dll
* @ B : pointer to struct dll
* returns: pointer to first new node in list A, NULL if copy failed.
* note: if returned null, list A reverted to state prior to function call
*******************************************************************************/
struct dll_node *dll_copy(struct dll *A, struct dll *B);

//macros

/*******************************************************************************
* macro: dll_size
* purpose: return total number of nodes currently in list
*******************************************************************************/
#define dll_size(list) list->size

/*******************************************************************************
* macro: push/pop/peek + head/tail
* purpose: insertion, removal, and access macros for head and tail 
*******************************************************************************/
#define dll_push_head(list, datum) dll_insert_pos(list, 0, datum)
#define dll_pop_head(list) dll_remove_pos(list, 0)
#define dll_peek_head(list) dll_access_pos(list, 0)

#define dll_push_tail(list, datum) dll_insert_pos(list, list->size, datum)
#define dll_pop_tail(list) dll_remove_pos(list, list->size - 1)
#define dll_peek_tail(list) dll_access_pos(list, list->size - 1)

#endif