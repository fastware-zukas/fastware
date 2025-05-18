#include <fastware/text.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftbitmap.h>

#include <cassert>

#include <fastware/maths.h>
#include <fastware/utils.h>

namespace fastware::text {

void create_atlas(const create_text_atlas_info_t *infos, uint32_t count,
                  text_atlas_t *results) {

  FT_Library ft;

  if (FT_Init_FreeType(&ft)) {
    // error
    return;
  }

  for (uint32_t i = 0; i < count; i++) {

    const create_text_atlas_info_t &info = infos[i];

    FT_Face face;
    if (FT_Error error = FT_New_Face(ft, info.font_file, 0, &face); error) {
      // error
      printf("font error: %s | %s\n", info.font_file, FT_Error_String(error));
      exit(1);
      continue;
    }

    FT_Set_Pixel_Sizes(face, 0, 128);

    text_atlas_t &atlas = results[i];

    uint32_t max_width = 0;
    uint32_t max_height = 0;

    subtexture_create_info_t sub_infos[128];

    FT_Bitmap bitmaps[128];

    for (uint32_t c = 0; c < 128; c++) {
      if (FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER); error) {
        // error
        printf("char error: %c | %s\n", c, FT_Error_String(error));
        exit(1);
        continue;
      }

      subtexture_create_info_t &sub = sub_infos[c];

      FT_Bitmap_Init(&bitmaps[c]);
      FT_Bitmap_Copy(ft, &face->glyph->bitmap, &bitmaps[c]);

      sub.width = bitmaps[c].width;
      sub.height = bitmaps[c].rows;
      sub.data = bitmaps[c].buffer;
      sub.format = graphics::texture_format_e::R8;

      max_width += bitmaps[c].width;
      max_height = fast_max(max_height, face->glyph->bitmap.rows);
    }

    param_info_t param_infos[]{
        param_info_t{.type = parameter_type_e::WRAP_S,
                     .wrap = wrap_e::CLAMP_TO_EDGE},
        param_info_t{.type = parameter_type_e::WRAP_T,
                     .wrap = wrap_e::CLAMP_TO_EDGE},
        param_info_t{.type = parameter_type_e::MIN_FILTER,
                     .min_filter = min_filter_e::LINEAR_MIPMAP_LINEAR},
        param_info_t{.type = parameter_type_e::MAG_FILTER,
                     .mag_filter = mag_filter_e::LINEAR}};

    texture_create_info_t text_info{.width = max_width,
                                    .height = max_height,
                                    .format = graphics::texture_format_e::R8,
                                    .pixel_unpack_align =
                                        graphics::pixel_align_e::B1,
                                    .param_infos = param_infos,
                                    .sub_texture = sub_infos,
                                    .param_info_count = 4,
                                    .sub_texture_count = 128};

    texture::create_2d(&text_info, 1, &atlas.texture_id);

    for (uint32_t c = 0, offset = 0; c < 128; c++) {
      if (FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER); error) {
        // error
        printf("char error: %c, %u | %s\n", c, offset, FT_Error_String(error));
        exit(1);
        continue;
      }

      FT_Bitmap_Done(ft, &bitmaps[c]);

      character_t &ch = atlas.chars[c];

      ch.size = vec2i_t{cast<int32_t>(face->glyph->bitmap.width),
                        cast<int32_t>(face->glyph->bitmap.rows)};

      ch.bearing = vec2i_t{cast<int32_t>(face->glyph->bitmap_left),
                           cast<int32_t>(face->glyph->bitmap_top)};

      ch.top_left =
          vec2_t{cast<float_t>(offset) / cast<float_t>(max_width), 0.f};

      ch.bottom_right =
          vec2_t{cast<float_t>(offset + ch.size.x) / cast<float_t>(max_width),
                 cast<float_t>(ch.size.y) / cast<float_t>(max_height)};

      ch.advance = face->glyph->advance.x;

      offset += face->glyph->bitmap.width;
    }

    atlas.width = max_width;
    atlas.height = max_height;

    FT_Done_Face(face);
  }

  FT_Done_FreeType(ft);
}

void create_buffers(const uint32_t *lengths, uint32_t count,
                    text_buffer_t *results) {
  for (uint32_t i = 0; i < count; i++) {

    text_buffer_t &txt_buffer = results[i];

    const uint32_t length = lengths[i];
    const uint32_t section_size = size_of<uint32_t, vec2_t>() * 4 * length;

    printf("Length: %u, Array(L): %u, Index(L): %u\n", length,
           size_of<uint32_t, vec2_t>() * 12 * length,
           size_of<uint32_t, uint32_t>() * 6 * length);

    const buffer_create_info_t buffer_infos[]{
        {.target = buffer_target_e::ARRAY_BUFFER,
         .type = buffer_type_e::DYNAMIC,
         .size = section_size * 3,
         .data = nullptr},
        {.target = buffer_target_e::ELEMENT_ARRAY_BUFFER,
         .type = buffer_type_e::DYNAMIC,
         .size = size_of<uint32_t, uint32_t>() * 6 * length,
         .data = nullptr}};

    uint32_t buffers[2]{0};
    buffer::create(buffer_infos, 2, buffers);
    txt_buffer.array_buffer = buffers[0];
    txt_buffer.index_buffer = buffers[1];
    txt_buffer.section_size = section_size;

    printf("created text buffers, array_buffer: %u, index_buffer: %u\n",
           txt_buffer.array_buffer, txt_buffer.index_buffer);

    constexpr vertex_buffer_section_definition_t vbuffer_section_info[3]{
        {.type = data_type_e::FLOAT_VECTOR2, .normalize = normalize_e::NO},
        {.type = data_type_e::FLOAT_VECTOR2, .normalize = normalize_e::NO},
        {.type = data_type_e::FLOAT_VECTOR2, .normalize = normalize_e::NO}};

    const vertex_buffer_definition_t vbuffer_info{
        .buffer_id = txt_buffer.array_buffer,
        .data_rate = data_rate_e::PER_VERTEX,
        .data_element_count = length * 4,
        .section_definition_count = 3,
        .section_definitions = vbuffer_section_info};

    const vertex_array_definition_t varray_info{
        .buffer_definitions = &vbuffer_info,
        .buffer_definition_count = 1,
        .index_buffer_id = txt_buffer.index_buffer};

    varray::create(&varray_info, 1, &txt_buffer.varray_id);

    printf("created text varray: %u\n", txt_buffer.varray_id);
    // exit(1);
  }
}

void update_buffers(memory::allocator_t *allocator,
                    const update_text_buffer_info_t *infos, uint32_t count,
                    uint32_t *index_sizes) {

  memory::stack_alloc_create_info_t alloc_info{.parent = allocator,
                                               .size = memory::Mb * 2,
                                               .alignment =
                                                   memory::alignment_t::b64};

  memory::allocator_t *alloc = memory::create(&alloc_info);

  constexpr auto rotate = [](vec2_t origin, vec2_t location, float rotation) {
    vec2_t trans = glms_vec2_sub(location, origin);
    float_t rad = glm_rad(rotation);
    float_t tcos = cosf(rad);
    float_t tsin = sinf(rad);
    vec2_t rot = {trans.x * tcos - trans.y * tsin,
                  trans.x * tsin + trans.y * tcos};
    return glms_vec2_add(rot, origin);
  };

  for (uint32_t i = 0; i < count; i++) {
    const update_text_buffer_info_t &info = infos[i];

    const uint32_t section_size = sizeof(vec2_t) * 4 * (info.length);
    const uint32_t index_size = sizeof(uint32_t) * 6 * (info.length);

    assert(section_size <= info.buffer->section_size);

    index_sizes[i] = 6 * info.length;

    printf("Text: %s, Length: %u, Section(L): %u, Index(L): %u\n", info.text,
           info.length, section_size, index_size);

    memory::memblk blk_vert = memory::allocate(alloc, section_size);
    memory::memblk blk_orig = memory::allocate(alloc, section_size);
    memory::memblk blk_glyph = memory::allocate(alloc, section_size);
    memory::memblk blk_elem = memory::allocate(alloc, index_size);

    vec2_t *vert_data = cast<vec2_t *>(blk_vert.ptr);
    vec2_t *orig_data = cast<vec2_t *>(blk_orig.ptr);
    vec2_t *glypth_data = cast<vec2_t *>(blk_glyph.ptr);
    uint32_t *idx_data = cast<uint32_t *>(blk_elem.ptr);

    float_t pos_x = info.pos.x;
    const float_t pos_y = info.pos.y;
    const float_t scale = info.size;

    auto len1 = memory::address{orig_data} - memory::address{vert_data};
    auto len2 = memory::address{glypth_data} - memory::address{orig_data};

    printf("buffer length (1): %lu, (2): %lu\n", len1, len2);

    for (uint32_t n = 0; n < info.length; n++) {

      const int32_t n1 = n * 4 + 0;
      const int32_t n2 = n * 4 + 1;
      const int32_t n3 = n * 4 + 2;
      const int32_t n4 = n * 4 + 3;

      char c = info.text[n];
      const character_t &ch = info.atlas->chars[c];

      float_t x = pos_x + ch.bearing.x * scale;
      float_t y = pos_y - (ch.size.y - ch.bearing.y) * scale;
      float_t w = ch.size.x * scale;
      float_t h = ch.size.y * scale;
      float_t margin = 0.00002f;

      printf("current char: %c, at %u\n", c, n);

      // vert_data[n1] = rotate(info.pos, {x, y + h}, info.rotation);
      // vert_data[n2] = rotate(info.pos, {x, y}, info.rotation);
      // vert_data[n3] = rotate(info.pos, {x + w, y}, info.rotation);
      // vert_data[n4] = rotate(info.pos, {x + w, y + h}, info.rotation);

      vert_data[n1] = {x, y + h};
      vert_data[n2] = {x, y};
      vert_data[n3] = {x + w, y};
      vert_data[n4] = {x + w, y + h};

      orig_data[n1] = {float(c * 2) / 256.f, float(c)};
      orig_data[n2] = {float(c * 2) / 256.f, float(c)};
      orig_data[n3] = {float(c * 2) / 256.f, float(c)};
      orig_data[n4] = {float(c * 2) / 256.f, float(c)};

      glypth_data[n1] = {ch.top_left.x + margin, ch.top_left.y};
      glypth_data[n2] = {ch.top_left.x + margin, ch.bottom_right.y};
      glypth_data[n3] = {ch.bottom_right.x - margin, ch.bottom_right.y};
      glypth_data[n4] = {ch.bottom_right.x - margin, ch.top_left.y};

      // glypth_data[n1] = {0.f, 0.f};
      // glypth_data[n2] = {0.f, 1.f};
      // glypth_data[n3] = {1.f, 1.f};
      // glypth_data[n4] = {1.f, 0.f};

      idx_data[n * 6 + 0] = n1;
      idx_data[n * 6 + 1] = n2;
      idx_data[n * 6 + 2] = n3;
      idx_data[n * 6 + 3] = n3;
      idx_data[n * 6 + 4] = n4;
      idx_data[n * 6 + 5] = n1;

      pos_x += (ch.advance >> 6) * scale;
    }

    buffer_update_info_t update_infos[4]{
        buffer_update_info_t{.buffer_id = info.buffer->array_buffer,
                             .offset = 0,
                             .size = section_size,
                             .data = vert_data},
        buffer_update_info_t{.buffer_id = info.buffer->array_buffer,
                             .offset = info.buffer->section_size,
                             .size = section_size,
                             .data = orig_data},
        buffer_update_info_t{.buffer_id = info.buffer->array_buffer,
                             .offset = info.buffer->section_size * 2,
                             .size = section_size,
                             .data = glypth_data},
        buffer_update_info_t{.buffer_id = info.buffer->index_buffer,
                             .offset = 0,
                             .size = index_size,
                             .data = idx_data}};

    buffer::update(update_infos, 4);
    memory::deallocate_all(alloc);
  }

  memory::destroy(alloc);
  // https://github.com/Samson-Mano/opengl_textrendering/blob/master/opengl_textrendering/src/label_text_store.cpp
}

} // namespace fastware::text