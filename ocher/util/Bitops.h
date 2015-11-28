#ifndef UTIL_BITOPS_H
#define UTIL_BITOPS_H

/** @file
 *  Fast bitwise functions, mostly from by "Hacker's Delight".
 */

#include <stdint.h>

/**
 *  @note branch-free
 *  @return absolute value of x
 */
inline int32_t absolute(int32_t x)
{
    // Hacker's Delight, 2-4, p 17
    int y = x >> 31;

    return (x ^ y) - y;
}

/**
 *  Swaps a and b in-place.
 */
inline void swap(int &a, int &b)
{
    // Hacker's Delight, p 38
    a ^= b;
    b ^= a;
    a ^= b;
}

/**
 *  @note branch-free
 *  @return -1 if x < 0, 0 if x = 0, 1 if x > 0
 */
inline int sign(int32_t x)
{
    // Hacker's Delight, 2-7, p 19
    return -((-(x >> 31)) | ((-x) >> 31));
}

/**
 *  @note branch-free
 *  @return Number of bits set in x.
 */
inline unsigned int population(uint32_t x)
{
    // Hacker's Delight, 5-1, p 66
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0f0f0f0f;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x0000003f;
}

/**
 *  @note branch-free
 *  @return Number of leading (most significant) zeros (0 to 32).
 */
inline unsigned int nlz(uint32_t x)
{
    // Hacker's Delight, 5-11, p 80
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return population(~x);
}

/**
 *  @note branch-free
 *  @return Number of trailing (least significant) zeros (0 to 32).
 */
inline unsigned int ntz(uint32_t x)
{
    // Hacker's Delight, p 84
    return 32 - population(x | -x);
}

/**
 *  @note branch-free
 *  @return If pred then a else b.
 */
inline unsigned int bitopCond(bool pred, unsigned int a, unsigned int b)
{
    const unsigned int p = -static_cast<unsigned int>(pred);

    return (a & p) | (b & ~p);
}

#endif
