#ifndef WINDOW_SYSTEM_H
#define WINDOW_SYSTEM_H

#pragma once

namespace fastware {

namespace memory {
struct allocator_t;
}

class window_system {
public:
  window_system(memory::allocator_t *parent_alloc);
  ~window_system();

  void poll();

  bool closing() const;

private:
  memory::allocator_t *d_parent_alloc;
  memory::allocator_t *d_alloc;

  bool d_closing;
};

} // namespace fastware

#endif