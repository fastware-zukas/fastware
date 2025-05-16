#include <fastware/bounding.h>
#include <fastware/data_types.h>
#include <fastware/utils.h>

namespace fastware {

uint32_t create_bounding_box_vao(mat4_t *matrixes, uint32_t count,
                                 buffer_update_info_t *update_buffer) {

  constexpr vec3_t vertices[]{{-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
                              {0.5, 0.5, -0.5},   {-0.5, 0.5, -0.5},
                              {-0.5, -0.5, 0.5},  {0.5, -0.5, 0.5},
                              {0.5, 0.5, 0.5},    {-0.5, 0.5, 0.5}};

  constexpr uint32_t indexes[]{0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};

  const buffer_create_info_t buffer_infos[]{
      {.target = buffer_target_e::ARRAY_BUFFER,
       .type = buffer_type_e::STATIC,
       .size = sizeof(vertices),
       .data = vertices},
      {.target = buffer_target_e::ELEMENT_ARRAY_BUFFER,
       .type = buffer_type_e::STATIC,
       .size = sizeof(indexes),
       .data = indexes},
      {.target = buffer_target_e::ARRAY_BUFFER,
       .type = buffer_type_e::DYNAMIC,
       .size = size_of<uint32_t, mat4_t>() * count,
       .data = matrixes}};

  uint32_t buffers[3]{0};
  buffer::create(buffer_infos, 3, buffers);

  constexpr vertex_buffer_section_definition_t per_vertex{
      data_type_e::FLOAT_VECTOR3};

  constexpr vertex_buffer_section_definition_t per_instance{
      data_type_e::FLOAT_MATRIX4};

  const vertex_buffer_definition_t infos[]{
      {buffers[0], data_rate_e::PER_VERTEX, 8, 1, &per_vertex},
      {buffers[2], data_rate_e::PER_INSTANCE, count, 1, &per_instance}};

  const vertex_array_definition_t varray_info{infos, 2, buffers[1]};

  uint32_t vert_id = 0;
  varray::create(&varray_info, 1, &vert_id);

  *update_buffer = {.buffer_id = buffers[2],
                    .offset = 0,
                    .size = size_of<uint32_t, mat4_t>() * count,
                    .data = matrixes};

  return vert_id;
}

mat4_t compute_bounding_box(vec3_t *vetexes, int32_t count) {

  float min_x = vetexes[0].x;
  float min_y = vetexes[0].y;
  float min_z = vetexes[0].z;
  float max_x = vetexes[0].x;
  float max_y = vetexes[0].y;
  float max_z = vetexes[0].z;

  for (int32_t i = 1; i < count; i++) {
    if (vetexes[i].x < min_x)
      min_x = vetexes[i].x;
    if (vetexes[i].y < min_y)
      min_y = vetexes[i].y;
    if (vetexes[i].z < min_z)
      min_z = vetexes[i].z;

    if (vetexes[i].x > max_x)
      max_x = vetexes[i].x;
    if (vetexes[i].y > max_y)
      max_y = vetexes[i].y;
    if (vetexes[i].z > max_z)
      max_z = vetexes[i].z;
  }

  vec3_t size{max_x - min_x, max_y - min_y, max_z - min_z};
  vec3_t center{(min_x + max_x) / 2.f, (min_y + max_y) / 2.f,
                (min_z + max_z) / 2.f};
  mat4_t transform = glms_mat4_mul(glms_translate(glms_mat4_identity(), center),
                                   glms_scale(glms_mat4_identity(), size));
  return transform;
}
} // namespace fastware