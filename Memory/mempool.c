/*
* Author: Biren Patel
* Description: Memory pool implementation.
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mempool.h"

/*******************************************************************************
* struct: block
* purpose: doubly linked list node hidden before requested memory blocks
* @ size : the byte-size of the memory block handed back to the user
* @ available : flag where 1 indiciates that the block is free
* @ top_gap : bytes between the start of this block and end of the last block
* note: in total, if a user requests an x-byte block, x + top_gap + 32 bytes
*       are reserved, but only x + 32 bytes are actually used.
*
*
* struct: manager
* purpose: doubly linked list and memory pool manager
* @ top : points to first open and aligned byte after the last occupied block
* @ max_free : block.size of the largest available block
* @ available : total bytes available outside of blocks (from *top onward)
*
*
*
*  pool start                                    top pointer
*     |                                               |
*     |                                               |
*     V                                               V
*
*     #-------------------#-----#--------------------#------------------------#
*     # block + user data #     # block + user data  #                        #
*     #-------------------#-----#--------------------#------------------------#
*    
*     \__________________/\_____/                     \______________________/
*              |             \                                available
*              |              \
*              |               \_________ gap between blocks
*              |                          to ensure alignment
*   block may be free, the size 
*   of the largest free block
*   is stored in max_free member
*   
*******************************************************************************/

#define SIZEOF_BLOCK 32

struct block
{
    struct block *prev;
    struct block *next;
    size_t size;   
    char available;
    char top_gap;
    char reserved[6];
};


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
//prototypes and general macros

//using 8-byte alignment to handle most types.
//0x7 instead of 0x8 b/c alignment is just used to round down manager.top
#define ALIGN ((uintptr_t) 0x7)
#define ALIGN_MASK (~ ((uintptr_t) 0x7))

static void insert_node_at_tail(struct block *new);

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
    
    //try to repurpose an available block
    if (size <= manager.max_free)
    {
        return NULL;
    }
    //otherwise use a new section of the pool
    else if (size + ALIGN + SIZEOF_BLOCK <= manager.available)
    {
        //align top pointer to somewhere between old top and ALIGN-bytes above
        uintptr_t old_top = (uintptr_t) manager.top;
        manager.top = (void*) (((uintptr_t) manager.top + ALIGN) & ALIGN_MASK);
        
        char delta = (char) ((uintptr_t) manager.top - old_top);
        assert((delta >= 0 || delta <= ALIGN) && "delta exceeds allowed gap");
        
        //place a new block node at top and add to tail of linked list
        struct block *new = manager.top;
        
        new->size = size;
        new->available = false;
        new->top_gap = delta;
        insert_node_at_tail(new);
        
        //update manager, move top a la sbrk() and note total bytes used
        manager.available -= SIZEOF_BLOCK + size + delta;
        manager.top = (char*) manager.top + (SIZEOF_BLOCK + size);
        
        //hide the metadata and return 32 bytes above for user to use
        assert((uintptr_t) new % 8 == 0 && "struct block not aligned");
        assert((uintptr_t) (new + 1) % 8 == 0 && "user block not aligned");
        
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
    //todo: check for overflow before passing
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
    struct block *meta = (struct block *) ((char*) ptr - SIZEOF_BLOCK);
    
    //psuedo-free, don't return to system just mark the block for reuse
    meta->available = 1;
    
    //todo: join neighbor blocks
    //1. try to combine this with previous block
    //2. try to combine this potentially new block with the next block
    
    //notify manager
    if (manager.max_free < meta->size) manager.max_free = meta->size;
}

/******************************************************************************/

static void insert_node_at_tail(struct block *new)
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
    mempool_init(1000);
    
    pmalloc(18);
    
    pcalloc(18, 1);
    
    mempool_free();
    
    return 0;
}