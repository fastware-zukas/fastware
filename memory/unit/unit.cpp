

#include <gtest/gtest.h>

TEST(memory, start) { SUCCEED(); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}