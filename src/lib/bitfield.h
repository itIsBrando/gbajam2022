#ifndef BITFIELD_H
#define BITFIELD_H

#include "defines.h"

typedef uint32_t bitfield_t;

#define BITFIELD(name, bits) bitfield_t name[(bits) >> 5];

bool bf_get(bitfield_t *bitfield, uint i);
void bf_set(bitfield_t *bitfield, uint i, bool v);

#endif