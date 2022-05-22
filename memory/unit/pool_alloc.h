#include <fastware/memory.h>
#include <gtest/gtest.h>

using namespace fastware::memory;

TEST(memory, pool_allocator_create) {

  pool_alloc_create_info_t create_info{nullptr, 32, alignment_t::b32, 32};

  allocator_t *alloc = fastware::memory::create(&create_info);

  ASSERT_NE(alloc, nullptr);

  destroy(alloc);
}

TEST(memory, pool_allocator_aligned_alloc) {

  pool_alloc_create_info_t create_info{nullptr, 32, alignment_t::b32, 32};

  allocator_t *alloc = fastware::memory::create(&create_info);

  ASSERT_NE(alloc, nullptr);

  memblk blk = allocate(alloc, 17);

  ASSERT_EQ(blk.size, 32);
  ASSERT_TRUE(is_aligned(blk.ptr, alignment_t::b32));

  destroy(alloc);
}

TEST(memory, pool_allocator_aligned_alloc_all) {

  pool_alloc_create_info_t create_info{nullptr, 32, alignment_t::b32, 32};

  allocator_t *alloc = fastware::memory::create(&create_info);

  ASSERT_NE(alloc, nullptr);

  for (int i = 0, s = 1024 / 32; i < s; i++) {
    memblk blk = allocate(alloc, 17);

    ASSERT_EQ(blk.size, 32);
    ASSERT_TRUE(is_aligned(blk.ptr, alignment_t::b32));
  }

  memblk blk = allocate(alloc, 17);
  ASSERT_EQ(blk.size, 0);
  ASSERT_EQ(blk.ptr, nullptr);

  destroy(alloc);
}

TEST(memory, pool_allocator_aligned_alloc_all_dealloc_all) {

  pool_alloc_create_info_t create_info{nullptr, 32, alignment_t::b32, 32};

  allocator_t *alloc = fastware::memory::create(&create_info);

  ASSERT_NE(alloc, nullptr);

  for (int i = 0, s = 1024 / 32; i < s; i++) {
    memblk blk = allocate(alloc, 17);

    ASSERT_EQ(blk.size, 32);
    ASSERT_TRUE(is_aligned(blk.ptr, alignment_t::b32));
  }

  memblk empty_blk1 = allocate(alloc, 17);
  ASSERT_EQ(empty_blk1.size, 0);
  ASSERT_EQ(empty_blk1.ptr, nullptr);

  deallocate_all(alloc);

  for (int i = 0, s = 1024 / 32; i < s; i++) {
    memblk blk = allocate(alloc, 17);

    ASSERT_EQ(blk.size, 32);
    ASSERT_TRUE(is_aligned(blk.ptr, alignment_t::b32));
  }

  memblk empty_blk2 = allocate(alloc, 17);
  ASSERT_EQ(empty_blk2.size, 0);
  ASSERT_EQ(empty_blk2.ptr, nullptr);

  destroy(alloc);
}

TEST(memory, pool_allocator_aligned_alloc_dealloc) {

  pool_alloc_create_info_t create_info{nullptr, 32, alignment_t::b32, 32};

  allocator_t *alloc = fastware::memory::create(&create_info);

  ASSERT_NE(alloc, nullptr);

  memblk blk = allocate(alloc, 17);
  ASSERT_EQ(blk.size, 32);
  ASSERT_TRUE(is_aligned(blk.ptr, alignment_t::b32));

  memblk blk2 = allocate(alloc, 17);
  ASSERT_EQ(blk2.size, 32);
  ASSERT_TRUE(is_aligned(blk2.ptr, alignment_t::b32));

  memblk blk3 = allocate(alloc, 17);
  ASSERT_EQ(blk3.size, 32);
  ASSERT_TRUE(is_aligned(blk3.ptr, alignment_t::b32));

  deallocate(alloc, blk);

  memblk blk4 = allocate(alloc, 17);
  ASSERT_EQ(blk4.size, 32);
  ASSERT_TRUE(is_aligned(blk4.ptr, alignment_t::b32));
  ASSERT_EQ(blk4.ptr, blk.ptr);

  deallocate(alloc, blk4);

  memblk blk5 = allocate(alloc, 17);
  ASSERT_EQ(blk5.size, 32);
  ASSERT_TRUE(is_aligned(blk5.ptr, alignment_t::b32));
  ASSERT_EQ(blk5.ptr, blk4.ptr);

  destroy(alloc);
}
