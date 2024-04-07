#include <fastware/camera.h>

#include <fastware/types.h>

namespace fastware {

mat4_t camera_view(camera c) {
  return glms_lookat(c.pos, glms_vec3_add(c.pos, c.dir), c.up);
}

camera move_parallel(camera c, float_t rate) {

  const vec3_t cr1 = glms_cross(c.dir, c.up);
  const vec3_t cr2 = glms_cross(c.up, cr1);
  const vec3_t crn = glms_normalize(cr2);
  const vec3_t crs = glms_vec3_scale(crn, rate);
  const vec3_t np = glms_vec3_add(c.pos, crs);

  return {np, c.dir, c.up};
}
camera move_perpendicular(camera c, float_t rate) {

  const vec3_t cr = glms_cross(c.dir, c.up);
  const vec3_t crn = glms_normalize(cr);
  const vec3_t crs = glms_vec3_scale(crn, rate);
  const vec3_t np = glms_vec3_add(c.pos, crs);

  return {np, c.dir, c.up};
}

camera move_vertically(camera c, float_t rate) {

  return {glms_vec3_add(c.pos, vec3_t{0, rate, 0}), c.dir, c.up};
}

camera rotate_horizontally(camera c, float_t rate) {

  const auto dir = glms_vec3_rotate(c.dir, rate, c.up);

  return {c.pos, dir, c.up};
}

camera pan_horizontal(camera c, float_t angle) {
  return {c.pos, glms_vec3_rotate(c.dir, angle, c.up), c.up};
}

camera pan_vertical(camera c, float_t angle) {
  return {c.pos, glms_vec3_rotate(c.dir, angle, c.up),
          glms_normalize(glms_cross(c.dir, c.up))};
}

} // namespace fastware