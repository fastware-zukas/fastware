#include <fastware/camera.h>

#include <glm/gtx/rotate_vector.hpp>

namespace fastware {

glm::mat4 camera_view(camera c) {
  return glm::lookAt(c.pos, c.pos + c.dir, c.up);
}

camera move_parallel(camera c, float_t rate) {

  const glm::vec3 cr1 = glm::cross(c.dir, c.up);
  const glm::vec3 cr2 = glm::cross(c.up, cr1);
  const glm::vec3 crn = glm::normalize(cr2);
  const glm::vec3 crs = crn * rate;
  const glm::vec3 np = c.pos + crs;

  return {np, c.dir, c.up};
}
camera move_perpendicular(camera c, float_t rate) {

  const glm::vec3 cr = glm::cross(c.dir, c.up);
  const glm::vec3 crn = glm::normalize(cr);
  const glm::vec3 crs = crn * rate;
  const glm::vec3 np = c.pos + crs;

  return {np, c.dir, c.up};
}

camera move_vertically(camera c, float_t rate) {

  return {c.pos + glm::vec3(0, rate, 0), c.dir, c.up};
}

camera rotate_horizontally(camera c, float_t rate) {
  const glm::vec3 nd = glm::rotate(c.dir, rate / 2.f, c.up);
  const glm::vec3 np = glm::rotate(c.pos, -rate / 2.f, c.up);

  return {np, nd, c.up};
}

camera pan_horizontal(camera c, float_t angle) {
  return {c.pos, glm::rotate(c.dir, angle, c.up), c.up};
}

camera pan_vertical(camera c, float_t angle) {
  return {c.pos, glm::rotate(c.dir, angle, c.up),
          glm::normalize(glm::cross(c.dir, c.up))};
}

} // namespace fastware