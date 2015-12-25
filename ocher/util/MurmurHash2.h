#ifndef LIBCLC_HASH_H
#define LIBCLC_HASH_H

/** @file
 *  Murmur2 hash.
 */

#include <stdint.h>

/**
 *  Hashes a chunk of data.
 *  @param data
 *  @param len
 *  @return A 32 bit hash value.
 *  @note This hash is not necessarily consistent across different endiannesses.
 */
uint32_t hash(const uint8_t *data, unsigned int len);

inline uint32_t hash(const char *data, unsigned int len)
{
    return hash((const uint8_t *)data, len);
}
inline uint32_t hash(const void *data, unsigned int len)
{
    return hash((const uint8_t *)data, len);
}

#endif
