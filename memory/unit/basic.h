#include <fastware/memory.h>
#include <gtest/gtest.h>

TEST(memory, pow_of_2) {

  const uint64_t pow_of_2_size = fastware::memory::next_pow_of_2(17);

  ASSERT_EQ(pow_of_2_size, 32);

  ASSERT_TRUE(fastware::memory::pow_of_2(pow_of_2_size));
}

TEST(memory, basic_align) {

  const uint64_t aligned_size =
      fastware::memory::align(17, fastware::memory::alignment_t::b64);

  ASSERT_EQ(aligned_size, 64);

  ASSERT_TRUE(fastware::memory::pow_of_2(aligned_size));
}