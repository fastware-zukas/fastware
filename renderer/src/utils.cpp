#include <fastware/utils.h>

#include <GL/glew.h>

#include <cstdio>
#include <utility>

#include <fastware/debug.h>

#include <fastware/logger.h>
#include <fastware/types.h>

namespace fastware {

constexpr GLenum select(buffer_target target) {
  switch (target) {
  case buffer_target::ARRAY_BUFFER:
    return GL_ARRAY_BUFFER;
  case buffer_target::ATOMIC_COUNTER_BUFFER:
    return GL_ATOMIC_COUNTER_BUFFER;
  case buffer_target::COPY_READ_BUFFER:
    return GL_COPY_READ_BUFFER;
  case buffer_target::COPY_WRITE_BUFFER:
    return GL_COPY_WRITE_BUFFER;
  case buffer_target::DISPATCH_INDIRECT_BUFFER:
    return GL_DISPATCH_INDIRECT_BUFFER;
  case buffer_target::DRAW_INDIRECT_BUFFER:
    return GL_DRAW_INDIRECT_BUFFER;
  case buffer_target::ELEMENT_ARRAY_BUFFER:
    return GL_ELEMENT_ARRAY_BUFFER;
  case buffer_target::PIXEL_PACK_BUFFER:
    return GL_PIXEL_PACK_BUFFER;
  case buffer_target::PIXEL_UNPACK_BUFFER:
    return GL_PIXEL_UNPACK_BUFFER;
  case buffer_target::QUERY_BUFFER:
    return GL_QUERY_BUFFER;
  case buffer_target::SHADER_STORAGE_BUFFER:
    return GL_SHADER_STORAGE_BUFFER;
  case buffer_target::TEXTURE_BUFFER:
    return GL_TEXTURE_BUFFER;
  case buffer_target::TRANSFORM_FEEDBACK_BUFFER:
    return GL_TRANSFORM_FEEDBACK_BUFFER;
  case buffer_target::UNIFORM_BUFFER:
    return GL_UNIFORM_BUFFER;
  }
  return 0;
}

constexpr GLenum select(buffer_type_e type) {

  switch (type) {
  case buffer_type_e::NONE: {
    assert(false && "Cannot use NONE buffer type");
    return 0;
  }
  case buffer_type_e::STREAM:
    return GL_STREAM_DRAW;
  case buffer_type_e::STATIC:
    return GL_STATIC_DRAW;
  case buffer_type_e::DYNAMIC:
    return GL_DYNAMIC_DRAW;
  }

  return GL_STATIC_DRAW;
}

GLuint select(data_rate_e e) {
  switch (e) {
  case data_rate_e::PER_VERTEX:
    return 0;
  case data_rate_e::PER_INSTANCE:
    return 1;
  case data_rate_e::PER_INSTANCE_2:
    return 2;
  case data_rate_e::PER_INSTANCE_4:
    return 4;
  }

  return 0;
}

struct attr_req_t {
  int32_t number_of_attributes;
  int32_t count;
  int32_t type_size;
  GLenum type;
} attr_packed;

constexpr attr_req_t select(data_type_e e) {
  switch (e) {
  case data_type_e::SHORT:
    return {1, 1, sizeof(GLshort), GL_SHORT};
  case data_type_e::UNSIGNED_SHORT:
    return {1, 1, sizeof(GLushort), GL_UNSIGNED_SHORT};
  case data_type_e::INT:
    return {1, 1, sizeof(GLint), GL_INT};
  case data_type_e::UNSIGNED_INT:
    return {1, 1, sizeof(GLuint), GL_UNSIGNED_INT};
  case data_type_e::FLOAT:
    return {1, 1, sizeof(GLfloat), GL_FLOAT};
  case data_type_e::DOUBLE:
    return {1, 1, sizeof(GLdouble), GL_DOUBLE};
  case data_type_e::FLOAT_VECTOR2:
    return {1, 2, sizeof(GLfloat), GL_FLOAT};
  case data_type_e::FLOAT_VECTOR3:
    return {1, 3, sizeof(GLfloat), GL_FLOAT};
  case data_type_e::FLOAT_VECTOR4:
    return {1, 4, sizeof(GLfloat), GL_FLOAT};
  case data_type_e::FLOAT_MATRIX3:
    return {3, 3, sizeof(GLfloat), GL_FLOAT};
  case data_type_e::FLOAT_MATRIX4:
    return {4, 4, sizeof(GLfloat), GL_FLOAT};
  }

  return {1, 1, sizeof(GLshort), GL_SHORT};
}

void buffer::create(const buffer_create_info_t *infos, uint32_t count,
                    uint32_t *buffer_ids) {
  glCreateBuffers(static_cast<GLsizei>(count), buffer_ids);
  for (int32_t i = 0, c = static_cast<int32_t>(count); i < c; ++i) {
    GLCALL(glNamedBufferData, buffer_ids[i],
           static_cast<GLsizei>(infos[i].size), infos[i].data,
           select(infos[i].type));
  }
}

void buffer::update(const buffer_update_info_t *infos, uint32_t count) {
  for (int32_t i = 0, c = static_cast<int32_t>(count); i < c; ++i) {
    GLCALL(glNamedBufferSubData, infos[i].buffer_id, infos[i].offset,
           infos[i].size, infos[i].data);
  }
}

void buffer::destroy(const uint32_t *buffer_ids, uint32_t count) {
  GLCALL(glDeleteBuffers, static_cast<GLsizei>(count), buffer_ids);
}

void varray::create(const vertex_array_definition_t *infos, uint32_t count,
                    uint32_t *varray_ids) {
  GLCALL(glCreateVertexArrays, static_cast<GLsizei>(count), varray_ids);
  for (int32_t n = 0, cn = static_cast<int32_t>(count); n < cn; ++n) {
    const vertex_array_definition_t &info = infos[n];
    GLCALL(glBindVertexArray, varray_ids[n]);
    int32_t attribute_idx{0};
    for (int32_t i = 0, ci = static_cast<int32_t>(info.buffer_definition_count);
         i < ci; ++i) {
      const vertex_buffer_definition_t &buffer_def = info.buffer_definitions[i];

      GLCALL(glBindBuffer, GL_ARRAY_BUFFER, buffer_def.buffer_id);
      const uint32_t divisor = select(buffer_def.data_rate);
      int32_t offset = 0;
      for (int32_t j = 0, cj = static_cast<int32_t>(
                              buffer_def.section_definition_count);
           j < cj; ++j) {
        const vertex_buffer_section_definition_t &buffer_section_def =
            buffer_def.section_definitions[j];
        const GLboolean normalize =
            buffer_section_def.normalize == normalize_e::YES ? GL_TRUE
                                                             : GL_FALSE;
        const attr_req_t req = select(buffer_section_def.type);
        const int32_t stride =
            req.count * req.number_of_attributes * req.type_size;

        for (int32_t k = 0, ck = static_cast<int32_t>(req.number_of_attributes);
             k < ck; ++k) {
          const uint32_t effective_attr_index =
              static_cast<uint32_t>(attribute_idx + k);
          const int32_t local_offset = req.count * req.type_size * k;
          const void *applied_offset =
              reinterpret_cast<void *>(offset + local_offset);
          GLCALL(glVertexAttribPointer, effective_attr_index, req.count,
                 req.type, normalize, stride, applied_offset);
          GLCALL(glEnableVertexAttribArray, effective_attr_index);
          GLCALL(glVertexAttribDivisor, effective_attr_index, divisor);
        }
        attribute_idx += req.number_of_attributes;
        offset += stride * buffer_def.data_element_count;
      }
    }

    if (info.index_buffer_id) {
      GLCALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, info.index_buffer_id);
    }
  }
  GLCALL(glBindVertexArray, 0);
}

void varray::destroy(const uint32_t *varray_ids, uint32_t count) {
  GLCALL(glDeleteVertexArrays, static_cast<GLsizei>(count), varray_ids);
}

void present::render(uint32_t varray_id, uint32_t vertex_count) {
  GLCALL(glBindVertexArray, varray_id);
  GLCALL(glDrawArrays, GL_TRIANGLES, 0, static_cast<GLsizei>(vertex_count));
}

void present::render_indexed(uint32_t varray_id, uint32_t index_count) {
  GLCALL(glBindVertexArray, varray_id);
  GLCALL(glDrawElements, GL_TRIANGLES, static_cast<GLsizei>(index_count),
         GL_UNSIGNED_INT, nullptr);
}

void present::render_indexed_instanced(uint32_t varray_id, uint32_t index_count,
                                       uint32_t instance_count) {
  GLCALL(glBindVertexArray, varray_id);
  GLCALL(glDrawElementsInstanced, GL_TRIANGLES,
         static_cast<GLsizei>(index_count), GL_UNSIGNED_INT, nullptr,
         static_cast<GLsizei>(instance_count));
}

uint32_t program::create(const fastware::shader_source_t *shaders,
                         uint32_t count) {
  uint32_t program_id = glCreateProgram();

  for (int32_t i = 0, ci = static_cast<int32_t>(count); i < ci; ++i) {
    const shader_source_t &s = shaders[i];

    GLuint shader_id{0};
    switch (s.type) {
    case shader_type::VERTEX:
      shader_id = glCreateShader(GL_VERTEX_SHADER);
      break;
    case shader_type::FRAGMENT:
      shader_id = glCreateShader(GL_FRAGMENT_SHADER);
      break;
    case shader_type::GEOMETRY:
      shader_id = glCreateShader(GL_GEOMETRY_SHADER);
      break;
    case shader_type::TESS_CONTROL:
      shader_id = glCreateShader(GL_TESS_CONTROL_SHADER);
      break;
    case shader_type::TESS_EVALUATION:
      shader_id = glCreateShader(GL_TESS_EVALUATION_SHADER);
      break;
    case shader_type::COMPUTE:
      shader_id = glCreateShader(GL_COMPUTE_SHADER);
      break;
    }

    GLint size = static_cast<GLint>(s.length);
    glShaderSource(shader_id, 1, &s.glsl_source, &size);
    glCompileShader(shader_id);

    GLint status{GL_FALSE};
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (!status) {
      int log_length{0};
      glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
      if (log_length > 0) {
        char error[4096]{0};
        glGetShaderInfoLog(shader_id, log_length, nullptr, error);
        fastware::logger::log(
            "Failed to compile shader\n%s\nCompile error: %s\n", s.glsl_source,
            error);
      }
      exit(1);
    } else {
      glAttachShader(program_id, shader_id);
    }
    glDeleteShader(shader_id);
  }

  glLinkProgram(program_id);
  GLint status{GL_FALSE};
  glGetProgramiv(program_id, GL_LINK_STATUS, &status);
  if (!status) {
    int logLength{0};
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      char error[2048]{0};
      glGetProgramInfoLog(program_id, logLength, nullptr, error);
      fastware::logger::log("Program link error: %s\n", error);
    }
    glDeleteProgram(program_id);
    exit(1);
  }

  {
    // DEBUG STUFF
    GLint i;
    GLint count;

    GLint size;  // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 64; // maximum name length
    GLchar name[bufSize];       // variable name in GLSL
    GLsizei length;             // name length

    glGetProgramiv(program_id, GL_ACTIVE_ATTRIBUTES, &count);
    fastware::logger::log("Active Attributes: %d", count);

    for (i = 0; i < count; i++) {
      glGetActiveAttrib(program_id, static_cast<GLuint>(i), bufSize, &length,
                        &size, &type, name);

      fastware::logger::log("Attribute #%d Type: %u Name: %s, Location: %d", i,
                            type, name, glGetAttribLocation(program_id, name));
    }

    glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &count);
    fastware::logger::log("Active Uniforms: %d", count);

    for (i = 0; i < count; i++) {
      glGetActiveUniform(program_id, static_cast<GLuint>(i), bufSize, &length,
                         &size, &type, name);

      fastware::logger::log("Uniform #%d Type: %u Name: %s, Location: %d", i,
                            type, name, glGetUniformLocation(program_id, name));
    }
  }

  glUseProgram(0);

  return program_id;
}

void program::destroy(uint32_t program_id) { glDeleteProgram(program_id); }

void program::select(uint32_t program_id) { glUseProgram(program_id); }

int32_t uniform::resolve_uniform(uint32_t prog_id, const char *name) {
  return glGetUniformLocation(prog_id, name);
}

uint32_t uniform::resolve_uniform_block(uint32_t prog_id,
                                        const char *uniformBlockName) {
  return glGetUniformBlockIndex(prog_id, uniformBlockName);
}

void uniform::resolve_block(uint32_t prog_id, const char *uniformBlockName,
                            int32_t binding_point) {
  uint32_t block_id = glGetUniformBlockIndex(prog_id, uniformBlockName);
  glUniformBlockBinding(prog_id, block_id, binding_point);
}

void uniform::bind_buffer(uint32_t binding_point, uint32_t buffer_id) {
  glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, buffer_id);
}

void uniform::set_value(uint32_t program_id, uint32_t location, int32_t value) {
  glProgramUniform1i(program_id, location, value);
}

void uniform::set_value(uint32_t program_id, uint32_t location,
                        uint32_t value) {
  glProgramUniform1ui(program_id, location, value);
}

void uniform::set_value(uint32_t program_id, uint32_t location, float value) {
  glProgramUniform1f(program_id, location, value);
}

void uniform::set_value(uint32_t program_id, uint32_t location,
                        const glm::vec3 &value) {
  glProgramUniform3fv(program_id, location, 1, &value[0]);
}

void uniform::set_value(uint32_t program_id, uint32_t location,
                        const glm::mat4 &value) {
  glProgramUniformMatrix4fv(program_id, location, 1, GL_FALSE, &value[0][0]);
}

static constexpr GLenum convert(wrap_e value) {
  switch (value) {
  case wrap_e::CLAMP_TO_EDGE:
    return GL_CLAMP_TO_EDGE;
  case wrap_e::CLAMP_TO_BORDER:
    return GL_CLAMP_TO_BORDER;
  case wrap_e::MIRRORED_REPEAT:
    return GL_MIRRORED_REPEAT;
  case wrap_e::REPEAT:
    return GL_REPEAT;
  case wrap_e::MIRROR_CLAMP_TO_EDGE:
    return GL_MIRROR_CLAMP_TO_EDGE;
  }
  return GL_CLAMP_TO_EDGE;
}

static constexpr GLenum convert(mag_filter_e value) {
  switch (value) {
  case mag_filter_e::NEAREST:
    return GL_NEAREST;
  case mag_filter_e::LINEAR:
    return GL_LINEAR;
  }
  return GL_NEAREST;
}

static constexpr GLenum convert(min_filter_e value) {
  switch (value) {
  case min_filter_e::NEAREST:
    return GL_NEAREST;
  case min_filter_e::LINEAR:
    return GL_LINEAR;
  case min_filter_e::NEAREST_MIPMAP_NEAREST:
    return GL_NEAREST_MIPMAP_NEAREST;
  case min_filter_e::LINEAR_MIPMAP_NEAREST:
    return GL_LINEAR_MIPMAP_NEAREST;
  case min_filter_e::NEAREST_MIPMAP_LINEAR:
    return GL_NEAREST_MIPMAP_LINEAR;
  case min_filter_e::LINEAR_MIPMAP_LINEAR:
    return GL_LINEAR_MIPMAP_LINEAR;
  }
  return GL_NEAREST;
}

static constexpr GLenum convert(ds_mode value) {
  switch (value) {
  case ds_mode::DEPTH:
    return GL_DEPTH_COMPONENT;
  case ds_mode::STENCIL:
    return GL_STENCIL_INDEX;
  }
  return GL_DEPTH_COMPONENT;
}

static constexpr std::pair<GLenum, GLenum>
convert(graphics::texture_format value) {
  switch (value) {
  case graphics::texture_format::R8:
    return {GL_R8, GL_RED};
  case graphics::texture_format::R8_SNORM:
    return {GL_R8_SNORM, GL_RED};
  case graphics::texture_format::R16:
    return {GL_R16, GL_RED};
  case graphics::texture_format::R16_SNORM:
    return {GL_R16_SNORM, GL_RED};
  case graphics::texture_format::RG8:
    return {GL_RG8, GL_RG};
  case graphics::texture_format::RG8_SNORM:
    return {GL_RG8_SNORM, GL_RG};
  case graphics::texture_format::RG16:
    return {GL_RG16, GL_RG};
  case graphics::texture_format::RG16_SNORM:
    return {GL_RG16_SNORM, GL_RG};
  case graphics::texture_format::R3_G3_B2:
    return {GL_R3_G3_B2, GL_RGB};
  case graphics::texture_format::RGB4:
    return {GL_RGB4, GL_RGB};
  case graphics::texture_format::RGB5:
    return {GL_RGB5, GL_RGB};
  case graphics::texture_format::RGB8:
    return {GL_RGB8, GL_RGB};
  case graphics::texture_format::RGB8_SNORM:
    return {GL_RGB8_SNORM, GL_RGB};
  case graphics::texture_format::RGB10:
    return {GL_RGB10, GL_RGB};
  case graphics::texture_format::RGB12:
    return {GL_RGB12, GL_RGB};
  case graphics::texture_format::RGB16_SNORM:
    return {GL_RGB16_SNORM, GL_RGB};
  case graphics::texture_format::RGBA2:
    return {GL_RGBA2, GL_RGBA};
  case graphics::texture_format::RGBA4:
    return {GL_RGBA4, GL_RGBA};
  case graphics::texture_format::RGB5_A1:
    return {GL_RGB5_A1, GL_RGBA};
  case graphics::texture_format::RGBA8:
    return {GL_RGBA8, GL_RGBA};
  case graphics::texture_format::RGBA8_SNORM:
    return {GL_RGBA8_SNORM, GL_RGBA};
  case graphics::texture_format::RGB10_A2:
    return {GL_RGB10_A2, GL_RGBA};
  case graphics::texture_format::RGB10_A2UI:
    return {GL_RGB10_A2UI, GL_RGBA};
  case graphics::texture_format::RGBA12:
    return {GL_RGBA12, GL_RGBA};
  case graphics::texture_format::RGBA16:
    return {GL_RGBA16, GL_RGBA};
  case graphics::texture_format::SRGB8:
    return {GL_SRGB8, GL_RGB};
  case graphics::texture_format::SRGB8_ALPHA8:
    return {GL_SRGB8_ALPHA8, GL_RGBA};
  case graphics::texture_format::R16F:
    return {GL_R16F, GL_RED};
  case graphics::texture_format::RG16F:
    return {GL_RG16F, GL_RG};
  case graphics::texture_format::RGB16F:
    return {GL_RGB16F, GL_RGB};
  case graphics::texture_format::RGBA16F:
    return {GL_RGBA16F, GL_RGBA};
  case graphics::texture_format::R32F:
    return {GL_R32F, GL_RED};
  case graphics::texture_format::RG32F:
    return {GL_RG32F, GL_RG};
  case graphics::texture_format::RGB32F:
    return {GL_RGB32F, GL_RGB};
  case graphics::texture_format::RGBA32F:
    return {GL_RGBA32F, GL_RGBA};
  case graphics::texture_format::R11F_G11F_B10F:
    return {GL_R11F_G11F_B10F, GL_RGB};
  case graphics::texture_format::RGB9_E5:
    return {GL_RGB9_E5, GL_RGB};
  case graphics::texture_format::R8I:
    return {GL_R8I, GL_RED};
  case graphics::texture_format::R8UI:
    return {GL_R8UI, GL_RED};
  case graphics::texture_format::R16I:
    return {GL_R16I, GL_RED};
  case graphics::texture_format::R16UI:
    return {GL_R16UI, GL_RED};
  case graphics::texture_format::R32I:
    return {GL_R32I, GL_RED};
  case graphics::texture_format::R32UI:
    return {GL_R32UI, GL_RED};
  case graphics::texture_format::RG8I:
    return {GL_RG8I, GL_RGB};
  case graphics::texture_format::RG8UI:
    return {GL_RG8UI, GL_RG};
  case graphics::texture_format::RG16I:
    return {GL_RG16I, GL_RG};
  case graphics::texture_format::RG16UI:
    return {GL_RG16UI, GL_RG};
  case graphics::texture_format::RG32I:
    return {GL_RG32I, GL_RGB};
  case graphics::texture_format::RG32UI:
    return {GL_RG32UI, GL_RG};
  case graphics::texture_format::RGB8I:
    return {GL_RGB8I, GL_RG};
  case graphics::texture_format::RGB8UI:
    return {GL_RGB8UI, GL_RGB};
  case graphics::texture_format::RGB16I:
    return {GL_RGB16I, GL_RGB};
  case graphics::texture_format::RGB16UI:
    return {GL_RGB16UI, GL_RGB};
  case graphics::texture_format::RGB32I:
    return {GL_RGB32I, GL_RGB};
  case graphics::texture_format::RGB32UI:
    return {GL_RGB32UI, GL_RGB};
  case graphics::texture_format::RGBA8I:
    return {GL_RGBA8I, GL_RGBA};
  case graphics::texture_format::RGBA8UI:
    return {GL_RGBA8UI, GL_RGBA};
  case graphics::texture_format::RGBA16I:
    return {GL_RGBA16I, GL_RGBA};
  case graphics::texture_format::RGBA16UI:
    return {GL_RGBA16UI, GL_RGBA};
  case graphics::texture_format::RGBA32I:
    return {GL_RGBA32I, GL_RGBA};
  case graphics::texture_format::RGBA32UI:
    return {GL_RGBA32UI, GL_RGBA};
  }

  return {GL_R8, GL_RED};
}

enum class param_setter_type { TEXTURE, SAMPLER };

template <param_setter_type> struct parameter;

template <> struct parameter<param_setter_type::TEXTURE> {

  static void set(uint32_t object_id, GLenum param, GLenum value) {
    glTextureParameteri(object_id, param, value);
  }

  static void set(uint32_t object_id, GLenum param, int32_t value) {
    glTextureParameteri(object_id, param, value);
  }

  static void set(uint32_t object_id, GLenum param, float_t value) {
    glTextureParameterf(object_id, param, value);
  }

  static void set(uint32_t object_id, GLenum param, const int32_t *values) {
    glTextureParameteriv(object_id, param, values);
  }

  static void set(uint32_t object_id, GLenum param, const float_t *values) {
    glTextureParameterfv(object_id, param, values);
  }
};

template <> struct parameter<param_setter_type::SAMPLER> {

  static void set(uint32_t object_id, GLenum param, GLenum value) {
    glSamplerParameteri(object_id, param, value);
  }

  static void set(uint32_t object_id, GLenum param, int32_t value) {
    glSamplerParameteri(object_id, param, value);
  }

  static void set(uint32_t object_id, GLenum param, float_t value) {
    glSamplerParameterf(object_id, param, value);
  }

  static void set(uint32_t object_id, GLenum param, const int32_t *values) {
    glSamplerParameteriv(object_id, param, values);
  }

  static void set(uint32_t object_id, GLenum param, const float_t *values) {
    glSamplerParameterfv(object_id, param, values);
  }
};

template <param_setter_type T>
static void set_params(uint32_t texture_id, const param_info_t *params,
                       uint32_t count) {
  for (int32_t j = 0, cj = static_cast<int32_t>(count); j < cj; ++j) {
    const param_info_t &param = params[j];

    switch (param.type()) {
    case parameter_type::WRAP_S: {
      parameter<T>::set(texture_id, GL_TEXTURE_WRAP_S,
                        convert(param.wrap.wrap));
      break;
    }
    case parameter_type::WRAP_T: {
      parameter<T>::set(texture_id, GL_TEXTURE_WRAP_T,
                        convert(param.wrap.wrap));
      break;
    }
    case parameter_type::WRAP_R: {
      parameter<T>::set(texture_id, GL_TEXTURE_WRAP_R,
                        convert(param.wrap.wrap));
      break;
    }
    case parameter_type::MIN_FILTER: {
      parameter<T>::set(texture_id, GL_TEXTURE_MIN_FILTER,
                        convert(param.min_filter.filter));
      break;
    }
    case parameter_type::MAG_FILTER: {
      parameter<T>::set(texture_id, GL_TEXTURE_MAG_FILTER,
                        convert(param.mag_filter.filter));
      break;
    }
    case parameter_type::DS_MODE: {
      parameter<T>::set(texture_id, GL_DEPTH_STENCIL_TEXTURE_MODE,
                        convert(param.ds_mode.mode));
      break;
    }
    }
  }
}

void texture::create_2d(const texture_create_info_t *infos, uint32_t count,
                        uint32_t *tex_ids) {
  GLCALL(glCreateTextures, GL_TEXTURE_2D, 1, tex_ids);

  for (int32_t i = 0, ci = static_cast<int32_t>(count); i < ci; ++i) {
    const uint32_t texture_id = tex_ids[i];
    const texture_create_info_t &tex = infos[i];

    set_params<param_setter_type::TEXTURE>(texture_id, tex.param_infos,
                                           tex.param_info_count);
    GLCALL(glTextureStorage2D, texture_id, 1, convert(tex.format).first,
           tex.width, tex.height);
    GLCALL(glTextureSubImage2D, texture_id, 0, 0, 0, tex.width, tex.height,
           convert(tex.format).second, GL_UNSIGNED_BYTE, tex.data);

    GLCALL(glGenerateTextureMipmap, texture_id);
  }
}

void texture::bind(uint32_t start_index, uint32_t count,
                   const uint32_t *tex_ids) {
  GLCALL(glBindTextures, start_index, count, tex_ids);
}

void texture::destroy(const uint32_t *tex_ids, uint32_t count) {
  GLCALL(glDeleteTextures, static_cast<int32_t>(count), tex_ids);
}

void sampler::create(const sampler_create_info_t *infos, uint32_t count,
                     uint32_t *samp_ids) {
  GLCALL(glCreateSamplers, count, samp_ids);
  for (int32_t i = 0, ci = static_cast<int32_t>(count); i < ci; ++i) {
    const uint32_t sampler_id = samp_ids[i];
    const sampler_create_info_t &tex = infos[i];
    set_params<param_setter_type::SAMPLER>(sampler_id, tex.param_infos,
                                           tex.param_info_count);
  }
}

void sampler::bind(uint32_t start_index, uint32_t count,
                   const uint32_t *samp_ids) {
  GLCALL(glBindSamplers, start_index, count, samp_ids);
}

void sampler::destroy(const uint32_t *samp_ids, uint32_t count) {
  GLCALL(glDeleteSamplers, count, samp_ids);
}

} // namespace fastware
