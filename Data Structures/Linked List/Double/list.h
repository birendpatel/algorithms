/*
* Author: Biren Patel
* Description: Generic doubly linked list API via void pointers
*/

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

/*******************************************************************************
* struct: list_node
* purpose: list node returned on some functions, can be used as later input to
* nodal functions for O(1) operations.
* @ prev : pointer to the previous node, NULL if at head node
* @ next : pointer to the next node, NULL if at tail node
* @ data : the piece of data stored in each node
*******************************************************************************/
typedef struct list_node
{
    struct list_node *prev;
    struct list_node *next;
    void *data;
} *Node;

/*******************************************************************************
* struct: list
* purpose: The list itself, composed of nodes and metadata. The client should
* first declare a pointer to this list structure in order to access the API.
* @ destroy : pointer to function for void * destruction, else NULL
* @ head : pointer to the first node in the list
* @ tail : pointer to the final node in the list
* @ size : the total number of nodes in the list
*******************************************************************************/
typedef struct list
{
    void (*destroy)(void *data);
    struct list_node *head;
    struct list_node *tail;
    int size;
} *List;

//constructors

/*******************************************************************************
* public function: list_create
* purpose: constructor
* @ destroy : pointer to function for void * destruction, else NULL
* returns: pointer to struct list
*******************************************************************************/
struct list *list_create(void (*destroy)(void *data));

/*******************************************************************************
* public function: list_destroy
* purpose: destructor, calls destroy passed on constructor unless NULL
* @ list : pointer to struct list
*******************************************************************************/
void list_destroy(struct list *list);

//positional functions

/*******************************************************************************
* public function: list_insert_pos
* purpose: insert a new node at the specified position
* @ list : pointer to struct list
* @ pos : position of insertion, equal to size of list for tail entry
* @ data : the piece of data to store in the new node
* returns: pointer to the new node if successful, else NULL
*******************************************************************************/
struct list_node *list_insert_pos(struct list *list, int pos, void *data);

/*******************************************************************************
* public function: list_remove_pos
* purpose: remove a node at the specified position
* @ list : pointer to struct list
* @ pos : position of removal, equal to size of list - 1 for tail removal
* returns: data stored at removed node
*******************************************************************************/
void *list_remove_pos(struct list *list, int pos);

/*******************************************************************************
* public function: list_access_pos
* purpose: peek data in node at specified position
* @ list : pointer to struct list
* @ pos : position of access, with negative indexing available
* returns : item at node, of type void *.
*******************************************************************************/
void *list_access_pos(struct list *list, int pos);

//nodal functions

/*******************************************************************************
* public function: list_insert_node
* purpose: insert a node after or before the input node
* @ list : pointer to struct list
* @ node : pointer to node after or before which to insert
* @ data : the piece of data stored at the new node
* @ method : 1 for insert after, 2 for insert before
* returns : pointer to the new node if successful, else NULL
*******************************************************************************/
struct list_node *list_insert_node
(
    struct list *list,
    struct list_node *node,
    void *data,
    char method
);

/*******************************************************************************
* public function: list_remove_node
* purpose: remove the input node
* @ list : pointer to struct list
* @ node : pointer to node requiring removal, NULL only if method is 1 or 2
* @ method : removal position, 0 if removing at middle, 1 if tail, 2 if head
* returns: data stored at removed node
*******************************************************************************/
void *list_remove_node(struct list *list, struct list_node *node, char method);

//utilities

/*******************************************************************************
* public function: list_search_node
* purpose: search for a node within the list
* @ list : pointer to struct list
* @ node : search criterion
* @ method : 1 to begin search at head, 2 to begin at tail
* @ cmp : comparison function for ndoe data, null for == check, returning 0 if 
*         no match. The argument checking is bypassed for more flexibility.
* returns: true if found, false otherwise
*******************************************************************************/
bool list_search_node
(
    struct list *list,
    struct list_node *node,
    char method,
    int (*cmp)()
);

/*******************************************************************************
* public function: list_search
* purpose: search for data within the list
* @ list : pointer to struct list
* @ data : search criterion
* @ method : 1 to begin search at head, 2 to begin at tail.
* @ cmp : comparison function, null for == check, returning 0 if no match. The
*         argument checking is bypassed for more flexibility.
* returns: the first list_node containing the data, null if not found
*******************************************************************************/
struct list_node *list_search
(
    struct list *list,
    void *data,
    char method,
    int (*cmp)()
);

/*******************************************************************************
* public function: list_concat
* purpose: concatenate nodes from list B to tail of list A, list B becomes empty
* @ A : pointer to struct list
* @ B : pointer to struct list
* returns: pointer to first new node in list A
*******************************************************************************/
struct list_node *list_concat(struct list *A, struct list *B);

/*******************************************************************************
* public function: list_copy
* purpose: deep copy nodes from list B to tail of list A, list B is preserved
* @ A : pointer to struct list
* @ B : pointer to struct list
* returns: pointer to first new node in list A, NULL if copy failed.
* note: if returned null, list A reverted to state prior to function call
*******************************************************************************/
struct list_node *list_copy(struct list *A, struct list *B);

/*******************************************************************************
* public function: list_size
* purpose: size of list
* @ list : point to struct list
* returns: int number of nodes
*******************************************************************************/
int list_size(struct list *list);

/*******************************************************************************
* macro: push/pop/peek + head/tail
* purpose: insertion, removal, and access macros for head and tail
*******************************************************************************/
#define list_push_head(list, data) list_insert_node(list, NULL, data, 2)
#define list_push_tail(list, data) list_insert_node(list, NULL, data, 1)

#define list_pop_head(list) list_remove_node(list, NULL, 2)
#define list_pop_tail(list) list_remove_node(list, NULL, 1)

#define list_peek_head(list) list_access_pos(list, 0)
#define list_peek_tail(list) list_access_pos(list, -1)

#endif