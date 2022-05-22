#include "window_system.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL.h>
#include <cassert>
#include <cstring>

#include <fastware/logger.h>
#include <fastware/memory.h>

namespace fastware {

window_system::window_system(memory::allocator_t *parent_alloc)
    : d_parent_alloc(parent_alloc), d_closing(false) {

  memory::stack_alloc_create_info_t create_info{parent_alloc, 8 * 1024 * 1024,
                                                memory::alignment_t::b64};

  d_alloc = memory::create(&create_info);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    logger::log("SDL could not initialize! SDL Error: %s", SDL_GetError());
    return;
  }
}

window_system::~window_system() {

  memory::destroy(d_alloc);
  SDL_Quit();
}

void window_system::poll() {

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      logger::log("System quit");
      d_closing = true;
    } else if (e.type == SDL_WINDOWEVENT) {
      switch (e.window.event) {
      case SDL_WINDOWEVENT_CLOSE: {
        logger::log("Window close");
        d_closing = true;
      }
      }
    }
  }
}

bool window_system::closing() const { return d_closing; }

} // namespace fastware