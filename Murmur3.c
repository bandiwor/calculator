//
// Created by kir on 09.05.2025.
//
#include "Murmur3.h"

uint64_t murmur3_64(const void *key, const size_t len, const uint64_t seed) {
  const uint64_t m = 0xc6a4a7935bd1e995;
  const int r = 47;
  uint64_t h = seed ^ (len * m);

  const uint64_t *data = key;
  const uint64_t *end = data + (len / 8);

  while (data != end) {
    uint64_t k = *data++;
    k *= m;
    k ^= k >> r;
    k *= m;
    h ^= k;
    h *= m;
  }

  const unsigned char *data2 = (const unsigned char *)data;
  switch (len & 7) {
  case 7:
    h ^= (uint64_t)data2[6] << 48;
  case 6:
    h ^= (uint64_t)data2[5] << 40;
  case 5:
    h ^= (uint64_t)data2[4] << 32;
  case 4:
    h ^= (uint64_t)data2[3] << 24;
  case 3:
    h ^= (uint64_t)data2[2] << 16;
  case 2:
    h ^= (uint64_t)data2[1] << 8;
  case 1:
    h ^= (uint64_t)data2[0];
    h *= m;
  default:
    break;
  };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;
  return h;
}

