/*
* Author: Biren Patel
* Description: Memory pool implementation.
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

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
        assert((delta >= 0 || delta <= (char) ALIGN) && "delta exceeds gap");
        
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
    struct block *block = (struct block *) ((char*) ptr - SIZEOF_BLOCK);
    
    //psuedo-free, can't return to system so instead mark the block for reuse
    block->available = 1;
    
    //todo: join neighbor blocks
    //1. try to combine this with previous block
    //2. try to combine this potentially new block with the next block
    
    //notify manager
    if (manager.max_free < block->size) manager.max_free = block->size;
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
//display the memory contents of the pool
    
#define memmap_manager()                                                       \
        do                                                                     \
        {                                                                      \
            printf("\nHead: 0x%p\n", (void*) manager.head);                    \
            printf("Tail: 0x%p\n", (void*) manager.tail);                      \
            printf("Pool: 0x%p\n", (void*) manager.pool);                      \
            printf("Top:  0x%p\n", (void*) manager.top);                       \
            printf("Max Free:  %llu\n", manager.max_free);                     \
            printf("Available: %llu\n", manager.available);                    \
        }                                                                      \
        while (0)                                                              \


#define memmap_header()                                                        \
        do                                                                     \
        {                                                                      \
            printf("\n");                                                      \
            printf("%5sAddress%13sStorage%18sValue\n", " ", " ", " ");         \
            printf("------------------\t---------\t----------------------\n"); \
        } while(0)                                                             \


//an address in the memory pool can have 6 different interpretations,
//4 of which relate to the struct block members
#define BLOCK_PREV_FMT "0x%p      [B] prev        0x%p       \n"
#define BLOCK_NEXT_FMT "0x%p      [B] next        0x%p       \n"
#define BLOCK_SIZE_FMT "0x%p      [B] size        %llu       \n"
#define BLOCK_FLAG_FMT "0x%p      [B] flag        Available = %d, Gap = %d\n"
#define BLOCK_USER_FMT "0x%p      [U]             "
#define BLOCK_NONE_FMT "0x%p      [N]                        \n"
     
void memmap(size_t words)
{
    //display memory map header
    memmap_manager();
    memmap_header();
    
    uintptr_t curr = (uintptr_t) manager.pool;
    uintptr_t end = (uintptr_t) manager.pool + words * 8;
    
    struct block *block = manager.head;
    
    while (curr <= end)
    {
        if (curr == (uintptr_t) block)
        {   
            //curr is at block node so an unrolled loop prints next 4 words
            printf(BLOCK_PREV_FMT, (void*) curr, (void*) block->prev);
            curr += 8;
            
            printf(BLOCK_NEXT_FMT, (void*) curr, (void*) block->next);
            curr += 8;
            
            printf(BLOCK_SIZE_FMT, (void*) curr, block->size);
            curr += 8;
            
            printf(BLOCK_FLAG_FMT, (void*) curr, block->available, block->top_gap);
            curr += 8;
            
            //and then print the remaining user blocks using char values
            size_t user_words = block->size/8 + (block->size % 8 != 0); //ceil
            size_t byte_cnt = block->size;
            
            for (size_t i = 0; i < user_words; ++i)
            {
                printf(BLOCK_USER_FMT, (void*) curr);
                
                char *byte = (char*) curr;
                
                for (size_t j = 0; j < 8; ++j, ++byte)
                {
                    if (*byte != '\0') 
                    {
                        if (*byte > 32 && *byte < 127) printf("%-3c", *byte);
                        else printf("?  ");
                    }
                    else printf(".  ");
                    
                    //user memory doesn't occupy the entire word so need break.
                    //the bytes from here onwards are the top gaps between the
                    //pool manager block nodes.
                    if (--byte_cnt == 0) break;
                }
                
                printf("\n");
                
                curr += 8;
            }
            
            printf("\n");
            
            block = block->next;
        }
        else
        {
            //this section of memory is not in use
            printf(BLOCK_NONE_FMT, (void*) curr);
            curr += 8;
        }
    }
    
    return;
}

/******************************************************************************/

int main(void)
{
    mempool_init(1024);
    
    char *x = pcalloc(24, 1);
    pcalloc(16, 1);
    pcalloc(32, 1);
    
    pfree(x);
    
    memmap(32);

    mempool_free();
    
    return 0;
}