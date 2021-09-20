#include "include/memory.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace fastware {
namespace memory {

struct allocator_t {};

enum class alloc_type_e : uint64_t { none = 0, stack };

struct stack_allocator_t : allocator_t {
  alloc_type_e type;
  uint8_t *block_start;
  uint8_t *mem_space_start;
  uint8_t *mem_space_end;
  alignment_t::value alignment;
  allocator_t *parent;
  uint64_t size;
};

namespace {
// internal functions here
struct aligned_storage_t {
  void *base_address;
  uint64_t size;
  uint8_t *usable_address;
  uint64_t usable_size;
};

struct aligned_storage_create_info_t {
  allocator_t *parent;
  uint64_t allocator_size;
  uint64_t mem_space_size;
  alignment_t::value alignment;
};

aligned_storage_t create_aligned_storage(aligned_storage_create_info_t *info) {
  return {nullptr, 0, nullptr, 0};
}

} // namespace

allocator_t *create(stack_alloc_create_info_t *info) {

  aligned_storage_create_info_t aligned_info{
      info->parent, sizeof(stack_allocator_t), info->size, info->alignment};

  aligned_storage_t storage = create_aligned_storage(&aligned_info);

  stack_allocator_t *alloc =
      static_cast<stack_allocator_t *>(storage.base_address);
  alloc->type = alloc_type_e::stack;
  alloc->block_start = storage.usable_address;
  alloc->mem_space_start = storage.usable_address;
  alloc->mem_space_end = storage.usable_address + storage.usable_size;
  alloc->alignment = info->alignment;
  alloc->parent = info->parent;
  alloc->size = storage.size;

  return alloc;
}

void destroy(allocator_t *alloc) {}

memblk allocate(allocator_t *alloc, uint64_t size) {}

void deallocate(allocator_t *alloc, memblk blk) {}

void deallocate_all(allocator_t *alloc) {}

uint64_t prefered_size(allocator_t *alloc, uint64_t size) {}

bool owns(allocator_t *alloc, memblk blk) {}

} // namespace memory
} // namespace fastware
