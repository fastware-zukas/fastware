#ifndef GEOMETRY_H
#define GEOMETRY_H

#pragma once

#include <fastware/fastware_def.h>
#include <fastware/types.h>

namespace fastware {

namespace geometry {
namespace sphere {
constexpr uint32_t vertex_count(uint32_t units) {
  return (units + 1) * (units + 1);
}
constexpr uint32_t index_count(uint32_t units) {
  return (units) * (units - 1) * 6;
}

void generate(vec3_t *positions, vec3_t *normals, vec2_t *uvs,
              uint32_t *indexes, uint32_t units);

} // namespace sphere

namespace rect {

void generate(vec3_t *positions, vec3_t *normals, vec2_t *uvs,
              uint32_t *indexes, uint32_t width, uint32_t height,
              uint32_t depth);

} // namespace rect

namespace matrix {

void fill(mat4_t *matrixes, int32_t count, mat4_t value);
}

} // namespace geometry

} // namespace fastware

#endif