#ifndef MATH_H
#define MATH_H

#include <cstddef>
#include <cstdint>

namespace fastware {

constexpr float PI{3.14159265358979323846f};

constexpr uint32_t fast_max(uint32_t x, uint32_t y) noexcept {
  return x ^ ((x ^ y) & -(x < y));
}

constexpr int32_t fast_max(int32_t x, int32_t y) noexcept {
  return x ^ ((x ^ y) & -(x < y));
}

constexpr uint32_t fast_min(uint32_t x, uint32_t y) noexcept {
  return y ^ ((x ^ y) & -(x < y));
}

constexpr int32_t fast_min(int32_t x, int32_t y) noexcept {
  return y ^ ((x ^ y) & -(x < y));
}

} // namespace fastware

#endif // MATH_H
