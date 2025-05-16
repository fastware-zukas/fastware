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

namespace matrix {

void fill(mat4_t *matrixes, int32_t count, mat4_t value) {
  for (int32_t i = 0; i < count; ++i) {
    matrixes[i] = value;
  }
}

} // namespace matrix

} // namespace geometry

} // namespace fastware