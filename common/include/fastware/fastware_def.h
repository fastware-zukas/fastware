#ifndef fastware_DEF_H
#define fastware_DEF_H

#include <cstdint>

#define attr_packed __attribute__((packed))

#define cpp_do_pragma(x) _Pragma(#x)
#define todo(x) cpp_do_pragma(message("TODO - " #x))

template <typename T, typename Ret> constexpr Ret size_of() {
  return sizeof(T);
}

#endif // fastware_DEF_H
