#ifndef WINDOW_SYSTEM_H
#define WINDOW_SYSTEM_H

#pragma once

#include <fastware/window_events.h>

namespace fastware {

namespace memory {
struct allocator_t;
}

using event_processor = void (*)(event_t *events, int32_t count,
                                 key_state_t states, void *context);

class window_system {
public:
  window_system(memory::allocator_t *parent_alloc, event_processor processor,
                void *processor_context);
  ~window_system();

  void frame_limiter(toggle state) const;
  void poll();

private:
  memory::allocator_t *d_parent_alloc;
  memory::allocator_t *d_alloc;
  event_processor d_processor;
  void *d_processor_context;
  struct internal_data_t *d_internal;
};

} // namespace fastware

#endif