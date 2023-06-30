/**
 * We have to use our own implementation of the random numbers generator
 * to have the same result for the same seed on every platform.
 */
#ifndef __LCG__
#define __LCG__

#include <stdint.h>

static uint64_t current_seed = 1904; // my birthday ^_^

#define lcg_init(seed) (current_seed = seed)

#define lcg_next() (current_seed = current_seed * 48271 % 0x7fffffff)

#endif // __LCG__
