#include "geometry.h"

#include <cstdlib>
#include <cstring>
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

  float hx = width * 0.5f;
  float hy = height * 0.5f;
  float hz = depth * 0.5f;

  vec3_t local_positions[]{// Front
                           {-hx, -hy, hz},
                           {hx, -hy, hz},
                           {hx, hy, hz},
                           {-hx, hy, hz},
                           // Back
                           {hx, -hy, -hz},
                           {-hx, -hy, -hz},
                           {-hx, hy, -hz},
                           {hx, hy, -hz},
                           // Left
                           {-hx, -hy, -hz},
                           {-hx, -hy, hz},
                           {-hx, hy, hz},
                           {-hx, hy, -hz},
                           // Right
                           {hx, -hy, hz},
                           {hx, -hy, -hz},
                           {hx, hy, -hz},
                           {hx, hy, hz},
                           // Top
                           {-hx, hy, hz},
                           {hx, hy, hz},
                           {hx, hy, -hz},
                           {-hx, hy, -hz},
                           // Bottom
                           {-hx, -hy, -hz},
                           {hx, -hy, -hz},
                           {hx, -hy, hz},
                           {-hx, -hy, hz}};

  memcpy(positions, local_positions, sizeof(local_positions));

  vec3_t local_normals[]{
      {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
      {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f},
      {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, -1.0f, 0.0f},
      {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
  };

  memcpy(normals, local_normals, sizeof(local_normals));

  vec2_t local_uvs[]{{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                     {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                     {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                     {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                     {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                     {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

  memcpy(uvs, local_uvs, sizeof(local_uvs));

  uint32_t local_indexes[]{
      0,  1,  2,  2,  3,  0,  // Front
      4,  5,  6,  6,  7,  4,  // Back
      8,  9,  10, 10, 11, 8,  // Left
      12, 13, 14, 14, 15, 12, // Right
      16, 17, 18, 18, 19, 16, // Top
      20, 21, 22, 22, 23, 20  // Bottom
  };

  memcpy(indexes, local_indexes, sizeof(local_indexes));
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