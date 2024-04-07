#include <fastware/aabb_store.h>

namespace fastware {

namespace colliders {

struct aabb_store_t {
  memory::allocator_t *allocator;
  int32_t count;
  // aabb_t
};

aabb_store_t *create_aabb_store(aabb_store_create_info_t *info) {
  return nullptr;
}

void destroy_aabb_store(aabb_store_t *store) {}

} // namespace colliders
} // namespace fastware