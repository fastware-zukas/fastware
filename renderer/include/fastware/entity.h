#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>

namespace fastware {

struct entity {
  uint32_t program_id{0};
  uint32_t varray_id{0};
  uint32_t count{0};
  uint32_t instance_count{1};
  enum { VERTEX, INDEX, INDEX_INSTANCED } render_type{VERTEX};
};

} // namespace fastware

#endif // ENTITY_H
