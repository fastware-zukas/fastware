#ifndef IMAGE_SOURCE_H
#define IMAGE_SOURCE_H

#include <fastware/fastware_def.h>
#include <fastware/types.h>

namespace fastware {

struct image_data {
  byte *bytes;
  uint32_t width;
  uint32_t height;
  graphics::texture_format_e format;
};

image_data load(const byte *raw_bytes, uint32_t length);

void unload(image_data image);

} // namespace fastware

#endif // IMAGE_SOURCE_H
