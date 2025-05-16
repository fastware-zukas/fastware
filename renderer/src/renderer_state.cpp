#include <fastware/renderer_state.h>

#include <GL/glew.h>

namespace fastware {

GLenum capability(renderer_state state) {
  switch (state) {
  case BLEND:
    return GL_BLEND;
  case CULL_FACE:
    return GL_CULL_FACE;
  case DEBUG_OUTPUT:
    return GL_DEBUG_OUTPUT;
  case DEPTH_TEST:
    return GL_DEPTH_TEST;
  case LINE_SMOOTH:
    return GL_LINE_SMOOTH;
  case MULTISAMPLE:
    return GL_MULTISAMPLE;
  case POLYGON_OFFSET_LINE:
    return GL_POLYGON_OFFSET_LINE;
  case POLYGON_OFFSET_POINT:
    return GL_POLYGON_OFFSET_POINT;
  case POLYGON_SMOOTH:
    return GL_POLYGON_SMOOTH;
  case RASTERIZER_DISCARD:
    return GL_RASTERIZER_DISCARD;
  case SCISSOR_TEST:
    return GL_SCISSOR_TEST;
  case STENCIL_TEST:
    return GL_STENCIL_TEST;
  case TEXTURE_CUBE_MAP_SEAMLESS:
    return GL_TEXTURE_CUBE_MAP_SEAMLESS;
  default:
    return GL_NONE;
  }
}

GLenum blend_func(renderer_blend_func func) {
  switch (func) {
  case ZERO:
    return GL_ZERO;
  case ONE:
    return GL_ONE;
  case SRC_COLOR:
    return GL_SRC_COLOR;
  case ONE_MINUS_SRC_COLOR:
    return GL_ONE_MINUS_SRC_COLOR;
  case DST_COLOR:
    return GL_DST_COLOR;
  case ONE_MINUS_DST_COLOR:
    return GL_ONE_MINUS_DST_COLOR;
  case SRC_ALPHA:
    return GL_SRC_ALPHA;
  case ONE_MINUS_SRC_ALPHA:
    return GL_ONE_MINUS_SRC_ALPHA;
  case DST_ALPHA:
    return GL_DST_ALPHA;
  case ONE_MINUS_DST_ALPHA:
    return GL_ONE_MINUS_DST_ALPHA;
  case CONSTANT_COLOR:
    return GL_CONSTANT_COLOR;
  case ONE_MINUS_CONSTANT_COLOR:
    return GL_ONE_MINUS_CONSTANT_COLOR;
  case CONSTANT_ALPHA:
    return GL_CONSTANT_ALPHA;
  case ONE_MINUS_CONSTANT_ALPHA:
    return GL_ONE_MINUS_CONSTANT_ALPHA;
  case SRC_ALPHA_SATURATE:
    return GL_SRC_ALPHA_SATURATE;
  case SRC1_COLOR:
    return GL_SRC1_COLOR;
  case ONE_MINUS_SRC1_COLOR:
    return GL_ONE_MINUS_SRC1_COLOR;
  case SRC1_ALPHA:
    return GL_SRC1_ALPHA;
  case ONE_MINUS_SRC1_ALPHA:
    return GL_ONE_MINUS_SRC1_ALPHA;
  }

  return GL_ONE_MINUS_SRC_COLOR;
}

GLenum cull_face_func(renderer_cull_face_func func) {
  switch (func) {
  case FRONT:
    return GL_FRONT;
  case BACK:
    return GL_BACK;
  case FRONT_AND_BACK:
    return GL_FRONT_AND_BACK;
  }
  return GL_BACK;
}

GLenum depth_func(renderer_depth_func func) {
  switch (func) {
  case NEVER:
    return GL_NEVER;
  case LESS:
    return GL_LESS;
  case EQUAL:
    return GL_EQUAL;
  case GREATER:
    return GL_GREATER;
  case NOTEQUAL:
    return GL_NOTEQUAL;
  case GEQUAL:
    return GL_GEQUAL;
  case ALWAYS:
    return GL_ALWAYS;
  }
  return GL_NEVER;
}

void glSetCapability(GLenum capability, toggle_e status) {
  switch (status) {
  case toggle_e::OFF: {
    glDisable(capability);
    break;
  }
  case toggle_e::ON: {
    glEnable(capability);
    break;
  }
  }
}

void execute_renderer_change(const renderer_change *change_array,
                             uint32_t count) {
  for (int32_t i = 0, c = static_cast<int32_t>(count); i < c; ++i) {
    const renderer_change &change = change_array[i];
    const GLenum cap = capability(change.state.state_change);
    if (cap != GL_NONE) {
      glSetCapability(cap, change.state.status);
    } else {
      switch (change.state.state_change) {

      case BLEND_FUNC: {
        const GLenum src = blend_func(change.blend_func.src_blend_func);
        const GLenum dst = blend_func(change.blend_func.dst_blend_func);
        glBlendFunc(src, dst);
        break;
      }
      case CULL_FACE_FUNC: {
        const GLenum cf = cull_face_func(change.cull_face_func.cull_face_func);
        glCullFace(cf);
        break;
      }
      case DEPTH_TEST_FUNC: {
        const GLenum dt = depth_func(change.depth_func.depth_func_func);
        glDepthFunc(dt);
        break;
      }
      default:
        break;
      }
    }
  }
}

} // namespace fastware
