/*
* Author: Biren Patel
* Description: Memory pool implementation.
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mempool.h"

/*******************************************************************************
* struct: block
* purpose: doubly linked list node hidden before requested memory blocks
* note: in total, if a user requests an x-byte block, x + 32 bytes are reserved
*******************************************************************************/

#define SIZEOF_BLOCK 32

struct block
{
    struct block *prev;
    struct block *next;
    size_t size;   
    char available;
    char reserved[7];
};

/*******************************************************************************
* struct: manager
* purpose: doubly linked list and memory pool manager
* @ top : points to the first available byte after the last occupied block
* @ max_free : block.size of the largest available block
* @ available : total bytes available outside of blocks (from *top onward)
*
*
*
*  pool start                              top pointer
*     |                                        |
*     |                                        |
*     V                                        V
*
*     #-------------------#-------------------#-------------------------------#
*     # block + user data # block + user data #                               #
*     #-------------------#-------------------#-------------------------------#
*    
*     \__________________/                     \_____________________________/
*              |                                           available
*              |
*              |
*   block may be free, the size 
*   of the largest free block
*   is stored in max_free member
*   
*******************************************************************************/

static struct
{
    struct block *head;
    struct block *tail;
    size_t max_free;
    size_t available;
    void *pool;
    void *top;
} 
manager = 
{
    .head = NULL,
    .tail = NULL,
    .max_free = 0,
    .available = 0,
    .pool = NULL,
    .top = NULL
};

/******************************************************************************/
//prototypes

static void insert_node(struct block *new);

/******************************************************************************/

bool mempool_init(size_t size)
{
    if (manager.pool != NULL || size == 0)
    {
        return false;
    }
    else
    {
        manager.pool = malloc(size);
    
        if (manager.pool == NULL)
        {
            return false;
        }
        else
        {
            manager.top = manager.pool;
            manager.available = size;
            return true;            
        }
    }
}

/******************************************************************************/

bool mempool_free(void)
{
    if (manager.pool == NULL)
    {
        return false;
    }
    else
    {
        free(manager.pool);
        
        //reset pool manager in case of future init
        manager.head = NULL;
        manager.tail = NULL;
        manager.max_free = 0;
        manager.available = 0;
        manager.pool = NULL;
        manager.top = NULL;
        
        return true;
    }
}

/******************************************************************************/
//functionality same as stdlib malloc just acting on manager.pool instead

void *pmalloc(size_t size)
{
    if (size == 0) return NULL;
    
    //repurpose an available block else use a new section of the pool
    if (size <= manager.max_free)
    {
        return NULL;
    }
    else if (size <= manager.available - 32)
    {
        //place a new list node at the top of the pool
        struct block *new = manager.top;
        new->size = size;
        new->available = false;
        
        //add node references
        insert_node(new);
        
        //update manager pool metadata
        manager.available -= SIZEOF_BLOCK + size;
        manager.top = (char*) manager.top + (SIZEOF_BLOCK + size);
        
        //hide the metadata and return 32 bytes above for user to use
        return new + 1;
    }
    else
    {
        //not enough memory at top to accomodate list node + user block
        return NULL;
    }
}

/******************************************************************************/

void *pcalloc(size_t n, size_t size)
{
    void *address = pmalloc(n * size);
    
    if (address == NULL)
    {
        return NULL;
    }
    else
    {
        memset(address, 0, n * size);
        return address;
    }
}

/******************************************************************************/

void pfree(void *ptr)
{
    if (ptr == NULL) return;
    
    //container of ptr
    struct block *meta = (char*) ptr - SIZEOF_BLOCK;
    
    //psuedo-free, don't return to system just mark the block for reuse
    meta->available = 1;
    
    //notify manager
    if (manager.max_free < meta->size) manager.max_free = meta->size;
    
    //could do some funky stuff here
    //try to join with neighbor blocks if they are free
    //or send the node to the head for faster walk on next malloc
}

/******************************************************************************/
//insert a block node into the tail of the linked list

static void insert_node(struct block *new)
{
    if (manager.head == NULL)
    {
        //insert into empty list
        manager.head = new;
        manager.tail = new;
        new->prev = NULL;
        new->next = NULL;
    }
    else
    {
        //insert at tail
        new->prev = manager.tail;
        new->next = NULL;
        manager.tail->next = new;
        manager.tail = new;
    }
}
/******************************************************************************/

#include <stdio.h>

int main(void)
{
    mempool_init(100);
    
    pmalloc(18);
    
    pcalloc(18, 1);
    
    mempool_free();
    
    return 0;
}