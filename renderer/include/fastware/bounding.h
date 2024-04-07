#ifndef BOUNDING_H
#define BOUNDING_H

#include <fastware/fastware_def.h>
#include <fastware/types.h>

namespace fastware {

struct buffer_update_info_t;

uint32_t create_bounding_box_vao(mat4_t *matrixes, uint32_t count,
                                 buffer_update_info_t *update_buffer);

mat4_t compute_bounding_box(vec3_t *vetexes, int32_t count);

} // namespace fastware

#endif // BOUNDING_H