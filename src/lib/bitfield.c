#include "bitfield.h"



inline bool bf_get(bitfield_t *bitfield, uint i) {
    bitfield_t l = bitfield[i >> 5];
    return l >> (i & 31);
}

inline void bf_set(bitfield_t *bitfield, uint i, bool v) {
    bitfield[i >> 5] = v << (i & 31);
}
