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

#include <fastware/bounding.h>
#include <fastware/camera.h>
#include <fastware/data_types.h>
#include <fastware/debug.h>
#include <fastware/entity.h>
#include <fastware/image_source.h>
#include <fastware/renderer.h>
#include <fastware/renderer_state.h>
#include <fastware/stopwatch.h>
#include <fastware/utils.h>

#include "game_setup.h"
#include "geometry.h"

template <typename T> struct allocator {
  static T *alloc(fastware::memory::allocator_t *alloc) {
    return static_cast<T *>(fastware::memory::allocate(alloc, sizeof(T)).ptr);
  }

  static void dealloc(fastware::memory::allocator_t *alloc, T *ptr) {
    fastware::memory::deallocate(
        alloc, {.ptr = ptr, fastware::memory::prefered_size(alloc, sizeof(T))});
  }
};
struct SystemAlloc {
  SystemAlloc() {
    fastware::memory::stack_alloc_create_info_t alloc_create_info{
        nullptr, 500 * 1024 * 1024, fastware::memory::alignment_t::b64};

    root_alloc = fastware::memory::create(&alloc_create_info);
  }

  ~SystemAlloc() { fastware::memory::destroy(root_alloc); }

  fastware::memory::allocator_t *root_alloc;
};

int main() {

  using namespace fastware;

  constexpr uint32_t instance_count = 50000;
  constexpr uint32_t units = 24;
  constexpr uint32_t vertex_count = geometry::sphere::vertex_count(units);
  constexpr uint32_t index_count = geometry::sphere::index_count(units);

  constexpr uint64_t vc = uint64_t(instance_count) * uint64_t(vertex_count);
  constexpr uint64_t tc = uint64_t(instance_count) * uint64_t(index_count) / 3;

  SystemAlloc alloc;

  logger::init_logger(alloc.root_alloc, memory::Mb * 100);

  setup::control_block control{.cam = camera{glm::vec3(50.0f, 50.0f, 300.0f),
                                             glm::vec3(0.0f, -0.45f, -1.0f),
                                             glm::vec3(0.0f, 1.0f, 0.0f)},
                               .main_window_id = 0,
                               .mode = 0,
                               .show_bounding_box = false};

  window_system ws(alloc.root_alloc, setup::process_events, &control);
  ws.frame_limiter(toggle::SELECTIVE);

  window_create_info window_create_info{"Fastware", 0, 0};
  window_info info;
  control.main_window_id = create_window(&window_create_info, &info);

  debug_init();

  setup::shader_source shaders[]{
      {.filename = "shaders/basic2.vert", .type = shader_type_e::VERTEX},
      {.filename = "shaders/basic2.frag", .type = shader_type_e::FRAGMENT}};

  const uint32_t prog_id = setup::create_program(alloc.root_alloc, shaders, 2);

  struct vertex_data {
    glm::vec3 positions[vertex_count];
    glm::vec3 normals[vertex_count];
    glm::vec2 uvs[vertex_count];
  };

  struct index_data {
    uint32_t indexes[index_count];
  };

  struct prep_matrixes {
    glm::mat4 models[instance_count];
    glm::mat4 animations[instance_count];
    float speeds[instance_count];
  };

  struct gpu_matrixes {
    glm::mat4 model_transforms[instance_count];
    glm::mat3 normal_transforms[instance_count];
  };

  vertex_data *vert_data = allocator<vertex_data>::alloc(alloc.root_alloc);
  index_data *idx_data = allocator<index_data>::alloc(alloc.root_alloc);
  gpu_matrixes *gpu_mat_data = allocator<gpu_matrixes>::alloc(alloc.root_alloc);

  geometry::sphere::generate(vert_data->positions, vert_data->normals,
                             vert_data->uvs, idx_data->indexes, units);

  const buffer_create_info_t buffer_infos[]{
      {.target = buffer_target_e::ARRAY_BUFFER,
       .type = buffer_type_e::STATIC,
       .size = sizeof(vertex_data),
       .data = vert_data},
      {.target = buffer_target_e::ELEMENT_ARRAY_BUFFER,
       .type = buffer_type_e::STATIC,
       .size = sizeof(index_data),
       .data = idx_data},
      {.target = buffer_target_e::ARRAY_BUFFER,
       .type = buffer_type_e::DYNAMIC,
       .size = sizeof(gpu_matrixes),
       .data = gpu_mat_data}};

  uint32_t buffers[3]{0};
  buffer::create(buffer_infos, 3, buffers);

  constexpr vertex_buffer_section_definition_t per_vertex[3]{
      {data_type_e::FLOAT_VECTOR3},
      {data_type_e::FLOAT_VECTOR3},
      {data_type_e::FLOAT_VECTOR2}};

  constexpr vertex_buffer_section_definition_t per_instance[2]{
      {data_type_e::FLOAT_MATRIX4}, {data_type_e::FLOAT_MATRIX3}};

  const vertex_buffer_definition_t infos[2]{
      {buffers[0], data_rate_e::PER_VERTEX, vertex_count, 3, per_vertex},
      {buffers[2], data_rate_e::PER_INSTANCE, instance_count, 2,
       &per_instance[0]}};

  const vertex_array_definition_t varray_info{infos, 2, buffers[1]};

  uint32_t vert_id = 0;
  varray::create(&varray_info, 1, &vert_id);

  allocator<index_data>::dealloc(alloc.root_alloc, idx_data);
  allocator<vertex_data>::dealloc(alloc.root_alloc, vert_data);

  entity e{.program_id = prog_id,
           .varray_id = vert_id,
           .offset = 0,
           .count = index_count,
           .instance_count = instance_count,
           .primitive_type = primitive_type_e::TRIANGLES,
           .render_type = entity::INDEX_INSTANCED};

  buffer_update_info_t gpu_matrix_buffer_update[]{
      {buffers[2], 0, sizeof(gpu_matrixes), gpu_mat_data}};

  prep_matrixes *prep_mat_data =
      allocator<prep_matrixes>::alloc(alloc.root_alloc);

  geometry::matrix::fill(prep_mat_data->animations, instance_count,
                         glm::mat4(1.f));
  setup::create_transforms(prep_mat_data->models, instance_count);
  setup::create_speeds(prep_mat_data->speeds, instance_count);

  setup::shader_source bounding_shaders[]{
      {.filename = "shaders/bounding_box.vert", .type = shader_type_e::VERTEX},
      {.filename = "shaders/bounding_box.frag",
       .type = shader_type_e::FRAGMENT}};

  const uint32_t bounding_prog_id =
      setup::create_program(alloc.root_alloc, bounding_shaders, 2);

  const glm::mat4 bounds =
      compute_bounding_box(vert_data->positions, vertex_count);
  struct bounding_box_instances {
    glm::mat4 models[instance_count];
  };

  bounding_box_instances *gpu_bounding_data =
      allocator<bounding_box_instances>::alloc(alloc.root_alloc);

  buffer_update_info_t bounding_box_update_buffer;
  const uint32_t bounding_vid = create_bounding_box_vao(
      gpu_bounding_data->models, instance_count, &bounding_box_update_buffer);

  entity bounding_e[3]{{.program_id = bounding_prog_id,
                        .varray_id = bounding_vid,
                        .offset = 0,
                        .count = 4,
                        .instance_count = instance_count,
                        .primitive_type = primitive_type_e::LINE_LOOP,
                        .render_type = entity::INDEX_INSTANCED},
                       {.program_id = bounding_prog_id,
                        .varray_id = bounding_vid,
                        .offset = 4,
                        .count = 4,
                        .instance_count = instance_count,
                        .primitive_type = primitive_type_e::LINE_LOOP,
                        .render_type = entity::INDEX_INSTANCED},
                       {.program_id = bounding_prog_id,
                        .varray_id = bounding_vid,
                        .offset = 8,
                        .count = 8,
                        .instance_count = instance_count,
                        .primitive_type = primitive_type_e::LINES,
                        .render_type = entity::INDEX_INSTANCED}};

  uint32_t texture_id =
      setup::create_texture(alloc.root_alloc, "textures/earth.jpg");

  param_info_t param_infos[2]{
      param_wrap_t{parameter_type::WRAP_S, wrap_e::CLAMP_TO_EDGE},
      param_wrap_t{parameter_type::WRAP_T, wrap_e::CLAMP_TO_EDGE}};
  sampler_create_info_t sampler_info{2, param_infos};

  uint32_t sampler_id = 0;
  sampler::create(&sampler_info, 1, &sampler_id);

  program::select(e.program_id);

  uint32_t texture_bind_location = 0;
  texture::bind(texture_bind_location, 1, &texture_id);
  sampler::bind(texture_bind_location, 1, &sampler_id);

  uniform::set_sampler_value(e.program_id, 11, texture_bind_location);
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

  setup::matrixes mats{
      .view = camera_view(control.cam),
      .projection = glm::perspective(glm::radians(60.f),
                                     float(info.width) / float(info.height),
                                     0.01f, 2000.0f)};

  while (control.main_window_id != 0) {

    METRIC(RenderLoop);

    {
      METRIC(EventPolling);
      ws.poll();
    }

    mats.view = camera_view(control.cam);

    glm::mat4 PV = mats.projection * mats.view;

    {
      METRIC(PrepModels);
      setup::update_transforms(prep_mat_data->animations, prep_mat_data->speeds,
                               instance_count);

      setup::compute_gpu_matrixes(
          gpu_mat_data->model_transforms, gpu_mat_data->normal_transforms,
          prep_mat_data->models, prep_mat_data->animations, instance_count);
    }
    {
      METRIC(PrepBoundBoxModels);

      setup::compute_bounding_model_matrixes(gpu_bounding_data->models,
                                             gpu_mat_data->model_transforms,
                                             instance_count, bounds);
    }
    {
      METRIC(BeginFrame);
      renderer::begin_frame();
    }
    {
      METRIC(RenderObject);

      uniform::set_value(e.program_id, 10, PV);
      uniform::set_value(e.program_id, 15, control.mode);

      buffer::update(&gpu_matrix_buffer_update[0], 1);

      renderer::render_targets(&e, 1);
    }
    if (control.show_bounding_box) {

      METRIC(RenderBoundBox);

      uniform::set_value(bounding_e[0].program_id, 7, PV);

      buffer::update(&bounding_box_update_buffer, 1);

      renderer::render_targets(bounding_e, 3);
    }
    {
      METRIC(SubmitFrame);
      renderer::end_frame();
    }
    {
      METRIC(SwapBuffers);
      swap_buffers(&control.main_window_id, 1);
    }
    {
      METRIC(EndFrameTasks);
      logger::flush();
      clock::update();
    }
  }

  logger::log("Program end");
  logger::log(
      "Vertex count: %u, Index count: %u, Triangles per instance: %u, "
      "Instance count: %u, Vertex rendered: %lu, Triangles rendered: %lu",
      vertex_count, index_count, index_count / 3, instance_count, vc, tc);

  sampler::destroy(&sampler_id, 1);
  texture::destroy(&texture_id, 1);

  varray::destroy(&vert_id, 1);
  buffer::destroy(buffers, 3);
  program::destroy(prog_id);

  logger::flush();
  logger::deinit_logger();

  return 0;
}