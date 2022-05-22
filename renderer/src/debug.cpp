#include <fastware/debug.h>

#include <GL/glew.h>

#include <fastware/logger.h>

void GLAPIENTRY bebug_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar *message, const void *userParam) {
  fastware::logger::log("[GLLOG] %s type = 0x%x, severity = 0x%x, message = %s",
                        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                        type, severity, message);
  fastware::logger::flush();
}

void fastware::debug_init() {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(bebug_callback, nullptr);
}

void fastware::manualErrorCheck(const char *call, const char *name, long line) {

  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    fastware::logger::log("[GLLOG] %s:%ld CALL: %s ERROR: %d", name, line, call,
                          err);
  }
}
