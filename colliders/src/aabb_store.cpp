#include <fastware/aabb_store.h>
#include <fastware/colliders.h>

namespace fastware {

namespace colliders {

struct aabb_node_t {
  aabb_t node;
  aabb_t *next;
};

struct aabb_store_t {
  memory::allocator_t *allocator;
  int32_t count;
  aabb_node_t *store;
};

aabb_store_t *create_aabb_store(aabb_store_create_info_t *info) {

  return nullptr;
}

void destroy_aabb_store(aabb_store_t *store) {}

int32_t insert_aabbs(aabb_store_t *store, aabb_t *aabbs, int32_t count) {
  return 0;
}

void remove_aabbs(aabb_store_t *store, int32_t *idxs, int32_t count) {}

aabb_t *get_aabb(aabb_store_t *store, int32_t idx) { return nullptr; }

int32_t get_aabbs(aabb_store_t *store, int32_t *idxs, int32_t count,
                  aabb_t *aabbs) {
  return 0;
}

} // namespace colliders
} // namespace fastware