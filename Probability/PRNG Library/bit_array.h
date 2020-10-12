/*
* Author: Biren Patel
* Description: Header-only API for bit array data structures. 
*/

#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

//API works on standard C arrays but this provides a little self-documentation
#define bitarray(x)

//Same as above but convenient for casting out of void *
#define bitarray_init(fx, x, type)                                             \
        type fx = (type) x                                                     \

/*******************************************************************************
* NAME: bitarray_fetch_block
* DESC: get the block containing the bit index
* OUTP: array block aka the array element containing the requested bit
* @ x : array
* @ n : array element bit size
* @ k : bit index
*******************************************************************************/
#define bitarray_fetch_block(x, n, k)                                          \
        x[(k)/(n)]                                                             \

/*******************************************************************************
* NAME: bitarray_fetch_pos
* DESC: get the bit position of the bit index within its respective block
* OUTP: bit position not exceeding n
* @ x : array
* @ n : array element bit size
* @ k : bit index
*******************************************************************************/   
#define bitarray_fetch_pos(n, k)                                               \
        ((k) % (n))                                                            \

/*******************************************************************************
* NAME: bitarray_test
* DESC: test if the bit at the requested index is set to one
* OUTP: zero/one true/false
* @ x : array
* @ n : array element bit size
* @ k : bit index
*******************************************************************************/          
#define bitarray_test(x, n, k)                                                 \
        (bitarray_fetch_block(x, n, k) & (1ULL << bitarray_fetch_pos(n, k)))   \

/*******************************************************************************
* NAME: bitarray_set
* DESC: set the bit at the requested index to one
* OUTP: evaluated for assignment side effect
* @ x : array
* @ n : array element bit size
* @ k : bit index
*******************************************************************************/         
#define bitarray_set(x, n, k)                                                  \
        bitarray_fetch_block(x, n, k) |= (1ULL << bitarray_fetch_pos(n, k))    \

/*******************************************************************************
* NAME: bitarray_clear
* DESC: set the bit at the requested index to zero
* OUTP: evaluated for assignment side effect
* @ x : array
* @ n : array element bit size
* @ k : bit index
*******************************************************************************/         
#define bitarray_clear(x, n, k)                                                \
        bitarray_fetch_block(x, n, k) &= ~(1ULL << bitarray_fetch_pos(n, k))   \

/*******************************************************************************
* NAME: bitarray_get
* DESC: obtain the value of the bit at the requested index
* OUTP: zero or one
* @ x : array
* @ n : array element bit size
* @ k : bit index
*******************************************************************************/         
#define bitarray_get(x, n, k)                                                  \
        ((bitarray_fetch_block(x, n, k) >> bitarray_fetch_pos(n, k)) & 1)      \

/*******************************************************************************
* NAME: bitarray_mask
* DESC: apply a bitmask to the block containing the bit index
* OUTP: value of masked block
* @ x : array
* @ n : array element bit size
* @ k : bit index
*******************************************************************************/         
#define bitarray_mask(x, n, k, mask)                                           \
        (bitarray_fetch_block(x, n, k) & mask)                                 \

/*******************************************************************************
* NAME: bitarray_mask
* DESC: apply a bitmask to the containing block from the bit index onwards
* OUTP: value of masked block from bit index onwards
* @ x : array
* @ n : array element bit size
* @ k : bit index
*******************************************************************************/         
#define bitarray_mask_at(x, n, k, mask)                                        \
        ((bitarray_fetch_block(x, n, k) >> bitarray_fetch_pos(n, k)) & mask)   \

#endif
