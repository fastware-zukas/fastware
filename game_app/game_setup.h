#ifndef GAME_SETUP_H
#define GAME_SETUP_H

#pragma once

#include <fastware/camera.h>
#include <fastware/data_types.h>
#include <fastware/fastware_def.h>
#include <fastware/types.h>
#include <fastware/window_events.h>

namespace fastware {

namespace memory {
struct allocator_t;
}

namespace setup {

constexpr int64_t FRAME{1000000000 / 60};

struct matrixes {
  mat4_t view;
  mat4_t projection;
};

struct control_block {
  camera cam;
  uint32_t main_window_id;
  int mode;
  bool show_bounding_box;
};

struct shader_source {
  const char *filename;
  shader_type_e type;
};

uint32_t create_program(memory::allocator_t *allocator, shader_source *shaders,
                        int32_t shader_count);

uint32_t create_texture(memory::allocator_t *allocator, const char *filename);

void process_events(event_t *events, int32_t count, key_state_t states,
                    void *context);

void create_transforms(mat4_t *matrixes, int32_t count);

void create_speeds(float *speeds, int32_t count);

void update_transforms(mat4_t *transforms, float *speeds, int32_t count);

void compute_model_matrixes(mat4_t *model_transforms, mat4_t *models,
                            mat4_t *animations, int32_t count);

void compute_normals_matrixes(mat3_t *normal_transforms,
                              mat4_t *model_transforms, int32_t count);

void compute_gpu_matrixes(mat4_t *model_transforms, mat3_t *normal_transforms,
                          mat4_t *models, mat4_t *animations, int32_t count);

void compute_bounding_model_matrixes(mat4_t *model_transforms, mat4_t *models,
                                     int32_t count, mat4_t bounding_box);

} // namespace setup

} // namespace fastware

#endif