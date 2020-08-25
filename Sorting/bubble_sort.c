/*
* Author: Biren Patel
* Description: Generic bubble sort algorithm
*/

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "src/unity.h"

/******************************************************************************/

void bbsort
(
    void *base,
    size_t nitems,
    size_t size,
    int (*cmp)(const void *, const void *)
)
{
    //silent return on user misuse
    if (base == NULL || cmp == NULL || nitems <= 1) return;
    
    //VLA used for temporary storage on swaps
    char tmp[size];
    
    //to access pointer arithmetic
    char *cbase = (char*) base;
    
    //at each iter perform swaps on the first i + 1 items
    //previous iteration guarantees i + 2 onwards is already sorted
    for (size_t i = nitems - 1; i > 0; --i)
    {
        for (size_t j = 0; j < i; ++j)
        {
            //load items at base[j] and base[j+1]
            char *item_1 = cbase + j * size;
            char *item_2 = cbase + (j + 1) * size;
            
            //if base[j] is greater then perform memory swap
            if (cmp(item_1, item_2) > 0)
            {
                memcpy(tmp, item_2, size);
                memcpy(item_2, item_1, size);
                memcpy(item_1, tmp, size);
            }
        }
    }
    
    return;
}

/******************************************************************************/
//the rest of the file is unit testing via the Unity Framework

static inline int cmp(const void *a, const void *b)
{
    return *(int*) a - *(int*) b;
}

void setUp(void) {}
void tearDown(void) {}

#define N 10000

void test_insertion_sort_on_random_integers(void)
{
    //arrange
    int *data = malloc(sizeof(int) * N);
    assert(data != NULL);
    for (size_t i = 0; i < N - 1; ++i) data[i] = rand();
    data[N-1] = 0;
    
    //act
    bbsort(data, N, 4, cmp);
    
    //assert
    for (size_t i = 0; i < N - 1; ++i) TEST_ASSERT_TRUE(data[i] <= data[i + 1]);
    
    //clean
    free(data);
}

int main(void)
{
    UNITY_BEGIN();
        RUN_TEST(test_insertion_sort_on_random_integers);
    UNITY_END();
}