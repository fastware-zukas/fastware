#include <fastware/clock.h>

#include <chrono>

namespace fastware {

namespace clock {

using timer = std::chrono::high_resolution_clock;
using duration = std::chrono::duration<int64_t, std::nano>;

static struct {
  int64_t system_delta{0};
  int64_t game_delta{0};
  int64_t game_speed{1};
  timer::time_point current_time{};
  timer::time_point start_time{};
} clock_data{};

void init() { clock_data.current_time = clock_data.start_time = timer::now(); }

void update() {
  auto tmp = timer::now();
  int64_t delta = duration(tmp - clock_data.current_time).count();
  clock_data.system_delta = delta;
  clock_data.game_delta = delta * clock_data.game_speed;
  clock_data.current_time = tmp;
}

int64_t system_time_delta() { return clock_data.system_delta; }

int64_t game_time_delta() { return clock_data.game_delta; }

int64_t system_time_elapsed() {
  return duration(clock_data.current_time - clock_data.start_time).count();
}

int64_t system_time() { return timer::now().time_since_epoch().count(); }

void set_game_speed(int64_t speed) { clock_data.game_speed = speed; }

int64_t game_speed() { return clock_data.game_speed; }

} // namespace clock
} // namespace fastware
