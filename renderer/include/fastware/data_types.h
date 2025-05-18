#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstddef>
#include <cstdint>

#include <fastware/memory.h>
#include <fastware/types.h>

namespace fastware {

enum class shader_type_e {
  VERTEX,
  FRAGMENT,
  GEOMETRY,
  TESS_CONTROL,
  TESS_EVALUATION,
  COMPUTE
};

enum class primitive_type_e {
  POINTS,
  LINES,
  LINE_LOOP,
  LINE_STRIP,
  TRIANGLES,
  TRIANGLE_STRIP,
  TRIANGLE_FAN,
  LINES_ADJACENCY,
  LINE_STRIP_ADJACENCY,
  TRIANGLES_ADJACENCY,
  TRIANGLE_STRIP_ADJACENCY
};

enum class buffer_target_e {
  ARRAY_BUFFER,              // Vertex attributes
  ATOMIC_COUNTER_BUFFER,     // Atomic counter storage
  COPY_READ_BUFFER,          // Buffer copy source
  COPY_WRITE_BUFFER,         // Buffer copy destination
  DISPATCH_INDIRECT_BUFFER,  // Indirect compute dispatch commands
  DRAW_INDIRECT_BUFFER,      // Indirect command arguments
  ELEMENT_ARRAY_BUFFER,      // Vertex array indices
  PIXEL_PACK_BUFFER,         // Pixel read target
  PIXEL_UNPACK_BUFFER,       // Texture data source
  QUERY_BUFFER,              // Query result buffer
  SHADER_STORAGE_BUFFER,     // Read-write storage for shaders
  TEXTURE_BUFFER,            // Texture data buffer
  TRANSFORM_FEEDBACK_BUFFER, // Transform feedback buffer
  UNIFORM_BUFFER             // Uniform block storage
};

enum class buffer_type_e {
  NONE,
  STREAM,  // The data store contents will be modified once and used at most a
           // few times.
  STATIC,  // The data store contents will be modified once and used many times.
  DYNAMIC, // The data store contents will be modified repeatedly and used many
           // times.
};

enum class rate_e {
  PER_VERTEX = 0,
  PER_INSTANCE = 1,
  PER_INSTANCE_2 = 2,
  PER_INSTANCE_4 = 4
};

enum class wrap_e {
  CLAMP_TO_EDGE,
  CLAMP_TO_BORDER,
  MIRRORED_REPEAT,
  REPEAT,
  MIRROR_CLAMP_TO_EDGE
};

enum class mag_filter_e { NEAREST, LINEAR };

enum class min_filter_e {
  NEAREST,
  LINEAR,
  NEAREST_MIPMAP_NEAREST,
  LINEAR_MIPMAP_NEAREST,
  NEAREST_MIPMAP_LINEAR,
  LINEAR_MIPMAP_LINEAR
};

enum class ds_mode { DEPTH, STENCIL };

enum class parameter_type_e {
  WRAP_S,
  WRAP_T,
  WRAP_R,
  MIN_FILTER,
  MAG_FILTER,
  DS_MODE
};

struct shader_source_t {
  union {
    const char *glsl_source;
    const uint8_t *spir_v_source;
  };
  uint32_t length;
  shader_type_e type;
};

enum class data_type_e {
  SHORT,
  UNSIGNED_SHORT,
  INT,
  UNSIGNED_INT,
  FLOAT,
  DOUBLE,
  FLOAT_VECTOR2,
  FLOAT_VECTOR3,
  FLOAT_VECTOR4,
  FLOAT_MATRIX3,
  FLOAT_MATRIX4,

  SAMPLER_2D = UNSIGNED_INT, // EXTENDED DATA TYPE DEFINITION
  SAMPLER_3D = UNSIGNED_INT, // EXTENDED DATA TYPE DEFINITION
  BLOCK = UNSIGNED_INT       // EXTENDED DATA TYPE DEFINITION
};

enum class normalize_e { NO, YES };

enum class data_rate_e {
  PER_VERTEX,
  PER_INSTANCE,
  PER_INSTANCE_2,
  PER_INSTANCE_4
};

struct shader_uniform_definition_t {
  data_type_e data_type;
  const char *name;
};

struct vertex_buffer_section_definition_t {
  data_type_e type;
  normalize_e normalize{normalize_e::NO};
};

struct vertex_buffer_definition_t {
  uint32_t buffer_id;
  data_rate_e data_rate;
  uint32_t data_element_count;
  uint32_t section_definition_count;
  const vertex_buffer_section_definition_t *section_definitions;
};

struct vertex_array_definition_t {
  const vertex_buffer_definition_t *buffer_definitions;
  int32_t buffer_definition_count;
  uint32_t index_buffer_id;
};

struct vertex_item_description_t {
  data_type_e type;
  uint32_t size;
  uint32_t normalize;
};

struct buffer_create_info_t {
  buffer_target_e target{buffer_target_e::ARRAY_BUFFER};
  buffer_type_e type{buffer_type_e::NONE};
  uint32_t size{0};
  const void *data{nullptr};
};

struct buffer_update_info_t {
  uint32_t buffer_id{0};
  uint32_t offset{0};
  uint32_t size{0};
  const void *data{nullptr};
};

struct param_info_t {
  parameter_type_e type;
  union {
    wrap_e wrap;
    min_filter_e min_filter;
    mag_filter_e mag_filter;
    ds_mode mode;
  };
};

struct subtexture_create_info_t {
  uint32_t width{0};
  uint32_t height{0};
  graphics::texture_format_e format{graphics::texture_format_e::RGBA8};
  const void *data{nullptr};
};

struct texture_create_info_t {
  uint32_t width{0};
  uint32_t height{0};
  graphics::texture_format_e format{graphics::texture_format_e::RGBA8};
  graphics::pixel_align_e pixel_pack_align{graphics::pixel_align_e::DEFAULT};
  graphics::pixel_align_e pixel_unpack_align{graphics::pixel_align_e::DEFAULT};
  const param_info_t *param_infos{nullptr};
  const subtexture_create_info_t *sub_texture{nullptr};
  uint32_t param_info_count{0};
  uint32_t sub_texture_count{0};
};

struct sampler_create_info_t {
  uint32_t param_info_count{0};
  const param_info_t *param_infos{nullptr};
};

struct character_t {
  vec2i_t size{0, 0};
  vec2i_t bearing{0, 0};
  vec2_t top_left{0, 0};
  vec2_t bottom_right{0, 0};
  uint32_t advance{0};
};

struct text_atlas_t {
  uint32_t texture_id{0};
  uint32_t width{0};
  uint32_t height{0};
  character_t chars[128];
};

struct text_buffer_t {
  uint32_t varray_id{0};
  uint32_t array_buffer{0};
  uint32_t index_buffer{0};
  uint32_t section_size{0};
};

struct create_text_atlas_info_t {
  memory::allocator_t *alloc{nullptr};
  const char *font_file{nullptr};
};

struct update_text_buffer_info_t {
  text_buffer_t *buffer{nullptr};
  text_atlas_t *atlas{nullptr};
  const char *text{nullptr};
  uint32_t length{0};
  vec2_t pos{0.f, 0.f};
  float_t size{1.f};
  float_t rotation{0.f};
};

} // namespace fastware

#endif // DATA_TYPES_H
