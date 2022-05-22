#include "window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL.h>
#include <cassert>
#include <cstring>

#include <fastware/logger.h>
#include <fastware/memory.h>

namespace fastware {

window::window(const window_create_info *info) {

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_Window *window = SDL_CreateWindow(
      info->title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      info->width, info->height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  d_window_handle = SDL_GetWindowID(window);
  SDL_GLContext context = SDL_GL_CreateContext(window);
  glewExperimental = GL_TRUE;

  GLenum status = glewInit();
  if (GLEW_OK != status) {
    logger::log("Failed to initiase GLEW: %s", glewGetErrorString(status));
  }
}

window::~window() {

  auto window = SDL_GetWindowFromID(d_window_handle);

  SDL_DestroyWindow(window);
}

void window::swap_buffers() {

  auto window = SDL_GetWindowFromID(d_window_handle);
  SDL_GL_SwapWindow(window);
}

} // namespace fastware