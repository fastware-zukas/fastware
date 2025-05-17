#ifndef RENDERER_STATE_H
#define RENDERER_STATE_H

#include <fastware/types.h>

namespace fastware {

enum renderer_state {
  BLEND,
  CULL_FACE,
  DEBUG_OUTPUT,
  DEPTH_TEST,
  LINE_SMOOTH,
  MULTISAMPLE,
  POLYGON_OFFSET_LINE,
  POLYGON_OFFSET_POINT,
  POLYGON_SMOOTH,
  RASTERIZER_DISCARD,
  SCISSOR_TEST,
  STENCIL_TEST,
  TEXTURE_CUBE_MAP_SEAMLESS,
  // function changes
  BLEND_FUNC,
  CULL_FACE_FUNC,
  DEPTH_TEST_FUNC,
};

enum renderer_blend_func {
  ZERO,
  ONE,
  SRC_COLOR,
  ONE_MINUS_SRC_COLOR,
  DST_COLOR,
  ONE_MINUS_DST_COLOR,
  SRC_ALPHA,
  ONE_MINUS_SRC_ALPHA,
  DST_ALPHA,
  ONE_MINUS_DST_ALPHA,
  CONSTANT_COLOR,
  ONE_MINUS_CONSTANT_COLOR,
  CONSTANT_ALPHA,
  ONE_MINUS_CONSTANT_ALPHA,
  SRC_ALPHA_SATURATE,
  SRC1_COLOR,
  ONE_MINUS_SRC1_COLOR,
  SRC1_ALPHA,
  ONE_MINUS_SRC1_ALPHA
};

enum renderer_cull_face_func { FRONT, BACK, FRONT_AND_BACK };

enum renderer_depth_func {
  NEVER,
  LESS,
  EQUAL,
  GREATER,
  NOTEQUAL,
  GEQUAL,
  ALWAYS
};

struct renderer_change_state {
  renderer_state state_change;
  toggle_e status;
};

struct renderer_change_blend_func {
  renderer_state state_change;
  renderer_blend_func src_blend_func;
  renderer_blend_func dst_blend_func;
};

struct renderer_change_cull_face_func {
  renderer_state state_change;
  renderer_cull_face_func cull_face_func;
};

struct renderer_change_depth_func {
  renderer_state state_change;
  renderer_depth_func depth_func_func;
};

struct renderer_change {
  union {
    renderer_change_state state;
    renderer_change_blend_func blend_func;
    renderer_change_cull_face_func cull_face_func;
    renderer_change_depth_func depth_func;
  };
};

void execute_renderer_change(const renderer_change *change_array,
                             uint32_t count);

} // namespace fastware

#endif // RENDERER_STATE_H
