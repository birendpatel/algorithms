/*
* Author: Biren Patel
* Description: Header-only API for bit array data structures. 
*/

#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

//API works on standard C arrays but this provides a little self-documentation
#define u64_bitarray(x)
#define u32_bitarray(x)
#define u16_bitarray(x)
#define u8_bitarray(x)

/*******************************************************************************
* NAME: bitarray_fetch_block
* DESC: get the block containing the bit index
* OUTP: array block aka the array element containing the requested bit
* @ x : array
* @ k : bit index
*******************************************************************************/
#define u64_bitarray_fetch_block(x, k) x[(k)/64]
#define u32_bitarray_fetch_block(x, k) x[(k)/32]
#define u16_bitarray_fetch_block(x, k) x[(k)/16]
#define u8_bitarray_fetch_block(x, k) x[(k)/8]

/*******************************************************************************
* NAME: bitarray_fetch_pos
* DESC: get the bit position of the bit index within its respective block
* OUTP: bit position not exceeding n
* @ x : array
* @ k : bit index
*******************************************************************************/   
#define u64_bitarray_fetch_pos(k) ((k) % 64)
#define u32_bitarray_fetch_pos(k) ((k) % 32)
#define u16_bitarray_fetch_pos(k) ((k) % 16)
#define u8_bitarray_fetch_pos(k) ((k) % 8)

/*******************************************************************************
* NAME: bitarray_test
* DESC: test if the bit at the requested index is set to one
* OUTP: zero/one true/false
* @ x : array
* @ k : bit index
*******************************************************************************/          
#define u64_bitarray_test(x, k)                                                \
        (u64_bitarray_fetch_block(x, k) & (1ULL << u64_bitarray_fetch_pos(k))) \
                  
#define u32_bitarray_test(x, k)                                                \
        (u32_bitarray_fetch_block(x, k) & (1UL  << u32_bitarray_fetch_pos(k))) \
                  
#define u16_bitarray_test(x, k)                                                \
        (u16_bitarray_fetch_block(x, k) & (1U << u16_bitarray_fetch_pos(k)))   \
                  
#define u8_bitarray_test(x, k)                                                 \
        (u8_bitarray_fetch_block(x, k) & (1U << u8_bitarray_fetch_pos(k)))     \

/*******************************************************************************
* NAME: bitarray_set
* DESC: set the bit at the requested index to one
* OUTP: evaluated for assignment side effect
* @ x : array
* @ k : bit index
*******************************************************************************/         
#define u64_bitarray_set(x, k)                                                 \
        u64_bitarray_fetch_block(x, k) |= (1ULL << u64_bitarray_fetch_pos(k))  \
                 
#define u32_bitarray_set(x, k)                                                 \
        u32_bitarray_fetch_block(x, k) |= (1UL << u32_bitarray_fetch_pos(k))   \
                 
#define u16_bitarray_set(x, k)                                                 \
        u16_bitarray_fetch_block(x, k) |= (1U << u16_bitarray_fetch_pos(k))    \
                 
#define u8_bitarray_set(x, k)                                                  \
        u8_bitarray_fetch_block(x, k) |= (1U << u8_bitarray_fetch_pos(k))      \

/*******************************************************************************
* NAME: bitarray_clear
* DESC: set the bit at the requested index to zero
* OUTP: evaluated for assignment side effect
* @ x : array
* @ k : bit index
*******************************************************************************/         
#define u64_bitarray_clear(x, k)                                               \
        u64_bitarray_fetch_block(x, k) &= ~(1ULL << u64_bitarray_fetch_pos(k)) \
                
#define u32_bitarray_clear(x, k)                                               \
        u32_bitarray_fetch_block(x, k) &= ~(1UL << u32_bitarray_fetch_pos(k))  \
                
#define u16_bitarray_clear(x, k)                                               \
        u16_bitarray_fetch_block(x, k) &= ~(1U << u16_bitarray_fetch_pos(k))   \
                
#define u8_bitarray_clear(x, k)                                                \
        u8_bitarray_fetch_block(x, k) &= ~(1U << u8_bitarray_fetch_pos(k))     \

/*******************************************************************************
* NAME: bitarray_get
* DESC: obtain the value of the bit at the requested index
* OUTP: zero or one
* @ x : array
* @ k : bit index
*******************************************************************************/         
#define u64_bitarray_get(x, k)                                                 \
        ((u64_bitarray_fetch_block(x, k) >> u64_bitarray_fetch_pos(k)) & 1)    \
                 
#define u32_bitarray_get(x, k)                                                 \
        ((u32_bitarray_fetch_block(x, k) >> u32_bitarray_fetch_pos(k)) & 1)    \
                 
#define u16_bitarray_get(x, k)                                                 \
        ((u16_bitarray_fetch_block(x, k) >> u16_bitarray_fetch_pos(k)) & 1)    \
                 
#define u8_bitarray_get(x, k)                                                  \
        ((u8_bitarray_fetch_block(x, k) >> u8_bitarray_fetch_pos(k)) & 1)      \

/*******************************************************************************
* NAME: bitarray_mask
* DESC: apply a bitmask to the block containing the bit index
* OUTP: value of masked block
* @ x : array
* @ k : bit index
*******************************************************************************/         
#define u64_bitarray_mask(x, k, mask)                                          \
        (u64_bitarray_fetch_block(x, k) & mask)                                \
                
#define u32_bitarray_mask(x, k, mask)                                          \
        (u32_bitarray_fetch_block(x, k) & mask)                                \
                
#define u16_bitarray_mask(x, k, mask)                                          \
        (u16_bitarray_fetch_block(x, k) & mask)                                \
                
#define u8_bitarray_mask(x, k, mask)                                           \
        (u8_bitarray_fetch_block(x, k) & mask)                                 \

/*******************************************************************************
* NAME: bitarray_mask_at
* DESC: apply a bitmask to the containing block from the bit index onwards
* OUTP: value of masked block from bit index onwards
* @ x : array
* @ k : bit index
*******************************************************************************/         
#define u64_bitarray_mask_at(x, k, mask)                                       \
        ((u64_bitarray_fetch_block(x, k) >> u64_bitarray_fetch_pos(k)) & mask) \
                
#define u32_bitarray_mask_at(x, k, mask)                                       \
        ((u32_bitarray_fetch_block(x, k) >> u32_bitarray_fetch_pos(k)) & mask) \
                
#define u16_bitarray_mask_at(x, k, mask)                                       \
        ((u16_bitarray_fetch_block(x, k) >> u16_bitarray_fetch_pos(k)) & mask) \
                
#define u8_bitarray_mask_at(x, k, mask)                                        \
        ((u8_bitarray_fetch_block(x, k) >> u8_bitarray_fetch_pos(k)) & mask)   \

#endif