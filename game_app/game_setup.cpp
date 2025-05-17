#include "game_setup.h"

#include "geometry.h"

#include <algorithm>
#include <random>

#include <fastware/clock.h>
#include <fastware/file.h>
#include <fastware/image_source.h>
#include <fastware/logger.h>
#include <fastware/maths.h>
#include <fastware/memory.h>
#include <fastware/stopwatch.h>
#include <fastware/types.h>
#include <fastware/utils.h>
#include <fastware/window.h>

namespace fastware {

namespace setup {

uint32_t create_program(memory::allocator_t *allocator, shader_source *shaders,
                        int32_t shader_count) {

  memory::stack_alloc_create_info_t local_alloc_create_info{
      .parent = allocator,
      .size = memory::Mb,
      .alignment = memory::alignment_t::b32};

  memory::allocator_t *local_allocator =
      memory::create(&local_alloc_create_info);

  auto shader_blk =
      memory::allocate(local_allocator, shader_count * sizeof(shader_source_t));
  shader_source_t *shader_srcs = static_cast<shader_source_t *>(shader_blk.ptr);

  for (int32_t i = 0; i < shader_count; i++) {
    File shader_file(shaders[i].filename);
    size_t file_size = shader_file.size();
    memory::memblk blk = memory::allocate(local_allocator, file_size);
    char *src = static_cast<char *>(blk.ptr);
    shader_file.read(src, file_size);
    shader_srcs[i] = shader_source_t{.glsl_source = src,
                                     .length = static_cast<uint32_t>(file_size),
                                     .type = shaders[i].type};
  }

  auto prog_id = program::create(shader_srcs, shader_count);

  memory::destroy(local_allocator);
  return prog_id;
}

uint32_t create_texture(memory::allocator_t *allocator, const char *filename) {

  File file(filename);
  size_t size = file.size();
  memory::memblk blk = memory::allocate(allocator, size);
  byte *src = static_cast<byte *>(blk.ptr);
  file.read(src, size);

  image_data img = load(src, size);
  param_info_t param_infos[2]{
      {parameter_type_e::WRAP_S, wrap_e::CLAMP_TO_EDGE},
      {parameter_type_e::WRAP_T, wrap_e::CLAMP_TO_EDGE}};

  uint8_t *img_data = static_cast<uint8_t *>(img.bytes);
  std::reverse(img_data, img_data + (img.width + img.height));

  subtexture_create_info_t sub_texture{.width = img.width,
                                       .height = img.height,
                                       .format = img.format,
                                       .data = img_data};

  texture_create_info_t texture_info{.width = img.width,
                                     .height = img.height,
                                     .format = img.format,
                                     .param_infos = param_infos,
                                     .sub_texture = &sub_texture,
                                     .param_info_count = 2,
                                     .sub_texture_count = 1

  };

  uint32_t texture_id = 0;

  texture::create_2d(&texture_info, 1, &texture_id);

  unload(img);

  memory::deallocate(allocator, blk);

  return texture_id;
}

void process_events(event_t *events, int32_t count, key_state_t states,
                    void *context) {

  METRIC(ProcessEvents);
  control_block *control = static_cast<control_block *>(context);

  constexpr void *event_handles[]{
      [value(event_type_t::WINDOW_SHOWN)] = &&WINDOW_SHOWN_LBL,
      [value(event_type_t::WINDOW_HIDDEN)] = &&WINDOW_HIDDEN_LBL,
      [value(event_type_t::WINDOW_EXPOSED)] = &&WINDOW_EXPOSED_LBL,
      [value(event_type_t::WINDOW_MOVED)] = &&WINDOW_MOVED_LBL,
      [value(event_type_t::WINDOW_RESIZED)] = &&WINDOW_RESIZED_LBL,
      [value(event_type_t::WINDOW_SIZE_CHANGED)] = &&WINDOW_SIZE_CHANGED_LBL,
      [value(event_type_t::WINDOW_MINIMIZED)] = &&WINDOW_MINIMIZED_LBL,
      [value(event_type_t::WINDOW_MAXIMIZED)] = &&WINDOW_MAXIMIZED_LBL,
      [value(event_type_t::WINDOW_RESTORED)] = &&WINDOW_RESTORED_LBL,
      [value(event_type_t::WINDOW_ENTER)] = &&WINDOW_ENTER_LBL,
      [value(event_type_t::WINDOW_LEAVE)] = &&WINDOW_LEAVE_LBL,
      [value(event_type_t::WINDOW_FOCUS_GAINED)] = &&WINDOW_FOCUS_GAINED_LBL,
      [value(event_type_t::WINDOW_FOCUS_LOST)] = &&WINDOW_FOCUS_LOST_LBL,
      [value(event_type_t::WINDOW_CLOSE)] = &&WINDOW_CLOSE_LBL,
      [value(event_type_t::WINDOW_TAKE_FOCUS)] = &&WINDOW_TAKE_FOCUS_LBL,
      [value(event_type_t::WINDOW_HIT_TEST)] = &&WINDOW_HIT_TEST_LBL,
      [value(event_type_t::WINDOW_KEY)] = &&WINDOW_KEY_LBL,
      [value(event_type_t::WINDOW_MOUSE_MOVE)] = &&WINDOW_MOUSE_MOVE_LBL,
      [value(event_type_t::WINDOW_MOUSE_WHEEL)] = &&WINDOW_MOUSE_WHEEL_LBL,
      [value(event_type_t::WINDOW_MOUSE_BUTTON)] = &&WINDOW_MOUSE_BUTTON_LBL,
      [value(event_type_t::WINDOW_TEXT_EDIT)] = &&WINDOW_TEXT_EDIT_LBL,
      [value(event_type_t::END_STREAM)] = &&END_STREAM_LBL,
  };

  const event_t *eve = nullptr;
  const auto next = [&]() {
    eve = events;
    auto type = eve->type;
    events++;
    return event_handles[value(type)];
  };
  goto *next();

WINDOW_SHOWN_LBL:
  goto *next();

WINDOW_HIDDEN_LBL:
  goto *next();

WINDOW_EXPOSED_LBL:
  goto *next();

WINDOW_MOVED_LBL:
  goto *next();

WINDOW_RESIZED_LBL:
  goto *next();

WINDOW_SIZE_CHANGED_LBL:
  goto *next();

WINDOW_MINIMIZED_LBL:
  goto *next();

WINDOW_MAXIMIZED_LBL:
  goto *next();

WINDOW_RESTORED_LBL:
  goto *next();

WINDOW_ENTER_LBL:
  goto *next();

WINDOW_LEAVE_LBL:
  goto *next();

WINDOW_FOCUS_GAINED_LBL:
  goto *next();

WINDOW_FOCUS_LOST_LBL:
  goto *next();

WINDOW_CLOSE_LBL:
  logger::log("Window closing %u", eve->window_id);
  if (control->main_window_id == eve->window_id) {
    destroy_window(control->main_window_id);
    control->main_window_id = 0;
  }

  goto *next();

WINDOW_TAKE_FOCUS_LBL:
  goto *next();

WINDOW_HIT_TEST_LBL:
  goto *next();

WINDOW_KEY_LBL:
  if (eve->key.action == input::action_e::RELEASE) {
    switch (eve->key.keycode) {
    case input::key_e::KEY_KP_PLUS: {
      clock::set_game_speed(clock::game_speed() + 1);
      break;
    }
    case input::key_e::KEY_KP_MINUS: {
      clock::set_game_speed(clock::game_speed() - 1);
      break;
    }
    case input::key_e::KEY_0: {
      control->mode = 0;
      break;
    }
    case input::key_e::KEY_1: {
      control->mode = 1;
      break;
    }
    case input::key_e::KEY_2: {
      control->mode = 2;
      break;
    }
    case input::key_e::KEY_3: {
      control->mode = 3;
      break;
    }
    case input::key_e::KEY_B: {
      control->show_bounding_box = !control->show_bounding_box;
      break;
    }
    case input::key_e::KEY_ESCAPE: {
      destroy_window(control->main_window_id);
      control->main_window_id = 0;
      break;
    }
    default:
      break;
    }
  }
  goto *next();

WINDOW_MOUSE_MOVE_LBL:
  goto *next();

WINDOW_MOUSE_WHEEL_LBL:
  goto *next();

WINDOW_MOUSE_BUTTON_LBL:
  goto *next();

WINDOW_TEXT_EDIT_LBL:
  goto *next();

END_STREAM_LBL:

  float duration = float(clock::system_time_delta()) / float(FRAME);
  logger::log("Frame duration ratio: %f, Expected time: %ld, Frame time: %ld",
              duration, FRAME, clock::system_time_delta());

  if (states.current_key_states[value(input::key_e::KEY_Q)]) {
    control->cam = pan_horizontal(control->cam, glm_rad(1.f) * duration);
  }

  if (states.current_key_states[value(input::key_e::KEY_E)]) {
    control->cam = pan_horizontal(control->cam, glm_rad(-1.f) * duration);
  }

  if (states.current_key_states[value(input::key_e::KEY_W)]) {
    control->cam = move_parallel(control->cam, 6.f * duration);
  }

  if (states.current_key_states[value(input::key_e::KEY_S)]) {
    control->cam = move_parallel(control->cam, -6.f * duration);
  }

  if (states.current_key_states[value(input::key_e::KEY_A)]) {
    control->cam = move_perpendicular(control->cam, -6.f * duration);
  }

  if (states.current_key_states[value(input::key_e::KEY_D)]) {
    control->cam = move_perpendicular(control->cam, 6.f * duration);
  }

  if (states.current_key_states[value(input::key_e::KEY_PAGEUP)]) {
    control->cam = move_vertically(control->cam, 4.f * duration);
  }

  if (states.current_key_states[value(input::key_e::KEY_PAGEDOWN)]) {
    control->cam = move_vertically(control->cam, -4.f * duration);
  }

  logger::log("Events processed %d", count);

  return;
}

void create_transforms(mat4_t *matrixes, int32_t count) {

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<float> dis(-4000.f, 4000.f);
  std::uniform_real_distribution<float> dis2(3.f, 15.f);

  for (int32_t i = 0; i < count; ++i) {

    float r = dis2(gen);
    float x = dis(gen);
    float y = dis(gen);
    float z = dis(gen);
    mat4_t scale = glms_scale(glms_mat4_identity(), vec3_t{r, r, r});
    mat4_t rot =
        glms_rotate(glms_mat4_identity(), glm_rad(-90.f), vec3_t{1, 0, 0});
    mat4_t move = glms_translate(glms_mat4_identity(), vec3_t{x, y, z});
    matrixes[i] = glms_mul(move, glms_mul(rot, scale));
  }
}

void create_speeds(float *speeds, int32_t count) {

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<float> dis(-2.f, 2.f);

  for (int32_t i = 0; i < count; ++i) {
    speeds[i] = float(dis(gen));
  }
}

void update_transforms(mat4_t *transforms, float *speeds, int32_t count) {
  constexpr float pi_scale = PI / 10000000000.f;
  const float time = clock::game_time_delta() * pi_scale;
  for (int32_t i = 0; i < count; ++i) {
    transforms[i] =
        glms_rotate(transforms[i], time * speeds[i], vec3_t{0, 0, 1});
  }
}

void compute_model_matrixes(mat4_t *model_transforms, mat4_t *models,
                            mat4_t *animations, int32_t count) {
  for (int32_t i = 0; i < count; ++i) {
    model_transforms[i] = glms_mul(models[i], animations[i]);
  }
}

void compute_normals_matrixes(mat3_t *normal_transforms,
                              mat4_t *model_transforms, int32_t count) {
  for (int32_t i = 0; i < count; ++i) {
    normal_transforms[i] = glms_mat4_pick3(
        glms_mat4_transpose(glms_mat4_inv(model_transforms[i])));
  }
}

void compute_gpu_matrixes(mat4_t *model_transforms, mat3_t *normal_transforms,
                          mat4_t *models, mat4_t *animations, int32_t count) {
  for (int32_t i = 0; i < count; ++i) {
    model_transforms[i] = glms_mul(models[i], animations[i]);
    normal_transforms[i] = glms_mat4_pick3(
        glms_mat4_transpose(glms_mat4_inv(model_transforms[i])));
  }
}

void compute_bounding_model_matrixes(mat4_t *model_transforms, mat4_t *models,
                                     int32_t count, mat4_t bounding_box) {

  for (int32_t i = 0; i < count; ++i) {
    model_transforms[i] = glms_mul(models[i], bounding_box);
  }
}

} // namespace setup

} // namespace fastware
