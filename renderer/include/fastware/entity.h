#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include <fastware/data_types.h>

namespace fastware {

struct entity {
  uint32_t program_id{0};
  uint32_t varray_id{0};
  uint32_t offset{0};
  uint32_t count{0};
  uint32_t instance_count{1};
  primitive_type_e primitive_type{primitive_type_e::TRIANGLES};
  enum { VERTEX, INDEX, INDEX_INSTANCED } render_type{VERTEX};
};

} // namespace fastware

#endif // ENTITY_H
