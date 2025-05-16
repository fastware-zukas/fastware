#include <fastware/types.h>

namespace fastware {

namespace input {

mod_bits_e operator|(mod_bits_e lhs, mod_bits_e rhs) {
  return static_cast<mod_bits_e>(static_cast<int32_t>(lhs) |
                                 static_cast<int32_t>(rhs));
}

mod_bits_e &operator|=(mod_bits_e &lhs, mod_bits_e rhs) {
  lhs = lhs | rhs;
  return lhs;
}

mod_bits_e operator&(mod_bits_e lhs, mod_bits_e rhs) {
  return static_cast<mod_bits_e>(static_cast<int32_t>(lhs) &
                                 static_cast<int32_t>(rhs));
}

mod_bits_e &operator&=(mod_bits_e &lhs, mod_bits_e rhs) {
  lhs = lhs & rhs;
  return lhs;
}

mod_bits_e operator^(mod_bits_e lhs, mod_bits_e rhs) {
  return static_cast<mod_bits_e>(static_cast<int32_t>(lhs) ^
                                 static_cast<int32_t>(rhs));
}

mod_bits_e &operator^=(mod_bits_e &lhs, mod_bits_e rhs) {
  lhs = lhs ^ rhs;
  return lhs;
}

mod_bits_e operator~(mod_bits_e lhs) {
  return static_cast<mod_bits_e>(~static_cast<int32_t>(lhs));
}

action_e operator~(action_e lhs) {
  return lhs == action_e::RELEASE ? action_e::PRESS : action_e::RELEASE;
}

} // namespace input

// namespace input

// namespace input
} // namespace fastware
