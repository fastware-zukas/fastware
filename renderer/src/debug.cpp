#include <fastware/debug.h>

#include <GL/glew.h>

#include <fastware/logger.h>

const char *type_select(GLenum type) {
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    return "ERROR";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "DEPRECATED_BEHAVIOR";

  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "UNDEFINED_BEHAVIOR";

  case GL_DEBUG_TYPE_PORTABILITY:
    return "PORTABILITY";

  case GL_DEBUG_TYPE_PERFORMANCE:
    return "PERFORMANCE";

  case GL_DEBUG_TYPE_OTHER:
    return "OTHER";
  }

  return "UNKNOWN";
}

const char *severity_select(GLenum severity) {
  switch (severity) {
  case GL_DEBUG_SEVERITY_LOW:
    return "LOW";
  case GL_DEBUG_SEVERITY_MEDIUM:
    return "MEDIUM";
  case GL_DEBUG_SEVERITY_HIGH:
    return "HIGH";
  }
  return "UNKNOWN";
}

void GLAPIENTRY bebug_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei, const GLchar *message,
                               const void *) {
  fastware::logger::log("[GL DEBUG] type = %s severity = %s, message = %s",
                        type_select(type), severity_select(severity), message);
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
