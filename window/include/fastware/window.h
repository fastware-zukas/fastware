#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <cstdint>
#include <fastware/window_events.h>

namespace fastware {

using window = uint32_t;

struct window_create_info {
  const char *title;
  int32_t width;
  int32_t height;
};

struct window_info {
  int32_t width;
  int32_t height;
};

window create_window(const window_create_info *info,
                     window_info *window_info = nullptr);

void destroy_window(window win);

void swap_buffers(window *windows, uint32_t count);

} // namespace fastware

#endif