#include <fastware/window_system.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL.h>
#include <cassert>
#include <cstring>

#include <fastware/logger.h>
#include <fastware/memory.h>

namespace fastware {

constexpr int32_t max_events{64};

struct internal_data_t {
  event_t events[max_events];
  key_state_t key_states;
};

window_system::window_system(memory::allocator_t *parent_alloc,
                             event_processor processor, void *processor_context)
    : d_parent_alloc(parent_alloc), d_processor(processor),
      d_processor_context(processor_context), d_internal(nullptr) {

  memory::stack_alloc_create_info_t create_info{parent_alloc, 8 * 1024 * 1024,
                                                memory::alignment_t::b64};

  d_alloc = memory::create(&create_info);

  memory::memblk blk = memory::allocate(d_alloc, sizeof(internal_data_t));
  d_internal = static_cast<internal_data_t *>(blk.ptr);

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    logger::log("SDL could not initialize! SDL Error: %s", SDL_GetError());
    return;
  }
}

window_system::~window_system() {

  memory::destroy(d_alloc);
  SDL_Quit();
}

void window_system::poll() {

  SDL_Event sdl_events[max_events];

  SDL_PumpEvents();

  const int32_t count = SDL_PeepEvents(sdl_events, max_events - 1, SDL_GETEVENT,
                                       SDL_FIRSTEVENT, SDL_LASTEVENT);
  event_t *events = d_internal->events;

  int32_t out_idx = 0;
  for (int32_t i = 0; i < count; ++i) {
    const SDL_Event &e = sdl_events[i];
    switch (e.type) {
    case SDL_WINDOWEVENT: {
      switch (e.window.event) {
      case SDL_WINDOWEVENT_SHOWN:
        logger::log("SDL_WINDOWEVENT_SHOWN %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_SHOWN,
                             .window_id = e.window.windowID,
                             .shown = window_shown_t{}};
        break;
      case SDL_WINDOWEVENT_HIDDEN:
        logger::log("SDL_WINDOWEVENT_HIDDEN %u", e.window.windowID);
        events[out_idx++] = event_t{.type = event_type_t::WINDOW_HIDDEN,
                                    .window_id = e.window.windowID,
                                    .hidden = window_hidden_t{}};
        break;
      case SDL_WINDOWEVENT_EXPOSED:
        logger::log("SDL_WINDOWEVENT_EXPOSED %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_EXPOSED,
                             .window_id = e.window.windowID,
                             .exposed = window_exposed_t{}};
        break;
      case SDL_WINDOWEVENT_MOVED:
        logger::log("SDL_WINDOWEVENT_MOVED %u", e.window.windowID);
        events[out_idx++] = {
            .type = event_type_t::WINDOW_MOVED,
            .window_id = e.window.windowID,
            .moved = window_moved_t{e.window.data1, e.window.data2}};
        break;
      case SDL_WINDOWEVENT_RESIZED:
        logger::log("SDL_WINDOWEVENT_RESIZED %u", e.window.windowID);
        events[out_idx++] = {
            .type = event_type_t::WINDOW_RESIZED,
            .window_id = e.window.windowID,
            .resized = window_resized_t{e.window.data1, e.window.data2}};
        break;
      case SDL_WINDOWEVENT_SIZE_CHANGED:
        logger::log("SDL_WINDOWEVENT_SIZE_CHANGED %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_SIZE_CHANGED,
                             .window_id = e.window.windowID,
                             .size_changed = window_size_changed_t{
                                 e.window.data1, e.window.data2}};
        break;
      case SDL_WINDOWEVENT_MINIMIZED:
        logger::log("SDL_WINDOWEVENT_MINIMIZED %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_MINIMIZED,
                             .window_id = e.window.windowID,
                             .minimized = window_minimized_t{}};
        break;
      case SDL_WINDOWEVENT_MAXIMIZED:
        logger::log("SDL_WINDOWEVENT_MAXIMIZED %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_MAXIMIZED,
                             .window_id = e.window.windowID,
                             .maximized = window_maximized_t{}};
        break;
      case SDL_WINDOWEVENT_RESTORED:
        logger::log("SDL_WINDOWEVENT_RESTORED %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_RESTORED,
                             .window_id = e.window.windowID,
                             .restored = window_restored_t{}};
        break;
      case SDL_WINDOWEVENT_ENTER:
        logger::log("SDL_WINDOWEVENT_ENTER %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_ENTER,
                             .window_id = e.window.windowID,
                             .mouse_entered = window_mouse_entered_t{}};
        break;
      case SDL_WINDOWEVENT_LEAVE:
        logger::log("SDL_WINDOWEVENT_LEAVE %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_LEAVE,
                             .window_id = e.window.windowID,
                             .mouse_leave = window_mouse_leave_t{}};
        break;
      case SDL_WINDOWEVENT_FOCUS_GAINED:
        logger::log("SDL_WINDOWEVENT_FOCUS_GAINED %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_FOCUS_GAINED,
                             .window_id = e.window.windowID,
                             .focus_gained = window_focus_gained_t{}};
        break;
      case SDL_WINDOWEVENT_FOCUS_LOST:
        logger::log("SDL_WINDOWEVENT_FOCUS_LOST %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_FOCUS_LOST,
                             .window_id = e.window.windowID,
                             .focus_lost = window_focus_lost_t{}};
        break;
      case SDL_WINDOWEVENT_CLOSE:
        logger::log("SDL_WINDOWEVENT_CLOSE %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_CLOSE,
                             .window_id = e.window.windowID,
                             .close = window_close_t{}};
        break;
      case SDL_WINDOWEVENT_TAKE_FOCUS:
        logger::log("SDL_WINDOWEVENT_TAKE_FOCUS %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_TAKE_FOCUS,
                             .window_id = e.window.windowID,
                             .take_focus = window_take_focus_t{}};
        break;
      case SDL_WINDOWEVENT_HIT_TEST:
        logger::log("SDL_WINDOWEVENT_HIT_TEST %u", e.window.windowID);
        events[out_idx++] = {.type = event_type_t::WINDOW_HIT_TEST,
                             .window_id = e.window.windowID,
                             .hit_test = window_hit_test_t{}};
        break;
      default:
        logger::log("Window %d got unknown event %d", e.window.windowID,
                    e.window.event);
        break;
      }
      break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      logger::log("SDL_KEY %u", e.window.windowID);
      events[out_idx++] = {
          .type = event_type_t::WINDOW_KEY,
          .window_id = e.window.windowID,
          .key = window_key_t{static_cast<input::key_e>(e.key.keysym.scancode),
                              static_cast<input::mod_bits_e>(e.key.keysym.mod),
                              static_cast<input::action_e>(e.key.state),
                              static_cast<toggle_e>(e.key.repeat)}};
      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      logger::log("SDL_MOUSEBUTTON %u", e.window.windowID);
      events[out_idx++] = {
          .type = event_type_t::WINDOW_MOUSE_BUTTON,
          .window_id = e.window.windowID,
          .mouse_button = window_mouse_button_t{
              static_cast<input::mouse_button_bits_e>(e.button.button),
              static_cast<input::action_e>(e.button.state),
              static_cast<input::mod_bits_e>(SDL_GetModState()),
              static_cast<input::mouse_click_count_e>(e.button.clicks),
              e.button.x, e.button.y}};
      break;
    }
    case SDL_MOUSEMOTION: {
      logger::log("SDL_MOUSEMOTION %u", e.window.windowID);
      events[out_idx++] = {
          .type = event_type_t::WINDOW_MOUSE_MOVE,
          .window_id = e.window.windowID,
          .mouse_move = window_mouse_move_t{
              e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel,
              static_cast<input::mouse_button_bits_e>(e.motion.state)}};
      break;
    }
    case SDL_MOUSEWHEEL: {
      logger::log("SDL_MOUSEWHEEL %u", e.window.windowID);
      events[out_idx++] = {
          .type = event_type_t::WINDOW_MOUSE_WHEEL,
          .window_id = e.window.windowID,
          .mouse_wheel = window_mouse_wheel_t{
              e.wheel.x, e.wheel.y,
              static_cast<input::mouse_wheel_direction_e>(e.wheel.direction)}};
      break;
    }
    case SDL_TEXTEDITING: {
      logger::log("SDL_TEXTEDITING %u", e.window.windowID);
      events[out_idx++] = {.type = event_type_t::WINDOW_TEXT_EDIT,
                           .window_id = e.window.windowID,
                           .text_edit = window_text_edit_t{
                               e.edit.text, e.edit.start, e.edit.length}};
      break;
    }
    }
  }

  events[out_idx++] = {.type = event_type_t::END_STREAM};

  d_internal->key_states.previous_mods = d_internal->key_states.current_mods;
  memcpy(d_internal->key_states.previous_key_states,
         d_internal->key_states.current_key_states,
         sizeof(d_internal->key_states.current_key_states));
  d_internal->key_states.current_mods =
      static_cast<input::mod_bits_e>(SDL_GetModState());
  memcpy(d_internal->key_states.current_key_states,
         SDL_GetKeyboardState(nullptr),
         sizeof(d_internal->key_states.current_key_states));

  d_processor(events, count, d_internal->key_states, d_processor_context);
}

void window_system::frame_limiter(toggle_e state) const {
  SDL_GL_SetSwapInterval(value(state));
}

} // namespace fastware