#ifndef CAMERA_H
#define CAMERA_H

#include <fastware/types.h>

namespace fastware {

struct camera {
  vec3_t pos;
  vec3_t dir;
  vec3_t up;
};

mat4_t camera_view(camera c);

camera move_parallel(camera c, float_t rate);

camera move_perpendicular(camera c, float_t rate);

camera move_vertically(camera c, float_t rate);

camera rotate_horizontally(camera c, float_t rate);

camera pan_horizontal(camera c, float_t angle);

camera pan_vertical(camera c, float_t angle);

} // namespace fastware

#endif // CAMERA_H
