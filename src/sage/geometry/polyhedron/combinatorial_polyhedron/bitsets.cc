/*
#############################################################################
# More or less taken from ``src/sage/data_structures/bitset.pxi``
#############################################################################
*/

#include "gmp.h"

const size_t index_shift = 6;
const size_t LIMB_BITS = 64;

/*
#############################################################################
# Creating limb patterns
#############################################################################
*/


inline uint64_t limb_one_set_bit(size_t n){
    /*
    Return a limb with only bit n set.
    */
    return (uint64_t) 1 << (n % LIMB_BITS);
}

inline uint64_t limb_one_zero_bit(size_t n){
    /*
    Return a limb with all bits set, except for bit n.
    */
    return !((uint64_t) 1 << (n % LIMB_BITS));
}

inline uint64_t limb_lower_bits_down(size_t n){
    /*
    Return a limb with the lower n bits set, where n is interpreted
    in [0 .. 64-1].
    */
    return ((uint64_t) 1 << (n % LIMB_BITS)) - 1;
}

/*
#############################################################################
# Bitset Bit Manipulation
#############################################################################
*/

inline int bitset_in(uint64_t* bits, size_t n){
    /*
    Check if n is in bits.  Return True (i.e., 1) if n is in the
    set, False (i.e., 0) otherwise.
    */
    return bits[n >> index_shift] & limb_one_set_bit(n);
}

inline void bitset_discard(uint64_t* bits, size_t n){
    /*
    Remove n from bits.
    */
    bits[n >> index_shift] &= limb_one_zero_bit(n);
}

inline void bitset_add(uint64_t* bits, size_t n){
    /*
    Add n to bits.
    */
    bits[n >> index_shift] |= limb_one_set_bit(n);
}

/*
#############################################################################
# Bitset Searching
#############################################################################
*/

inline size_t _bitset_first_in_limb(uint64_t limb){
    /*
    Given a limb of a bitset, return the index of the first nonzero
    bit. If there are no bits set in the limb, return -1.
    */
    if (limb == 0)
        return -1;
    return mpn_scan1(&limb, 0);
}

inline long _bitset_first_in_limb_nonzero(uint64_t limb){
    /*
    Given a non-zero limb of a bitset, return the index of the first
    nonzero bit.
    */
    return mpn_scan1(&limb, 0);
}

inline size_t bitset_next(uint64_t* bits, size_t face_length, size_t n){
    /*
    Calculate the index of the next element in the set, starting at
    (and including) n.  Return -1 if there are no elements from n
    onwards.
    */
    if (n >= face_length*LIMB_BITS)
        return -1;

    size_t i = n >> index_shift;
    size_t limb = bits[i] & ~limb_lower_bits_down(n);
    size_t ret = _bitset_first_in_limb(limb);
    if (ret != (size_t) -1)
        return (i << index_shift) | ret;
    for(i++; i < face_length; i++){
        if (bits[i])
            return (i << index_shift) | _bitset_first_in_limb_nonzero(bits[i]);
    }
    return -1;
}
