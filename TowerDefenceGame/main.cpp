#include <fastware/window.h>
#include <fastware/window_system.h>

#include <fastware/clock.h>
#include <fastware/file.h>
#include <fastware/logger.h>
#include <fastware/memory.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cstdint>
#include <random>

#include <fastware/camera.h>
#include <fastware/data_types.h>
#include <fastware/debug.h>
#include <fastware/entity.h>
#include <fastware/image_source.h>
#include <fastware/renderer.h>
#include <fastware/renderer_state.h>
#include <fastware/utils.h>

constexpr float PI{3.14159265358979323846f};

constexpr float FRAME{1e+9 / 60};

namespace sphere {

struct model_data_pos_normal_uv {
  glm::vec3 *positions;
  glm::vec3 *normals;
  glm::vec2 *uvs;
  int32_t count;
};

struct model_indexes {
  uint32_t *indexes;
  int32_t count;
};

constexpr uint32_t vertex_count(uint32_t units) {
  return (units + 1) * (units + 1);
}
constexpr uint32_t index_count(uint32_t units) {
  return (units) * (units - 1) * 6;
}

void generate(model_data_pos_normal_uv &vertex_data, model_indexes &idx_data,
              uint32_t units) {
  int64_t steps = units;

  float x, y, z, xy; // vertex position
  float nx, ny, nz;  // vertex normal
  float s, t;        // vertex texCoord

  const float sectorStep = 2.f * PI / float(steps);
  const float stackStep = PI / float(steps);
  float sectorAngle, stackAngle;

  int64_t index = 0;

  for (int64_t i = 0; i <= steps; ++i) {
    stackAngle = PI / 2.f - i * stackStep; // starting from pi/2 to -pi/2
    xy = cosf(stackAngle);                 // r * cos(u)
    z = sinf(stackAngle);                  // r * sin(u)

    for (int64_t j = 0; j <= units; ++j) {
      sectorAngle = j * sectorStep;

      x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
      y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)

      nx = x;
      ny = y;
      nz = z;

      s = float(j) / float(steps);
      t = float(i) / float(steps);

      vertex_data.positions[index] = glm::vec3{x, y, z};
      vertex_data.normals[index] = glm::vec3{nx, ny, nz};
      vertex_data.uvs[index] = glm::vec2{s, t};
      ++index;
    }
  }

  uint32_t k1 = 0;
  uint32_t k2 = 0;

  index = 0;

  for (int64_t i = 0; i < steps; ++i) {
    k1 = static_cast<uint32_t>(i * (steps + 1)); // beginning of current stack
    k2 = static_cast<uint32_t>(k1 + steps + 1);  // beginning of next stack

    for (int64_t j = 0; j < steps; ++j, ++k1, ++k2) {
      // 2 triangles per sector excluding first and last stacks
      // k1 => k2 => k1+1
      if (i != 0) {
        idx_data.indexes[index++] = k1;
        idx_data.indexes[index++] = k2;
        idx_data.indexes[index++] = k1 + 1;
      }

      // k1+1 => k2 => k2+1
      if (i != (steps - 1)) {
        idx_data.indexes[index++] = k1 + 1;
        idx_data.indexes[index++] = k2;
        idx_data.indexes[index++] = k2 + 1;
      }
    }
  }
}

void init_model_mat(glm::mat4 *matrixes, int32_t count) {
  for (int32_t i = 0; i < count; ++i) {
    matrixes[i] = glm::mat4(1.f);
  }
}

void create_instancing_transforms(glm::mat4 *matrixes, float *transform_speeds,
                                  int32_t count) {

  std::random_device
      rd; // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<float> dis(-4000.f, 4000.f);
  std::uniform_real_distribution<float> dis2(-2.f, 2.f);
  std::uniform_real_distribution<float> dis3(3.f, 15.f);

  for (int32_t i = 0; i < count; ++i) {

    float r = dis3(gen);
    float x = dis(gen);
    float y = dis(gen);
    float z = dis(gen);
    glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(r, r, r));
    glm::mat4 rot =
        glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1, 0, 0));
    glm::mat4 move = glm::translate(glm::mat4(1.f), glm::vec3(x, y, z));
    matrixes[i] = move * rot * scale;

    transform_speeds[i] = float(dis2(gen));
  }
}

void update_instancing_transforms(glm::mat4 *transforms,
                                  float *transform_speeds, int32_t count) {

  constexpr float pi_scale = PI / 10000000000.f;
  const float time = fastware::clock::game_time_delta() * pi_scale;
  for (int32_t i = 0; i < count; ++i) {
    transforms[i] = glm::rotate(transforms[i], time * transform_speeds[i],
                                glm::vec3(0, 0, 1));
  }
}
} // namespace sphere

struct SystemAlloc {
  SystemAlloc() {
    fastware::memory::stack_alloc_create_info_t alloc_create_info{
        nullptr, 500 * 1024 * 1024, fastware::memory::alignment_t::b64};

    root_alloc = fastware::memory::create(&alloc_create_info);
  }

  ~SystemAlloc() { fastware::memory::destroy(root_alloc); }

  fastware::memory::allocator_t *root_alloc;
};

struct Matrixes {
  glm::mat4 view;
  glm::mat4 projection;
};

struct control_block {
  fastware::camera cam;
  uint32_t main_window_id;
  int mode;
};

void process_events(fastware::event_t *events, int32_t count,
                    fastware::key_state_t states, void *context) {

  control_block *control = static_cast<control_block *>(context);

  constexpr void *event_handles[]{
      [fastware::value(fastware::event_type_t::WINDOW_SHOWN)] =
          &&WINDOW_SHOWN_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_HIDDEN)] =
          &&WINDOW_HIDDEN_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_EXPOSED)] =
          &&WINDOW_EXPOSED_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_MOVED)] =
          &&WINDOW_MOVED_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_RESIZED)] =
          &&WINDOW_RESIZED_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_SIZE_CHANGED)] =
          &&WINDOW_SIZE_CHANGED_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_MINIMIZED)] =
          &&WINDOW_MINIMIZED_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_MAXIMIZED)] =
          &&WINDOW_MAXIMIZED_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_RESTORED)] =
          &&WINDOW_RESTORED_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_ENTER)] =
          &&WINDOW_ENTER_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_LEAVE)] =
          &&WINDOW_LEAVE_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_FOCUS_GAINED)] =
          &&WINDOW_FOCUS_GAINED_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_FOCUS_LOST)] =
          &&WINDOW_FOCUS_LOST_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_CLOSE)] =
          &&WINDOW_CLOSE_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_TAKE_FOCUS)] =
          &&WINDOW_TAKE_FOCUS_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_HIT_TEST)] =
          &&WINDOW_HIT_TEST_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_KEY)] = &&WINDOW_KEY_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_MOUSE_MOVE)] =
          &&WINDOW_MOUSE_MOVE_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_MOUSE_WHEEL)] =
          &&WINDOW_MOUSE_WHEEL_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_MOUSE_BUTTON)] =
          &&WINDOW_MOUSE_BUTTON_LBL,
      [fastware::value(fastware::event_type_t::WINDOW_TEXT_EDIT)] =
          &&WINDOW_TEXT_EDIT_LBL,
      [fastware::value(fastware::event_type_t::END_STREAM)] = &&END_STREAM_LBL,
  };

  const fastware::event_t *eve = nullptr;
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
  fastware::logger::log("Window closing %u", eve->window_id);
  if (control->main_window_id == eve->window_id) {
    fastware::destroy_window(control->main_window_id);
    control->main_window_id = 0;
  }

  goto *next();

WINDOW_TAKE_FOCUS_LBL:
  goto *next();

WINDOW_HIT_TEST_LBL:
  goto *next();

WINDOW_KEY_LBL:
  if (eve->key.action == fastware::input::action::RELEASE) {
    switch (eve->key.keycode) {
    case fastware::input::key::KEY_KP_PLUS: {
      fastware::clock::set_game_speed(fastware::clock::game_speed() + 1);
      break;
    }
    case fastware::input::key::KEY_KP_MINUS: {
      fastware::clock::set_game_speed(fastware::clock::game_speed() - 1);
      break;
    }
    case fastware::input::key::KEY_0: {
      control->mode = 0;
      break;
    }
    case fastware::input::key::KEY_1: {
      control->mode = 1;
      break;
    }
    case fastware::input::key::KEY_2: {
      control->mode = 2;
      break;
    }
    case fastware::input::key::KEY_3: {
      control->mode = 3;
      break;
    }
    case fastware::input::key::KEY_ESCAPE: {
      fastware::destroy_window(control->main_window_id);
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

  float duration = float(fastware::clock::system_time_delta()) / FRAME;
  fastware::logger::log(
      "Frame duration ratio: %f, Expected time: %f, Frame time: %ld", duration,
      FRAME, fastware::clock::system_time_delta());

  if (states.current_key_states[fastware::value(fastware::input::key::KEY_Q)]) {
    control->cam =
        fastware::pan_horizontal(control->cam, glm::radians(1.f) * duration);
  }

  if (states.current_key_states[fastware::value(fastware::input::key::KEY_E)]) {
    control->cam =
        fastware::pan_horizontal(control->cam, glm::radians(-1.f) * duration);
  }

  if (states.current_key_states[fastware::value(fastware::input::key::KEY_W)]) {
    control->cam = fastware::move_parallel(control->cam, 6.f * duration);
  }

  if (states.current_key_states[fastware::value(fastware::input::key::KEY_S)]) {
    control->cam = fastware::move_parallel(control->cam, -6.f * duration);
  }

  if (states.current_key_states[fastware::value(fastware::input::key::KEY_A)]) {
    control->cam = fastware::move_perpendicular(control->cam, -6.f * duration);
  }

  if (states.current_key_states[fastware::value(fastware::input::key::KEY_D)]) {
    control->cam = fastware::move_perpendicular(control->cam, 6.f * duration);
  }

  if (states.current_key_states[fastware::value(
          fastware::input::key::KEY_PAGEUP)]) {
    control->cam = fastware::move_vertically(control->cam, 4.f * duration);
  }

  if (states.current_key_states[fastware::value(
          fastware::input::key::KEY_PAGEDOWN)]) {
    control->cam = fastware::move_vertically(control->cam, -4.f * duration);
  }

  return;
}

int main() {

  using namespace fastware;

  SystemAlloc alloc;

  logger::init_logger(alloc.root_alloc, 8 * 1024 * 64);

  control_block control;

  window_system ws(alloc.root_alloc, process_events, &control);
  ws.frame_limiter(toggle::ON);

  window_create_info window_create_info{"Fastware", 0, 0};
  window_info info;
  control.main_window_id = create_window(&window_create_info, &info);

  debug_init();

  File vert_shader_source("shaders/basic2.vert");
  size_t vert_size = vert_shader_source.size();
  memory::memblk vert_blk = memory::allocate(alloc.root_alloc, vert_size);
  char *vert = static_cast<char *>(vert_blk.ptr);
  vert_shader_source.read(vert, vert_size);

  File frag_shader_source("shaders/basic2.frag");
  size_t frag_size = frag_shader_source.size();
  memory::memblk frag_blk = memory::allocate(alloc.root_alloc, frag_size);
  char *frag = static_cast<char *>(frag_blk.ptr);
  frag_shader_source.read(frag, frag_size);

  logger::log("************** Begin Vertex Shader "
              "**************\n%.*s\n*************** End Vertex "
              "Shader ***************",
              vert_size, vert);
  logger::log("************** Begin Fragment Shader "
              "**************\n%.*s\n*************** End Fragment "
              "Shader ***************",
              frag_size, frag);

  shader_source_t shaders[]{
      {vert, static_cast<uint32_t>(vert_size), shader_type::VERTEX},
      {frag, static_cast<uint32_t>(frag_size), shader_type::FRAGMENT}};

  entity e;
  e.program_id = program::create(shaders, 2);

  memory::deallocate(alloc.root_alloc, frag_blk);
  memory::deallocate(alloc.root_alloc, vert_blk);

  constexpr uint32_t instance_count = 75000;
  constexpr uint32_t units = 24;
  constexpr uint32_t vertex_count = sphere::vertex_count(units);
  constexpr uint32_t index_count = sphere::index_count(units);
  struct m_vert {
    glm::vec3 positions[vertex_count];
    glm::vec3 normals[vertex_count];
    glm::vec2 uvs[vertex_count];
  } vert_data;

  struct m_idx {
    uint32_t indexes[index_count];
  } idx_data;

  uint64_t vc = uint64_t(instance_count) * uint64_t(vertex_count);
  uint64_t tc = uint64_t(instance_count) * uint64_t(index_count) / 3;

  sphere::model_data_pos_normal_uv vert_tmp{
      vert_data.positions, vert_data.normals, vert_data.uvs, 0};

  sphere::model_indexes idx_tmp{idx_data.indexes, 0};

  sphere::generate(vert_tmp, idx_tmp, units);

  memory::memblk model_mat_blk =
      memory::allocate(alloc.root_alloc, instance_count * sizeof(glm::mat4));
  glm::mat4 *instanced_model_mat2 = static_cast<glm::mat4 *>(model_mat_blk.ptr);
  memory::memblk speeds_blk =
      memory::allocate(alloc.root_alloc, instance_count * sizeof(float));
  float *transform_speeds = static_cast<float *>(speeds_blk.ptr);

  sphere::create_instancing_transforms(instanced_model_mat2, transform_speeds,
                                       instance_count);

  memory::memblk anim_mat_blk =
      memory::allocate(alloc.root_alloc, instance_count * sizeof(glm::mat4));
  glm::mat4 *instanced_anim_mat2 = static_cast<glm::mat4 *>(anim_mat_blk.ptr);
  sphere::init_model_mat(instanced_anim_mat2, instance_count);

  memory::memblk mvp_blk =
      memory::allocate(alloc.root_alloc, instance_count * sizeof(glm::mat4));
  glm::mat4 *model = static_cast<glm::mat4 *>(mvp_blk.ptr);
  for (int32_t i = 0, c = instance_count; i < c; ++i) {
    model[i] = instanced_model_mat2[i] * instanced_anim_mat2[i];
  }

  const buffer_create_info_t buffer_infos[]{
      {buffer_type_e::STATIC, sizeof(m_vert), &vert_data},
      {buffer_type_e::STATIC, sizeof(m_idx), &idx_data},
      {buffer_type_e::DYNAMIC, instance_count * sizeof(glm::mat4), model}};

  uint32_t buffers[3]{0};
  buffer::create(buffer_infos, 3, buffers);

  constexpr vertex_buffer_section_definition_t per_vertex[3]{
      {data_type_e::FLOAT_VECTOR3},
      {data_type_e::FLOAT_VECTOR3},
      {data_type_e::FLOAT_VECTOR2}};

  constexpr vertex_buffer_section_definition_t per_instance[1]{
      {data_type_e::FLOAT_MATRIX4}};

  const vertex_buffer_definition_t infos[2]{
      {buffers[0], data_rate_e::PER_VERTEX, vertex_count, 3, per_vertex},
      {buffers[2], data_rate_e::PER_INSTANCE, instance_count, 1,
       &per_instance[0]}};

  const vertex_array_definition_t varray_info{infos, 2, buffers[1]};

  varray::create(&varray_info, 1, &e.varray_id);

  e.count = index_count;
  e.render_type = entity::INDEX_INSTANCED;
  e.instance_count = instance_count;

  control.cam =
      camera{glm::vec3(50.0f, 50.0f, 300.0f), glm::vec3(0.0f, -0.45f, -1.0f),
             glm::vec3(0.0f, 1.0f, 0.0f)};

  control.mode = 0;

  Matrixes m{camera_view(control.cam),
             glm::perspective(glm::radians(60.f),
                              float(info.width) / float(info.height), 0.001f,
                              10000.0f)};

  buffer_update_info_t bui_model[]{
      {buffers[2], 0, instance_count * sizeof(glm::mat4), model}};

  logger::log("Starting image");
  logger::flush();

  image_data img = load("textures/earth.jpg");

  param_info_t param_infos[2]{
      param_wrap_t{parameter_type::WRAP_S, wrap_e::CLAMP_TO_EDGE},
      param_wrap_t{parameter_type::WRAP_T, wrap_e::CLAMP_TO_EDGE}};

  uint8_t *img_data = static_cast<uint8_t *>(img.bytes);
  std::reverse(img_data, img_data + (img.width + img.height));

  texture_create_info_t texture_info{img.width, img.height, img.format,
                                     2,         img_data,   param_infos};

  sampler_create_info_t sampler_info{2, param_infos};

  uint32_t texture_id = 0;
  uint32_t sampler_id = 0;

  texture::create_2d(&texture_info, 1, &texture_id);
  sampler::create(&sampler_info, 1, &sampler_id);

  unload(img);

  uniform::set_value(e.program_id, 11, 0);

  texture::bind(0, 1, &texture_id);
  sampler::bind(0, 1, &sampler_id);

  uniform::set_value(e.program_id, 12, glm::vec3(0, 0, 0));
  uniform::set_value(e.program_id, 13, glm::vec3(255, 255, 175));
  uniform::set_value(e.program_id, 14, 1200.f);
  uniform::set_value(e.program_id, 15, control.mode);

  renderer_change state[]{{{DEPTH_TEST, toggle::ON}},
                          {{CULL_FACE, toggle::ON}},
                          {{DEPTH_TEST_FUNC, LESS}},
                          {{CULL_FACE_FUNC, BACK}}};

  execute_renderer_change(state, 4);

  clock::init();
  clock::set_game_speed(1);

  logger::flush();
  clock::update();

  while (control.main_window_id != 0) {
    ws.poll();

    m.view = camera_view(control.cam);

    glm::mat4 PV = m.projection * m.view;

    sphere::update_instancing_transforms(instanced_anim_mat2, transform_speeds,
                                         instance_count);
    for (int32_t i = 0, c = instance_count; i < c; ++i) {
      model[i] = instanced_model_mat2[i] * instanced_anim_mat2[i];
    }

    renderer::begin_frame();

    uniform::set_value(e.program_id, 7, PV);
    uniform::set_value(e.program_id, 15, control.mode);

    buffer::update(&bui_model[0], 1);

    renderer::render_targets(&e, 1);

    renderer::end_frame();

    swap_buffers(&control.main_window_id, 1);

    logger::flush();
    clock::update();
  }

  logger::log("Program end");
  logger::log(
      "Vertex count: %u, Index count: %u, Triangles per instance: %u, "
      "Instance count: %u, Vertex rendered: %lu, Triangles rendered: %lu",
      vertex_count, index_count, index_count / 3, instance_count, vc, tc);

  logger::flush();
  logger::deinit_logger();

  return 0;
}