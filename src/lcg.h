/**
 * We have to use our own implementation of the random numbers generator
 * to have the same result for the same seed on every platform.
 */
#ifndef __LCG__
#define __LCG__

#include <stdint.h>

typedef uint64_t lcg;

#define lcg_rand(seed) (*seed = *seed * 48271 % 0x7fffffff)

#endif // __LCG__
