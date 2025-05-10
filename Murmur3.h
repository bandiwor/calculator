//
// Created by kir on 09.05.2025.
//

#ifndef HASH_H
#define HASH_H
#include <stdint.h>

uint64_t murmur3_64(const void *key, size_t len, uint64_t seed);

#endif // HASH_H
