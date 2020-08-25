/*
* Author: Biren Patel
* Description: generic insertion sort algorithm
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "src/unity.h"


/******************************************************************************/
//modeled after the stdlib qsort function. Only requires string.h, the other 4
//headers are used for the unit testing below. Incorrect nitems and size against
//base array is undefined behavior.

void isort
(
    void *base,
    size_t nitems,
    size_t size,
    int (*cmp)(const void *, const void *)
)
{
    //return silently on usage misuse
    if (nitems <= 1 || base == NULL || cmp == NULL) return;
    
    //inner loop uses VLA instead of malloc for temporary storage
    char tmp[size];
    
    //base[0] is trivially sorted hence i = 1
    for (size_t i = 1; i < nitems; ++i)
    {
        //walk a pointer down from base[i] to base[1] performing swaps as needed
        for (char *curr = (char*) base + i * size; curr != base; curr -= size)
        {
            char *prev = curr - size;
            
            if (cmp(curr, prev) < 0)
            {
                memcpy(tmp, curr, size);
                memcpy(curr, prev, size);
                memcpy(prev, tmp, size);
            }
            else break;
        }
    }
    
    return;
}


/******************************************************************************/
//the rest of the file is unit testing via the Unity Framework

static inline int cmp(const void *a, const void *b)
{
    return *((int*) a) - *((int*) b);
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
    isort(data, N, 4, cmp);
    
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
