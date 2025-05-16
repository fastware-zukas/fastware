#include <fastware/text.h>

#include <ft2build.h>
#include FT_FREETYPE_H

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
    if (FT_New_Face(ft, info.font_file, 0, &face)) {
      // error
      continue;
    }

    FT_Set_Pixel_Sizes(face, 0, 128);

    text_atlas_t &atlas = results[i];

    uint32_t max_width = 0;
    uint32_t max_height = 0;

    memory::stack_alloc_create_info_t meminfo{.parent = info.alloc,
                                              .size = memory::Mb,
                                              .alignment =
                                                  memory::alignment_t::b16};
    memory::allocator_t *local = memory::create(&meminfo);

    memory::memblk blk = memory::allocate(local, memory::Mb);

    byte *buffer = static_cast<byte *>(blk.ptr);

    for (uint32_t c = 0; c < 128; c++) {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        // error
        continue;
      }
      memccpy(buffer + max_width, face->glyph->bitmap.buffer, 0,
              face->glyph->bitmap.width);
      max_width += face->glyph->bitmap.width;
      max_height = fast_max(max_height, face->glyph->bitmap.rows);
    }

    for (uint32_t c = 0, offset = 0; c < 128; c++) {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        // error
        continue;
      }

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
                                    .param_info_count = 4,
                                    .data = buffer,
                                    .param_infos = param_infos};

    texture::create_2d(&text_info, 1, &atlas.texture_id);
    atlas.width = max_width;
    atlas.height = max_height;

    memory::destroy(local);

    FT_Done_Face(face);
  }

  FT_Done_FreeType(ft);
}

void create_buffers(const uint32_t *lengths, uint32_t count,
                    text_buffer_t *results) {
  for (uint32_t i = 0; i < count; i++) {

    text_buffer_t &txt_buffer = results[i];

    const uint32_t length = lengths[i];

    const buffer_create_info_t buffer_infos[]{
        {.target = buffer_target_e::ARRAY_BUFFER,
         .type = buffer_type_e::DYNAMIC,
         .size = size_of<uint32_t, vec2_t>() * 12 * length,
         .data = nullptr},
        {.target = buffer_target_e::ELEMENT_ARRAY_BUFFER,
         .type = buffer_type_e::DYNAMIC,
         .size = size_of<uint32_t, uint32_t>() * 6 * length,
         .data = nullptr}};

    uint32_t buffers[2]{0};
    buffer::create(buffer_infos, 2, buffers);
    txt_buffer.array_buffer = buffers[0];
    txt_buffer.index_buffer = buffers[1];

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
  }
}

void update_buffers(memory::allocator_t *allocator,
                    const update_text_buffer_info_t *infos, uint32_t count) {

  memory::stack_alloc_create_info_t alloc_info{.parent = allocator,
                                               .size = memory::Mb * 1,
                                               .alignment =
                                                   memory::alignment_t::b16};

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

    const int32_t section_len = sizeof(vec2_t) * 4 * info.length;

    memory::memblk blk_vert = memory::allocate(alloc, section_len * 3);
    memory::memblk blk_elem =
        memory::allocate(alloc, sizeof(uint32_t) * 6 * info.length);

    vec2_t *vert_data = cast<vec2_t *>(blk_vert.ptr);
    vec2_t *orig_data =
        cast<vec2_t *>((memory::address{blk_vert.ptr} + section_len).raw);
    vec2_t *glypth_data =
        cast<vec2_t *>((memory::address{blk_vert.ptr} + section_len * 2).raw);
    uint32_t *idx_data = cast<uint32_t *>(blk_elem.ptr);

    float_t pos_x = info.pos.x;
    const float_t pos_y = info.pos.y;
    const float_t scale = info.size;

    for (uint32_t n = 0; n < info.length; n++) {

      const int32_t n1 = n * 4 + 0;
      const int32_t n2 = n * 4 + 1;
      const int32_t n3 = n * 4 + 2;
      const int32_t n4 = n * 4 + 3;

      char c = info.text[n];
      const character_t &ch = info.atlas->chars[c];

      float_t x = ch.bearing.x * scale + pos_x;
      float_t y = pos_y - (ch.size.y - ch.bearing.y) * scale;
      float_t w = ch.size.x * scale;
      float_t h = ch.size.y * scale;
      float_t margin = 0.00005f;

      vert_data[n1] = rotate(info.pos, {x, y + h}, info.rotation);
      vert_data[n2] = rotate(info.pos, {x, y}, info.rotation);
      vert_data[n3] = rotate(info.pos, {x + w, y}, info.rotation);
      vert_data[n4] = rotate(info.pos, {x + w, y + h}, info.rotation);

      orig_data[n1] = info.pos;
      orig_data[n2] = info.pos;
      orig_data[n3] = info.pos;
      orig_data[n4] = info.pos;

      glypth_data[n1] = {ch.top_left.x + margin, ch.top_left.y};
      glypth_data[n2] = {ch.top_left.x + margin, ch.bottom_right.y};
      glypth_data[n3] = {ch.bottom_right.x - margin, ch.bottom_right.y};
      glypth_data[n4] = {ch.bottom_right.x - margin, ch.top_left.y};

      idx_data[n * 6 + 0] = n1;
      idx_data[n * 6 + 1] = n2;
      idx_data[n * 6 + 2] = n3;
      idx_data[n * 6 + 3] = n3;
      idx_data[n * 6 + 4] = n4;
      idx_data[n * 6 + 5] = n1;

      pos_x += (ch.advance >> 6) * scale;
    }

    buffer_update_info_t update_infos[2]{
        buffer_update_info_t{.buffer_id = info.array_buffer_id,
                             .offset = 0,
                             .size = cast<uint32_t>(blk_vert.size),
                             .data = vert_data},
        buffer_update_info_t{.buffer_id = info.element_buffer_id,
                             .offset = 0,
                             .size = cast<uint32_t>(blk_elem.size),
                             .data = idx_data}};

    buffer::update(update_infos, 2);
    memory::deallocate_all(alloc);
  }

  memory::destroy(alloc);
  // https://github.com/Samson-Mano/opengl_textrendering/blob/master/opengl_textrendering/src/label_text_store.cpp
}

} // namespace fastware::text