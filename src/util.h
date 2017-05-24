#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define ctor(type, ...) (type){ __VA_ARGS__ }
#define swap(type, a, b) { type ___p3d_swap_tmp_var = a; a = b; b = ___p3d_swap_tmp_var; }

typedef uint8_t bool;
#define true 1
#define false 0

#endif // UTIL_H
