/*
* Author: Biren Patel
* Description: Timing macros for rough and quick microsecond benchmarking.
*/

#ifndef TIMEIT_H
#define TIMEIT_H

#include <windows.h>

#define init_timeit()                                                          \
        LARGE_INTEGER start, end, mu, frequency;                               \
        QueryPerformanceFrequency(&frequency)                                  \
        
#define start_timeit()                                                         \
        QueryPerformanceCounter(&start)                                        \
        
#define end_timeit()                                                           \
        QueryPerformanceCounter(&end);                                         \
        mu.QuadPart = end.QuadPart - start.QuadPart;                           \
        mu.QuadPart *= 1000000;                                                \
        mu.QuadPart /= frequency.QuadPart;                                     \
        
#define TIMEIT_RESULT_MICROSECONDS mu.QuadPart

#endif
