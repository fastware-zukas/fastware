#ifndef AABB_STORE_H
#define AABB_STORE_H

#include <fastware/colliders.h>
#include <fastware/fastware_def.h>
#include <fastware/memory.h>

#include <cglm/cglm.h>

namespace fastware {

namespace colliders {

struct aabb_store_create_info_t {
  memory::allocator_t *allocator;
  int32_t count;
};

struct aabb_store_t;

aabb_store_t *create_aabb_store(aabb_store_create_info_t *info);

void destroy_aabb_store(aabb_store_t *store);

} // namespace colliders
} // namespace fastware

#endif // AABB_STORE_H
