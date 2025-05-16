#ifndef fastware_DEF_H
#define fastware_DEF_H

#include <cstdint>

#define attr_packed __attribute__((packed))

#define cpp_do_pragma(x) _Pragma(#x)
#define todo(x) cpp_do_pragma(message("TODO - " #x))

template <typename Ret, typename T> constexpr Ret size_of() {
  return static_cast<Ret>(sizeof(T));
}

template <typename Ret, typename T> constexpr Ret cast(T t) {
  return static_cast<Ret>(t);
}

#define CAT_(a, b) a##b
#define CAT(a, b) CAT_(a, b)
#define VARNAME(name) CAT(name, __LINE__)

#endif // fastware_DEF_H
