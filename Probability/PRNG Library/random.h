/*
* Author: Biren Patel
* Description: PRNG library for non-cryptographic non-secure purposes such as
* statistics and simulations. This library depends on GCC builtins, on the x86
* rdrand instruction, and on the AVX/AVX2 instruction sets.
*/

#ifndef RANDOM_H
#define RANDOM_H

//Utilities API
#include "random_utils.h"

//64-BIT API
#include "random_sisd.h"

//256-BIT API
#include "random_simd.h"

#endif