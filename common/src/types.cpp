#include <fastware/types.h>

namespace fastware {

namespace input {

mod_bits operator|(mod_bits lhs, mod_bits rhs) {
  return static_cast<mod_bits>(static_cast<int32_t>(lhs) |
                               static_cast<int32_t>(rhs));
}

mod_bits &operator|=(mod_bits &lhs, mod_bits rhs) {
  lhs = lhs | rhs;
  return lhs;
}

mod_bits operator&(mod_bits lhs, mod_bits rhs) {
  return static_cast<mod_bits>(static_cast<int32_t>(lhs) &
                               static_cast<int32_t>(rhs));
}

mod_bits &operator&=(mod_bits &lhs, mod_bits rhs) {
  lhs = lhs & rhs;
  return lhs;
}

mod_bits operator^(mod_bits lhs, mod_bits rhs) {
  return static_cast<mod_bits>(static_cast<int32_t>(lhs) ^
                               static_cast<int32_t>(rhs));
}

mod_bits &operator^=(mod_bits &lhs, mod_bits rhs) {
  lhs = lhs ^ rhs;
  return lhs;
}

mod_bits operator~(mod_bits lhs) {
  return static_cast<mod_bits>(~static_cast<int32_t>(lhs));
}

action operator~(action lhs) {
  return lhs == action::RELEASE ? action::PRESS : action::RELEASE;
}

} // namespace input

// namespace input

// namespace input
} // namespace fastware
