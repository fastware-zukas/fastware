#include <fastware/memory.h>

#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>

namespace fastware {
namespace memory {

struct allocator_t {};

enum class alloc_type_e : uint64_t { none = 0, stack, pool };

struct stack_allocator_t : allocator_t {
  alloc_type_e type;
  address block_start;
  address mem_space_start;
  address mem_space_end;
  alignment_t::value alignment;
  allocator_t *parent;
  uint64_t size;
};

struct pool_allocator_t : allocator_t {
  alloc_type_e type;
  address mem_space_start;
  address mem_space_end;
  allocator_t *parent;
  uint64_t size;
  uint64_t block_count;
  uint64_t aligned_block_size;
  alignment_t::value alignment;
  uint64_t bit_shift;
  address *block_start;
  address control_blocks[];
};

namespace {
void internal_print_state(pool_allocator_t *alloc) {
  printf("pool_allocator_t (%p) {\n"
         "mem_space_start = %p\n"
         "mem_space_end = %p\n"
         "parent = %p\n"
         "size = %lu\n"
         "block_count = %lu\n"
         "aligned_block_size = %lu\n"
         "alignment = %lu\n"
         "bit_shift = %lu\n"
         "block_start = %p\n"
         "control_blocks = %p\n"
         "}\n",
         alloc, alloc->mem_space_start.raw, alloc->mem_space_end.raw,
         alloc->parent, alloc->size, alloc->block_count,
         alloc->aligned_block_size, alloc->alignment, alloc->bit_shift,
         alloc->block_start, alloc->control_blocks);
}
} // namespace

namespace {
// internal functions here
struct aligned_storage_t {
  address base_address;
  uint64_t size;
  address usable_address;
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

  address base_address{.raw = temp_blk.ptr};
  address usable_address = base_address + aligned_alloc_size;

  uint64_t max_usable_size = temp_blk.size - aligned_alloc_size;
  uint64_t usable_size = aligned_mem_space_size;

  std::align(info->alignment, max_usable_size, usable_address.raw, usable_size);
  assert(usable_size >= aligned_mem_space_size &&
         "Not enough memory left after alignment");
  assert(is_aligned(usable_address, info->alignment) &&
         "Memory does not appear to be aligned");

  return {base_address, temp_blk.size, usable_address, usable_size};
}

memblk internal_alloc(stack_allocator_t *alloc, uint64_t size) {

  uint64_t aligned_size = align(size, alloc->alignment);

  auto current_head = alloc->block_start;
  auto next_head = current_head + aligned_size;

  if (__builtin_expect(next_head > alloc->mem_space_end, false)) {
    // out of memory
    return {nullptr, 0};
  }

  alloc->block_start = next_head;

  return {current_head, aligned_size};
}

memblk internal_alloc(pool_allocator_t *alloc, uint64_t size) {

#ifdef FASTWARE_VERBOSE
  printf("internal_alloc(pool_allocator_t*) - before\n");
  internal_print_state(alloc);
#endif
  const uint64_t aligned_size = align(size, alloc->alignment);
  assert(alloc->aligned_block_size == aligned_size && "Invalid block size");

  if (__builtin_expect(alloc->block_start == nullptr, false)) {
    // out of memory
    return {nullptr, 0};
  }

  address *next_block = static_cast<address *>(alloc->block_start->raw);
  const uint64_t address_diff = address{.raw = alloc->block_start} -
                                address{.raw = alloc->control_blocks};
  const uint64_t address_shift = address_diff << alloc->bit_shift;
  const address offset_address = alloc->mem_space_start + address_shift;

  alloc->block_start = next_block;

#ifdef FASTWARE_VERBOSE
  printf("address_diff = %lu, address_shift = %lu, mem_space_start = %p, "
         "offset_address = %p, range = %lu, control_blocks = %p, block_start = "
         "%p, next_block = %p\n",
         address_diff, address_shift, alloc->mem_space_start.raw,
         offset_address.raw, offset_address - alloc->mem_space_start,
         alloc->control_blocks, alloc->block_start, next_block);
  printf("internal_alloc(pool_allocator_t*) - after\n");
  internal_print_state(alloc);
#endif
  return {offset_address.raw, aligned_size};
}

void internal_dealloc(stack_allocator_t *alloc, memblk blk) {

  auto adjusted_head = alloc->block_start - blk.size;
  if (adjusted_head == blk.ptr) {
    // This was the last allocation
    alloc->block_start = adjusted_head;
  }
  // We cannot deallocate from the middle of the stack allocator
}

void internal_dealloc(pool_allocator_t *alloc, memblk blk) {
  assert(alloc->aligned_block_size == blk.size && "Not a correct block size");
#ifdef FASTWARE_VERBOSE
  printf("internal_dealloc(pool_allocator_t*) - before\n");
  internal_print_state(alloc);
#endif
  const address block{.raw = blk.ptr};
  const uint64_t mem_blk_diff = block - alloc->mem_space_start;
  const address control_block_addr = address{.raw = alloc->control_blocks} +
                                     (mem_blk_diff >> alloc->bit_shift);
  address *control_block = static_cast<address *>(control_block_addr.raw);
  control_block->raw = alloc->block_start;
  alloc->block_start = control_block;
#ifdef FASTWARE_VERBOSE
  printf("internal_dealloc(pool_allocator_t*) - after\n");
  internal_print_state(alloc);
#endif
}

void internal_dealloc_all(stack_allocator_t *alloc) {
  alloc->block_start = alloc->mem_space_start;
}

void internal_dealloc_all(pool_allocator_t *alloc) {
#ifdef FASTWARE_VERBOSE
  printf("internal_dealloc_all(pool_allocator_t*)\n");
  internal_print_state(alloc);
#endif
  alloc->block_start = &alloc->control_blocks[0];
  for (uint64_t i = 0; i < alloc->block_count - 1; i++) {
    alloc->control_blocks[i].raw = &alloc->control_blocks[i + 1];
  }
  alloc->control_blocks[alloc->block_count - 1].raw = nullptr;
}

uint64_t internal_pref_size(stack_allocator_t *alloc, uint64_t size) {
  return align(size, alloc->alignment);
}

uint64_t internal_pref_size(pool_allocator_t *alloc, uint64_t size) {
  return size > alloc->aligned_block_size ? 0 : alloc->aligned_block_size;
}

bool internal_owns(stack_allocator_t *alloc, memblk blk) {
  return blk.ptr >= alloc->mem_space_start & blk.ptr < alloc->mem_space_end;
}

bool internal_owns(pool_allocator_t *alloc, memblk blk) {
  return blk.ptr >= alloc->mem_space_start & blk.ptr < alloc->mem_space_end;
}

bool constexpr pow_of_2(uint64_t size) { return __builtin_popcount(size) == 1; }

} // namespace

allocator_t *create(stack_alloc_create_info_t *info) {

  aligned_storage_create_info_t aligned_info{
      info->parent, sizeof(stack_allocator_t), info->size, info->alignment};

  aligned_storage_t storage = create_aligned_storage(&aligned_info);

  stack_allocator_t *alloc =
      static_cast<stack_allocator_t *>(storage.base_address.raw);
  alloc->type = alloc_type_e::stack;
  alloc->block_start = storage.usable_address;
  alloc->mem_space_start = storage.usable_address;
  alloc->mem_space_end = storage.usable_address + storage.usable_size;
  alloc->alignment = info->alignment;
  alloc->parent = info->parent;
  alloc->size = storage.size;

  return alloc;
}

allocator_t *create(pool_alloc_create_info_t *info) {

  const uint64_t aligned_block_size =
      align(info->block_size, info->block_alignment);

  assert(pow_of_2(aligned_block_size) && "Block size can only be a power of 2");

  const uint64_t alloc_space_size = aligned_block_size * info->block_count;
  const uint64_t control_block_size = sizeof(address) * info->block_count;
  const uint64_t allocator_size = sizeof(pool_allocator_t) + control_block_size;

  aligned_storage_create_info_t storage_info{
      info->parent, allocator_size, alloc_space_size, info->block_alignment};

  aligned_storage_t storage = create_aligned_storage(&storage_info);

  pool_allocator_t *alloc =
      static_cast<pool_allocator_t *>(storage.base_address.raw);

  const uint64_t bit_shift =
      __builtin_ctzl(aligned_block_size) - __builtin_ctzl(sizeof(address));

  alloc->type = alloc_type_e::pool;
  alloc->mem_space_start = storage.usable_address;
  alloc->mem_space_end = storage.usable_address + storage.usable_size;
  alloc->parent = info->parent;
  alloc->size = storage.size;
  alloc->block_count = info->block_count;
  alloc->aligned_block_size = aligned_block_size;
  alloc->alignment = info->block_alignment;
  alloc->bit_shift = bit_shift;
  alloc->block_start = &alloc->control_blocks[0];

#ifdef FASTWARE_VERBOSE
  internal_print_state(alloc);
#endif
  for (uint64_t i = 0; i < info->block_count - 1; i++) {
    alloc->control_blocks[i].raw = &alloc->control_blocks[i + 1];
#ifdef FASTWARE_VERBOSE
    printf("i = %lu, size = %lu, control_blocks[i] = %p, control_blocks[i + 1] "
           "= %p\n",
           i, alloc->block_count, &alloc->control_blocks[i],
           &alloc->control_blocks[i + 1]);
#endif
  }
  alloc->control_blocks[info->block_count - 1].raw = nullptr;

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
  case alloc_type_e::pool: {
    pool_allocator_t *pool_alloc = static_cast<pool_allocator_t *>(alloc);
    parent = pool_alloc->parent;
    size = pool_alloc->size;
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
  case alloc_type_e::stack: {
    return internal_alloc(static_cast<stack_allocator_t *>(alloc), size);
  }
  case alloc_type_e::pool: {
    return internal_alloc(static_cast<pool_allocator_t *>(alloc), size);
  }
  default: {
    assert(false && "Unknown allocator used");
    return {nullptr, 0};
  }
  }
}

void deallocate(allocator_t *alloc, memblk blk) {
  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack: {
    internal_dealloc(static_cast<stack_allocator_t *>(alloc), blk);
    break;
  }
  case alloc_type_e::pool: {
    internal_dealloc(static_cast<pool_allocator_t *>(alloc), blk);
    break;
  }
  default: {
    assert(false && "Unknown allocator used");
  }
  }
}

void deallocate_all(allocator_t *alloc) {
  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack: {
    internal_dealloc_all(static_cast<stack_allocator_t *>(alloc));
    break;
  }
  case alloc_type_e::pool: {
    internal_dealloc_all(static_cast<pool_allocator_t *>(alloc));
    break;
  }
  default: {
    assert(false && "Unknown allocator used");
  }
  }
}

uint64_t prefered_size(allocator_t *alloc, uint64_t size) {
  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack: {
    return internal_pref_size(static_cast<stack_allocator_t *>(alloc), size);
  }
  case alloc_type_e::pool: {
    return internal_pref_size(static_cast<pool_allocator_t *>(alloc), size);
  }
  default: {
    assert(false && "Unknown allocator used");
    return 0;
  }
  }
}

bool owns(allocator_t *alloc, memblk blk) {
  alloc_type_e *type = reinterpret_cast<alloc_type_e *>(alloc);
  switch (*type) {
  case alloc_type_e::stack: {
    return internal_owns(static_cast<stack_allocator_t *>(alloc), blk);
  }
  case alloc_type_e::pool: {
    return internal_owns(static_cast<pool_allocator_t *>(alloc), blk);
  }
  default: {
    assert(false && "Unknown allocator used");
    return false;
  }
  }
}

} // namespace memory
} // namespace fastware
