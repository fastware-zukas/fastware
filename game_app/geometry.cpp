#include "geometry.h"

#include <fastware/maths.h>

namespace fastware {

namespace geometry {
namespace sphere {

void generate(vec3_t *positions, vec3_t *normals, vec2_t *uvs,
              uint32_t *indexes, uint32_t units) {

  int64_t steps = units;

  float x, y, z, xy; // vertex position
  float nx, ny, nz;  // vertex normal
  float s, t;        // vertex texCoord

  const float sectorStep = 2.f * PI / float(steps);
  const float stackStep = PI / float(steps);
  float sectorAngle, stackAngle;

  int64_t index = 0;

  for (int64_t i = 0; i <= steps; ++i) {
    stackAngle = PI / 2.f - i * stackStep; // starting from pi/2 to -pi/2
    xy = cosf(stackAngle);                 // r * cos(u)
    z = sinf(stackAngle);                  // r * sin(u)

    for (int64_t j = 0; j <= units; ++j) {
      sectorAngle = j * sectorStep;

      x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
      y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)

      nx = x;
      ny = y;
      nz = z;

      s = float(j) / float(steps);
      t = float(i) / float(steps);

      positions[index] = vec3_t{x, y, z};
      normals[index] = vec3_t{nx, ny, nz};
      uvs[index] = vec2_t{s, t};
      ++index;
    }
  }

  uint32_t k1 = 0;
  uint32_t k2 = 0;

  index = 0;

  for (int64_t i = 0; i < steps; ++i) {
    k1 = static_cast<uint32_t>(i * (steps + 1)); // beginning of current stack
    k2 = static_cast<uint32_t>(k1 + steps + 1);  // beginning of next stack

    for (int64_t j = 0; j < steps; ++j, ++k1, ++k2) {
      // 2 triangles per sector excluding first and last stacks
      // k1 => k2 => k1+1
      if (i != 0) {
        indexes[index++] = k1;
        indexes[index++] = k2;
        indexes[index++] = k1 + 1;
      }

      // k1+1 => k2 => k2+1
      if (i != (steps - 1)) {
        indexes[index++] = k1 + 1;
        indexes[index++] = k2;
        indexes[index++] = k2 + 1;
      }
    }
  }
}
} // namespace sphere

namespace rect {

void generate(vec3_t *positions, vec3_t *normals, vec2_t *uvs,
              uint32_t *indexes, uint32_t width, uint32_t height,
              uint32_t depth) {

  struct face_t {
    vec3_t n, u, v;
  };
  constexpr face_t faces[6] = {
      {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},   // Front (+Z)
      {{0, 0, -1}, {-1, 0, 0}, {0, 1, 0}}, // Back (-Z)
      {{1, 0, 0}, {0, 0, -1}, {0, 1, 0}},  // Right (+X)
      {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}},  // Left (-X)
      {{0, 1, 0}, {1, 0, 0}, {0, 0, -1}},  // Top (+Y)
      {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}}   // Bottom (-Y)
  };

  constexpr vec2_t quadUVs[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  constexpr vec2_t quadOffsets[4] = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};
  constexpr uint32_t idxOffsets[6]{0, 1, 2, 0, 2, 3};

  const vec3_t half{width * 0.5f, height * 0.5f, depth * 0.5f};

  for (uint32_t f = 0; f < 6; ++f) {

    for (uint32_t i = 0; i < 4; ++i) {
      const vec3_t u = glms_vec3_scale(faces[f].u, quadOffsets[i].x);
      const vec3_t v = glms_vec3_scale(faces[f].v, quadOffsets[i].y);
      const vec3_t uv = glms_vec3_add(u, v);
      const vec3_t dir = glms_vec3_add(faces[f].n, uv);
      const vec3_t pos = glms_vec3_mul(dir, half);
      const uint32_t idx = f * 6 + i;

      positions[idx] = pos;
      normals[idx] = faces[f].n;
      uvs[idx] = quadUVs[i];
    }

    const uint32_t baseOffset = f * 6;
    const uint32_t baseIdx = f * 4;

    for (uint32_t i = 0; i < 6; i++) {
      indexes[baseOffset + i] = baseIdx + idxOffsets[i];
    }
  }
}

} // namespace rect

namespace matrix {

void fill(mat4_t *matrixes, int32_t count, mat4_t value) {
  for (int32_t i = 0; i < count; ++i) {
    matrixes[i] = value;
  }
}

} // namespace matrix

} // namespace geometry

} // namespace fastware