#ifndef UTILS_H
#define UTILS_H

#include <fastware/data_types.h>

namespace fastware {

namespace buffer {

void create(const buffer_create_info_t *infos, uint32_t count,
            uint32_t *buffer_ids);

void update(const buffer_update_info_t *infos, uint32_t count);

void destroy(const uint32_t *buffer_ids, uint32_t count);

} // namespace buffer

namespace varray {

void create(const vertex_array_definition_t *infos, uint32_t count,
            uint32_t *varray_ids);
void destroy(const uint32_t *varray_ids, uint32_t count);

} // namespace varray

namespace present {

void render(uint32_t varray_id, uint32_t offset, uint32_t vertex_count,
            primitive_type_e type);

void render_indexed(uint32_t varray_id, uint32_t offset, uint32_t index_count,
                    primitive_type_e type);

void render_indexed_instanced(uint32_t varray_id, uint32_t offset,
                              uint32_t index_count, uint32_t instance_count,
                              primitive_type_e type);
} // namespace present

namespace program {

uint32_t create(const shader_source_t *shaders, uint32_t count);

void destroy(uint32_t program_id);

void select(uint32_t program_id);

} // namespace program

namespace uniform {

int32_t resolve_uniform(uint32_t prog_id, const char *name);

uint32_t resolve_uniform_block(uint32_t prog_id, const char *uniformBlockName);

void resolve_block(uint32_t prog_id, const char *uniformBlockName,
                   int32_t binding_point);

void bind_buffer(uint32_t binding_point, uint32_t buffer_id);

void set_sampler_value(uint32_t program_id, uint32_t location, int32_t value);

void set_value(uint32_t program_id, uint32_t location, int32_t value);

void set_value(uint32_t program_id, uint32_t location, uint32_t value);

void set_value(uint32_t program_id, uint32_t location, float value);

void set_value(uint32_t program_id, uint32_t location, const glm::vec3 &value);

void set_value(uint32_t program_id, uint32_t location, const glm::mat4 &value);

} // namespace uniform

namespace texture {
void create_2d(const texture_create_info_t *infos, uint32_t count,
               uint32_t *tex_ids);

void bind(uint32_t start_index, uint32_t count, const uint32_t *tex_ids);

void destroy(const uint32_t *tex_ids, uint32_t count);

} // namespace texture

namespace sampler {
void create(const sampler_create_info_t *infos, uint32_t count,
            uint32_t *samp_ids);

void bind(uint32_t start_index, uint32_t count, const uint32_t *samp_ids);

void destroy(const uint32_t *samp_ids, uint32_t count);

} // namespace sampler

} // namespace fastware

#endif // UTILS_H
