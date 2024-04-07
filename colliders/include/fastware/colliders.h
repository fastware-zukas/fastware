#ifndef COLLIDERS_H
#define COLLIDERS_H

#include <fastware/fastware_def.h>

#include <cglm/cglm.h>

namespace fastware {

namespace colliders {

using aabb_t = vec3[2];

struct ray_t {
  vec3 pos;
  vec3 dir;
};

} // namespace colliders
} // namespace fastware

#endif // COLLIDERS_H
