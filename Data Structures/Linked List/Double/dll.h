/*
* Author: Biren Patel
* Description: Generic doubly linked list API via void pointers
*/

#ifndef DLL_H
#define DLL_H

#include <stdbool.h>

/*******************************************************************************
* struct: dll_node
* purpose: list node returned on some functions, can be used as later input to
* nodal functions for O(1) operations.
* @ prev : pointer to the previous node, NULL if at head node
* @ next : pointer to the next node, NULL if at tail node
* @ data : the piece of data stored in each node
*******************************************************************************/
typedef struct dll_node
{
    struct dll_node *prev;
    struct dll_node *next;
    void *data;
} *Node;

/*******************************************************************************
* struct: dll
* purpose: The list itself, composed of nodes and metadata. The client should
* first declare a pointer to this list structure in order to access the API.
* @ destroy : pointer to function for void * destruction, else NULL
* @ head : pointer to the first node in the list
* @ tail : pointer to the final node in the list
* @ size : the total number of nodes in the list
*******************************************************************************/
typedef struct dll
{
    void (*destroy)(void *data);
    struct dll_node *head;
    struct dll_node *tail;
    int size;
} *List;

//constructors

/*******************************************************************************
* public function: dll_create
* purpose: constructor
* @ destroy : pointer to function for void * destruction, else NULL
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
* @ data : the piece of data to store in the new node
* returns: pointer to the new node if successful, else NULL
*******************************************************************************/
struct dll_node *dll_insert_pos(struct dll *list, int pos, void *data);

/*******************************************************************************
* public function: dll_remove_pos
* purpose: remove a node at the specified position
* @ list : pointer to struct dll
* @ pos : position of removal, equal to size of list - 1 for tail removal
* returns: data stored at removed node
*******************************************************************************/
void *dll_remove_pos(struct dll *list, int pos);

/*******************************************************************************
* public function: dll_access_pos
* purpose: peek data in node at specified position
* @ list : pointer to struct dll
* @ pos : position of access, with negative indexing available
* returns : item at node, of type void *.
*******************************************************************************/
void *dll_access_pos(struct dll *list, int pos);

//nodal functions

/*******************************************************************************
* public function: dll_insert_node
* purpose: insert a node after or before the input node
* @ list : pointer to struct dll
* @ node : pointer to node after or before which to insert
* @ data : the piece of data stored at the new node
* @ method : 1 for insert after, 2 for insert before
* returns : pointer to the new node if successful, else NULL
*******************************************************************************/
struct dll_node *dll_insert_node
(
    struct dll *list,
    struct dll_node *node,
    void *data,
    char method
);

/*******************************************************************************
* public function: dll_remove_node
* purpose: remove the input node
* @ list : pointer to struct dll
* @ node : pointer to node requiring removal, NULL only if method is 1 or 2
* @ method : removal position, 0 if removing at middle, 1 if tail, 2 if head
* returns: data stored at removed node
*******************************************************************************/
void *dll_remove_node(struct dll *list, struct dll_node *node, char method);

//utilities

/*******************************************************************************
* public function: dll_search_node
* purpose: search for a node within the list
* @ list : pointer to struct dll
* @ node : search criterion
* @ method : 1 to begin search at head, 2 to begin at tail
* @ returns: true if found, false otherwise
*******************************************************************************/
bool dll_search_node(struct dll *list, struct dll_node *node, char method);

/*******************************************************************************
* public function: dll_search
* purpose: search for data within the list
* @ list : pointer to struct dll
* @ data : search criterion
* @ method : 1 to begin search at head, 2 to begin at tail.
* returns: the first dll_node containing the data, null if not found
*******************************************************************************/
struct dll_node *dll_search(struct dll *list, void *data, char method);

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

/*******************************************************************************
* public function: dll_size
* purpose: size of list
* @ list : point to struct dll
* returns: int number of nodes
*******************************************************************************/
int dll_size(struct dll *list);

/*******************************************************************************
* macro: push/pop/peek + head/tail
* purpose: insertion, removal, and access macros for head and tail
*******************************************************************************/
#define dll_push_head(list, data) dll_insert_node(list, NULL, data, 2)
#define dll_push_tail(list, data) dll_insert_node(list, NULL, data, 1)

#define dll_pop_head(list) dll_remove_node(list, NULL, 2)
#define dll_pop_tail(list) dll_remove_node(list, NULL, 1)

#define dll_peek_head(list) dll_access_pos(list, 0)
#define dll_peek_tail(list) dll_access_pos(list, -1)

#endif