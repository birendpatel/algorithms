/*
* Author: Biren Patel
* Description: API for a singly linked list
*/

#ifndef SLL_H
#define SLL_H

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
* client-modifiable parameters
* @ sll_item : the data type of items contained in the list
*******************************************************************************/
typedef void* sll_item;

/*******************************************************************************
* structure: node
* purpose: describes a single node within the list
* @ datum : the item stored at this node
* @ next : pointer to the next node in the list
*******************************************************************************/
struct node
{
    sll_item datum;
    struct node *next;
};

/*******************************************************************************
* structure: sll
* purpose: client must declare pointer to this struct to access the API
* @ destroy : pointer to function used to clean up memory on destructor call
* @ head : pointer to the first node in the list
* @ size : number of nodes in list
* @ has_type_1_concat : flag that this list has been concatenated with type 1
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
    bool has_type_1_concat;
};

/*******************************************************************************
* public function: sll_create
* purpose: constructor
* @ destroy : pointer to function used to clean up memory on destructor call
* returns: pointer to struct sll, or NULL on failure
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
* @ datum : the piece of data to store at the new node
* returns: pointer to newly created struct node, or NULL on failure
*******************************************************************************/
struct node *sll_insert_idx(struct sll *s, uint32_t idx, sll_item datum);

/*******************************************************************************
* public function: sll_remove_idx
* purpose: remove a node at the specified index
* @ s : pointer to struct sll
* @ idx : index of removal, where 0 is the head
* returns: data which was stored at the removed node
*******************************************************************************/
sll_item sll_remove_idx(struct sll *s, uint32_t idx);

/*******************************************************************************
* public function: sll_access_idx
* purpose: sequential access of node at the specified index
* @ s : pointer to struct sll
* @ idx : index of access, where 0 is the head
* returns: pointer to struct node at specified index
*******************************************************************************/
struct node *sll_access_idx(struct sll *s, uint32_t idx);

/*******************************************************************************
* public function: sll_search_node
* purpose: sequential search for first node matching input node
* @ s : pointer to struct sll
* @ search_node : pointer to struct node
* returns: true if node in list, else false.
* note: probably wouldnt use this unless you need some sanity checks after lots
*       of type 1 sll_concat calls on many linked lists. 
*******************************************************************************/
bool sll_search_node(struct sll *s, struct node *search_node);

/*******************************************************************************
* public function: sll_search_data
* purpose: sequential search for first node containing specified datum
* @ s : pointer to struct sll
* @ datum : the piece of data to search for
* returns: pointer to struct node containing data, or null if not found
*******************************************************************************/
struct node *sll_search_data(struct sll *s, sll_item datum);

/*******************************************************************************
* public function: sll_concat
* purpose: concatate 'from' with 'to', the tail of 'to' points to head of 'from'
*
* @ to : pointer to struct sll, the list which gains the nodes
* @ from : pointer to struct sll, the list which transfers its nodes
* @ method : 0 - the 'from' list loses nodes and becomes an empty list.
*            1 - the 'from' list retains all of its nodes.
*            2 - the 'to' list gains a copy of all nodes in 'from'
* @ destroy : if method 2 fails on malloc, then copied sll_items may need to be
*             returned to memory. If not, pass null.
*
* returns: pointer to first new node on 'to', or NULL if malloc fails
*
* note: method type 1 needs care to call sll_destroy on 'from' before 'to'
* note: if null returned, then lists are reverted to original state before call
*******************************************************************************/
struct node *sll_concat
(
    struct sll *to, 
    struct sll *from, 
    char method, 
    void (*destroy)(void *data)
);

/*******************************************************************************
* macro: sll_size
* purpose: convenient access to size member in struct sll
* @ s : pointer to struct sll
* returns: unsigned 32 bit int of total nodes currently in list
*******************************************************************************/
#define sll_size(s) (s->size)

/*******************************************************************************
* macros: *_head / *_tail
* purpose: head and tail macro wrappers over _idx public functions
* @ s : pointer to struct sll
* @datum : the piece of data to store at the new node
*******************************************************************************/
#define sll_insert_head(s, datum) sll_insert_idx(s, 0, datum)
#define sll_insert_tail(s, datum) sll_insert_idx(s, s->size, datum)

#define sll_remove_head(s) sll_remove_idx(s, 0)
#define sll_remove_tail(s) sll_remove_idx(s, s->size - 1)

#define sll_access_head(s) sll_access_idx(s, 0)
#define sll_access_tail(s) sll_access_idx(s, s->size - 1)

#define sll_access_head_data(s) sll_access_idx(s, 0)->datum
#define sll_access_tail_data(s) sll_access_idx(s, s->size - 1)->datum

#endif