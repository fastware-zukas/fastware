#include <fastware/image_source.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace fastware {

image_data load(const byte *raw_bytes, uint32_t length) {

  int32_t width = 0;
  int32_t height = 0;
  int32_t components = 0;
  byte *data =
      stbi_load_from_memory(raw_bytes, length, &width, &height, &components, 0);

  graphics::texture_format format;
  switch (components) {
  case 1:
    format = graphics::texture_format::R8;
    break;
  case 2:
    format = graphics::texture_format::RG8;
    break;
  case 3:
    format = graphics::texture_format::RGB8;
    break;
  default:
    format = graphics::texture_format::RGBA8;
    break;
  }

  return {data, static_cast<uint32_t>(width), static_cast<uint32_t>(height),
          format};
}

void unload(image_data image) { stbi_image_free(image.bytes); }

} // namespace fastware
