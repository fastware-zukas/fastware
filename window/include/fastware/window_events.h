#ifndef WINDOW_EVENTS_H
#define WINDOW_EVENTS_H

#include <fastware/types.h>

namespace fastware {

struct key_state_t {
  uint8_t current_key_states[512];
  uint8_t previous_key_states[512];
  input::mod_bits current_mods;
  input::mod_bits previous_mods;
};

enum class event_type_t {
  WINDOW_SHOWN,
  WINDOW_HIDDEN,
  WINDOW_EXPOSED,
  WINDOW_MOVED,
  WINDOW_RESIZED,
  WINDOW_SIZE_CHANGED,
  WINDOW_MINIMIZED,
  WINDOW_MAXIMIZED,
  WINDOW_RESTORED,
  WINDOW_ENTER,
  WINDOW_LEAVE,
  WINDOW_FOCUS_GAINED,
  WINDOW_FOCUS_LOST,
  WINDOW_CLOSE,
  WINDOW_TAKE_FOCUS,
  WINDOW_HIT_TEST,
  WINDOW_KEY,
  WINDOW_MOUSE_MOVE,
  WINDOW_MOUSE_WHEEL,
  WINDOW_MOUSE_BUTTON,
  WINDOW_TEXT_EDIT,
  END_STREAM
};

constexpr int32_t value(event_type_t t) { return static_cast<int32_t>(t); }

struct window_shown_t {};
struct window_hidden_t {};
struct window_exposed_t {};
struct window_moved_t {
  int32_t xpos;
  int32_t ypos;
};
struct window_resized_t {
  int32_t width;
  int32_t height;
};
struct window_size_changed_t {
  int32_t width;
  int32_t height;
};
struct window_minimized_t {};
struct window_maximized_t {};
struct window_restored_t {};
struct window_mouse_entered_t {};
struct window_mouse_leave_t {};
struct window_focus_gained_t {};
struct window_focus_lost_t {};
struct window_close_t {};
struct window_take_focus_t {};
struct window_hit_test_t {};
struct window_key_t {
  input::key keycode;
  input::mod_bits modifiers;
  input::action action;
  toggle repeat;
};
struct window_mouse_move_t {
  int32_t x;
  int32_t y;
  int32_t relx;
  int32_t rely;
  input::mouse_button_bits button;
};
struct window_mouse_wheel_t {
  int32_t x;
  int32_t y;
  input::mouse_wheel_direction direction;
};
struct window_mouse_button_t {
  input::mouse_button_bits button;
  input::action action;
  input::mod_bits modifiers;
  input::mouse_click_count clicks;
  int32_t x;
  int32_t y;
};
struct window_text_edit_t {
  const char *text;
  int32_t start;
  int32_t length;
};

struct event_t {
  event_type_t type;
  uint32_t window_id;
  union {
    window_shown_t shown;
    window_hidden_t hidden;
    window_exposed_t exposed;
    window_moved_t moved;
    window_resized_t resized;
    window_size_changed_t size_changed;
    window_minimized_t minimized;
    window_maximized_t maximized;
    window_restored_t restored;
    window_mouse_entered_t mouse_entered;
    window_mouse_leave_t mouse_leave;
    window_focus_gained_t focus_gained;
    window_focus_lost_t focus_lost;
    window_close_t close;
    window_take_focus_t take_focus;
    window_hit_test_t hit_test;
    window_key_t key;
    window_mouse_move_t mouse_move;
    window_mouse_wheel_t mouse_wheel;
    window_mouse_button_t mouse_button;
    window_text_edit_t text_edit;
  };
};

} // namespace fastware

#endif // WINDOW_EVENTS_H
