#include <fastware/renderer.h>

#include <GL/glew.h>

#include <fastware/debug.h>
#include <fastware/utils.h>

#include <fastware/entity.h>

namespace fastware {

namespace renderer {

void begin_frame() {
  constexpr float clear[]{0.025f, 0.025f, 0.025f, 1.f};
  glClearBufferfv(GL_COLOR, 0, clear);
  glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.f, 1);
}

void end_frame() {}

void render_targets(const entity *entities, int32_t count) {
  for (int32_t i = 0; i < count; ++i) {
    const entity &e = entities[i];
    program::select(e.program_id);
    switch (e.render_type) {
    case entity::VERTEX:
      present::render(e.varray_id, e.count);
      break;
    case entity::INDEX:
      present::render_indexed(e.varray_id, e.count);
      break;
    case entity::INDEX_INSTANCED:
      present::render_indexed_instanced(e.varray_id, e.count, e.instance_count);
      break;
    }
  }
}

} // namespace renderer

} // namespace fastware