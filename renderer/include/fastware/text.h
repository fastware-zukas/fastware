#ifndef TEXT_H
#define TEXT_H

#include <fastware/data_types.h>
#include <fastware/fastware_def.h>
#include <fastware/memory.h>

namespace fastware {

namespace text {

void create_atlas(const create_text_atlas_info_t *infos, uint32_t count,
                  text_atlas_t *results);

void create_buffers(const uint32_t *lengths, uint32_t count,
                    text_buffer_t *results);

void update_buffers(memory::allocator_t *allocator,
                    const update_text_buffer_info_t *infos, uint32_t count,
                    uint32_t *index_sizes);

} // namespace text

} // namespace fastware

#endif // TEXT_H
