#ifndef CLOCK_H
#define CLOCK_H

#include <cstdint>

namespace fastware {

namespace clock {

void init();

void update();

int64_t system_time_delta();

int64_t game_time_delta();

int64_t system_time_elapsed();

int64_t system_time();

void set_game_speed(int64_t speed);

int64_t game_speed();

} // namespace clock
} // namespace fastware

#endif // CLOCK_H
