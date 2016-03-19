#ifndef FNV1A_H
#define FNV1A_H

#include <stdint.h>

struct fnv1a {
  uint64_t state; 
};

fnv1a fnv1a_create();
uint64_t fnv1a_hash(const void* data, int length);
uint64_t fnv1a_hash(fnv1a* hash_state, const void* data, int length);

#endif
