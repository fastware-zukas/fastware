#include <fastware/window.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL3/SDL.h>
#include <cassert>
#include <cstring>

#include <fastware/logger.h>
#include <fastware/memory.h>

namespace fastware {

window create_window(const window_create_info *info, window_info *window_info) {

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  int32_t window_width = info->width;
  int32_t window_height = info->height;

  if (window_width == 0 || window_height == 0) {

    SDL_DisplayID primary_display = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode *display = SDL_GetCurrentDisplayMode(primary_display);
    if (display == nullptr) {
      logger::log("Failed to get display mode: %s", SDL_GetError());
      logger::flush();
    }
    window_width = display->w;
    window_height = display->h;
  }

  SDL_Window *sdl_window = SDL_CreateWindow(
      info->title, window_width, window_height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN);

  window win = SDL_GetWindowID(sdl_window);
  SDL_GLContext context = SDL_GL_CreateContext(sdl_window);
  glewExperimental = GL_TRUE;

  GLenum status = glewInit();
  if (GLEW_OK != status) {
    logger::log("Failed to initiase GLEW: %s", glewGetErrorString(status));
    logger::flush();
  }

  if (window_info) {
    window_info->width = window_width;
    window_info->height = window_height;
  }

  return win;
}

void destroy_window(window win) {
  auto window = SDL_GetWindowFromID(win);
  SDL_DestroyWindow(window);
}

void swap_buffers(window *windows, uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    auto window = SDL_GetWindowFromID(windows[i]);
    SDL_GL_SwapWindow(window);
  }
}

} // namespace fastware