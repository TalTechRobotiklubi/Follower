#include "fl_block_alloc.h"
#include <stdint.h>
#include <assert.h>
#include <unordered_set>
#include <vector>
#include <algorithm>

struct fl_alloc_slot {
  void* ptr;
  size_t idx;
};

struct fl_block_alloc {
  size_t block_size;
  size_t num_bytes;
  uint8_t* memory;

  std::vector<fl_alloc_slot> full_slots;
  std::unordered_set<size_t> free_indices;
};

fl_block_alloc* fl_block_alloc_create(size_t block_size, size_t num_blocks) {
  fl_block_alloc* block_alloc = new fl_block_alloc();

  block_alloc->block_size = block_size;
  block_alloc->num_bytes = block_size * num_blocks;
  block_alloc->memory = new uint8_t[block_alloc->num_bytes];
  block_alloc->free_indices.reserve(num_blocks);

  for (size_t i = 0; i < num_blocks; i++) {
    block_alloc->free_indices.insert(i);
  }

  return block_alloc;
}

void fl_block_alloc_destroy(fl_block_alloc* block_alloc) {
  delete block_alloc->memory;
  delete block_alloc;
}

void* fl_block_alloc_acquire(fl_block_alloc* block_alloc) {
  if (block_alloc->free_indices.size() == 0) return nullptr;

  const size_t free_idx = *(block_alloc->free_indices.begin());
  const size_t offset = free_idx * block_alloc->block_size;

  uint8_t* block = block_alloc->memory + offset;

  const fl_alloc_slot slot = {block, free_idx};
  block_alloc->full_slots.push_back(slot);

  block_alloc->free_indices.erase(free_idx);

  return block;
}

void fl_block_alloc_release(fl_block_alloc* block_alloc, void* ptr) {
  auto it = std::find_if(
      block_alloc->full_slots.begin(), block_alloc->full_slots.end(),
      [=](const fl_alloc_slot& slot) { return ptr == slot.ptr; });

  auto end = block_alloc->full_slots.end();
  assert(it != end);

  if (it == end) {
    return;
  }

  block_alloc->free_indices.insert(it->idx);
  block_alloc->full_slots.erase(it);
}
