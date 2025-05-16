#ifndef AABB_STORE_H
#define AABB_STORE_H

#include <fastware/colliders.h>
#include <fastware/fastware_def.h>
#include <fastware/memory.h>
#include <fastware/types.h>

namespace fastware {

namespace colliders {

struct aabb_store_create_info_t {
  memory::allocator_t *allocator;
  int32_t count;
};

struct aabb_store_t;

aabb_store_t *create_aabb_store(aabb_store_create_info_t *info);

void destroy_aabb_store(aabb_store_t *store);

int32_t insert_aabbs(aabb_store_t *store, aabb_t *aabbs, int32_t count);

void remove_aabbs(aabb_store_t *store, int32_t *idxs, int32_t count);

aabb_t *get_aabb(aabb_store_t *store, int32_t idx);

int32_t get_aabbs(aabb_store_t *store, int32_t *idxs, int32_t count,
                  aabb_t *aabbs);

} // namespace colliders
} // namespace fastware

#endif // AABB_STORE_H
