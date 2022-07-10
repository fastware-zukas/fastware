#ifndef GAME_SETUP_H
#define GAME_SETUP_H

#pragma once

#include <fastware/camera.h>
#include <fastware/data_types.h>
#include <fastware/fastware_def.h>
#include <fastware/window_events.h>
#include <glm/glm.hpp>

namespace fastware {

namespace memory {
struct allocator_t;
}

namespace setup {

constexpr int64_t FRAME{16666667};

struct matrixes {
  glm::mat4 view;
  glm::mat4 projection;
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

void create_transforms(glm::mat4 *matrixes, int32_t count);

void create_speeds(float *speeds, int32_t count);

void update_transforms(glm::mat4 *transforms, float *speeds, int32_t count);

void compute_model_matrixes(glm::mat4 *model_transforms, glm::mat4 *models,
                            glm::mat4 *animations, int32_t count);

void compute_normals_matrixes(glm::mat3 *normal_transforms,
                              glm::mat4 *model_transforms, int32_t count);

void compute_gpu_matrixes(glm::mat4 *model_transforms,
                          glm::mat3 *normal_transforms, glm::mat4 *models,
                          glm::mat4 *animations, int32_t count);

void compute_bounding_model_matrixes(glm::mat4 *model_transforms,
                                     glm::mat4 *models, int32_t count,
                                     glm::mat4 bounding_box);

} // namespace setup

} // namespace fastware

#endif