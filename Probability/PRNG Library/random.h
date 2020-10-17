/*
* Author: Biren Patel
* Description: PRNG library for non-cryptographic non-secure purposes such as
* statistics and simulations. This library depends on GCC builtins and the x86 
* instruction RDRAND. The 256-Bit API depends on the AVX/AVX2 intruction set.
*/

#ifndef RANDOM_H
#define RANDOM_H

//64-BIT API
#include "random_sisd.h"

//256-BIT AVX/AVX2 API
#include "random_simd.h"

#endif