#ifndef BOUNDING_H
#define BOUNDING_H

#include <fastware/fastware_def.h>

#include <glm/glm.hpp>

namespace fastware {

struct buffer_update_info_t;

uint32_t create_bounding_box_vao(glm::mat4 *matrixes, uint32_t count,
                                 buffer_update_info_t *update_buffer);

glm::mat4 compute_bounding_box(glm::vec3 *vetexes, int32_t count);

} // namespace fastware

#endif // BOUNDING_H