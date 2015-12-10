#ifndef FL_BLOCK_ALLOC
#define FL_BLOCK_ALLOC

#include <stddef.h>

struct fl_block_alloc;

fl_block_alloc* fl_block_alloc_create(size_t block_size, size_t num_blocks);
void fl_block_alloc_destroy(fl_block_alloc* block_alloc);
void* fl_block_alloc_acquire(fl_block_alloc* block_alloc);
void fl_block_alloc_release(fl_block_alloc* block_alloc, void* ptr);

#endif
