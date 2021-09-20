#ifndef MEMORY_H
#define MEMORY_H

#include <cstddef>
#include <cstdint>

namespace fastware {
namespace memory {

struct alignment_t {
  enum value : uint64_t {
    b4 = 4,
    b8 = 8,
    b16 = 16,
    b32 = 32,
    b64 = 64,
    b128 = 128,
    b256 = 256,
    b512 = 512,
    b4k = 4196
  };
  static constexpr uint64_t mask(value val) { return val - 1; }
};

constexpr uint64_t align(uint64_t size, alignment_t::value alignment) {
  return (size + alignment_t::mask(alignment)) & ~alignment_t::mask(alignment);
}

constexpr bool is_aligned(const void *__restrict ptr,
                          alignment_t::value alignment) {
  return (reinterpret_cast<uintptr_t>(ptr) & alignment_t::mask(alignment)) == 0;
}

struct allocator_t;

struct memblk {
  void *ptr;
  uint64_t size;
};

struct stack_alloc_create_info_t {
  allocator_t *parent;
  uint64_t size;
  alignment_t::value alignment;
};

allocator_t *create(stack_alloc_create_info_t *info);

void destroy(allocator_t *alloc);

memblk allocate(allocator_t *alloc, uint64_t size);

void deallocate(allocator_t *alloc, memblk blk);

void deallocate_all(allocator_t *alloc);

uint64_t prefered_size(allocator_t *alloc, uint64_t size);

bool owns(allocator_t *alloc, memblk blk);

} // namespace memory
} // namespace fastware

#endif // MEMORY_H
