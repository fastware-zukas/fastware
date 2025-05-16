#ifndef COLLIDERS_H
#define COLLIDERS_H

#include <fastware/fastware_def.h>

#include <fastware/types.h>

namespace fastware {

namespace colliders {

struct aabb_t {
  union {
    vec3_t raw[2];
    struct {
      vec3_t min;
      vec3_t max;
    };
  };
};

struct ray_t {
  vec3_t pos;
  vec3_t dir;
};

} // namespace colliders
} // namespace fastware

#endif // COLLIDERS_H
