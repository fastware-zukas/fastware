#ifndef RENDERER_H
#define RENDERER_H

#pragma once

#include <cstdint>

namespace fastware {

struct entity;

namespace renderer {

void begin_frame();
void end_frame();

void render_targets(const entity *entities, int32_t count);

} // namespace renderer
} // namespace fastware

#endif