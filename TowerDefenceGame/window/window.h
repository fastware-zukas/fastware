#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <cstdint>

namespace fastware {

struct window_create_info {
  const char *title;
  int width;
  int height;
};

class window {
public:
  window(const window_create_info *info);
  ~window();

  void swap_buffers();

private:
  uint32_t d_window_handle;
};

} // namespace fastware

#endif