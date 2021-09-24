#include "include/memory.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <memory>

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

  const uint64_t aligned_alloc_size =
      align(info->allocator_size, info->alignment);
  const uint64_t aligned_mem_space_size =
      align(info->mem_space_size, info->alignment);
  const uint64_t total_aligned_size =
      aligned_alloc_size + aligned_mem_space_size;

  memblk temp_blk{nullptr, 0};
  if (info->parent) {
    temp_blk = allocate(info->parent, total_aligned_size +
                                          alignment_t::mask(info->alignment));
  } else {
    temp_blk = {aligned_alloc(info->alignment, total_aligned_size),
                total_aligned_size};
  }

  void *usable_address =
      static_cast<uint8_t *>(temp_blk.ptr) + aligned_alloc_size;
  uint64_t max_usable_size = temp_blk.size - aligned_alloc_size;
  uint64_t usable_size = aligned_mem_space_size;

  std::align(info->alignment, max_usable_size, usable_address, usable_size);
  assert(usable_size >= aligned_mem_space_size &&
         "Not enough memory left after alignment");
  assert(is_aligned(usable_address, info->alignment) &&
         "Memory does not appear to be aligned");

  return {temp_blk.ptr, temp_blk.size, static_cast<uint8_t *>(usable_address),
          usable_size};
}

memblk internal_alloc(stack_allocator_t *alloc, uint64_t size) {

  uint64_t aligned_size = align(size, alloc->alignment);

  uint8_t *current_head = alloc->block_start;
  uint8_t *next_head = current_head + aligned_size;

  if (next_head > alloc->mem_space_end) {
    // out of memory
    return {nullptr, 0};
  }

  alloc->block_start = next_head;

  return {current_head, aligned_size};
}

void internal_dealloc(stack_allocator_t *alloc, memblk blk) {

  uint8_t *adjusted_head = alloc->block_start - blk.size;
  if (adjusted_head == blk.ptr) {
    // This was the last allocation
    alloc->block_start = adjusted_head;
  }
  // We cannot deallocate from the middle of the stack allocator
}

void internal_dealloc_all(stack_allocator_t *alloc) {
  alloc->block_start = alloc->mem_space_start;
}

uint64_t internal_pref_size(stack_allocator_t *alloc, uint64_t size) {
  return align(size, alloc->alignment);
}

bool internal_owns(stack_allocator_t *alloc, memblk blk) {
  return blk.ptr >= alloc->mem_space_start & blk.ptr < alloc->mem_space_end;
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

void destroy(allocator_t *alloc) {

  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);

  allocator_t *parent = nullptr;
  uint64_t size = 0;

  switch (*type) {
  case alloc_type_e::stack: {
    stack_allocator_t *stack_alloc = static_cast<stack_allocator_t *>(alloc);
    parent = stack_alloc->parent;
    size = stack_alloc->size;
    break;
  }

  default:
    return;
  }

  if (parent) {
    deallocate(parent, {alloc, size});
  } else {
    free(alloc);
  }
}

memblk allocate(allocator_t *alloc, uint64_t size) {

  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack:
    return internal_alloc(static_cast<stack_allocator_t *>(alloc), size);
  default: {
    assert(false && "Unknown allocator used");
    return {nullptr, 0};
  }
  }
}

void deallocate(allocator_t *alloc, memblk blk) {
  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack:
    internal_dealloc(static_cast<stack_allocator_t *>(alloc), blk);
  default: {
    assert(false && "Unknown allocator used");
  }
  }
}

void deallocate_all(allocator_t *alloc) {
  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack:
    internal_dealloc_all(static_cast<stack_allocator_t *>(alloc));
  default: {
    assert(false && "Unknown allocator used");
  }
  }
}

uint64_t prefered_size(allocator_t *alloc, uint64_t size) {
  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack:
    return internal_pref_size(static_cast<stack_allocator_t *>(alloc), size);
  default: {
    assert(false && "Unknown allocator used");
    return 0;
  }
  }
}

bool owns(allocator_t *alloc, memblk blk) {
  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack:
    return internal_owns(static_cast<stack_allocator_t *>(alloc), blk);
  default: {
    assert(false && "Unknown allocator used");
    return false;
  }
  }
}

} // namespace memory
} // namespace fastware
