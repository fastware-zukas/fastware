#ifndef IMAGE_SOURCE_H
#define IMAGE_SOURCE_H

#include <fastware/types.h>

namespace fastware {

struct image_data {
  void *bytes;
  uint32_t width;
  uint32_t height;
  graphics::texture_format format;
};

image_data load(const char *filename);

void unload(image_data image);

} // namespace fastware

#endif // IMAGE_SOURCE_H
