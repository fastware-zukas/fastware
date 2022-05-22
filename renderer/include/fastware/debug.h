#ifndef DEBUG_H
#define DEBUG_H

namespace fastware {
void debug_init();
void manualErrorCheck(const char *call, const char *name, long line);
} // namespace fastware

#define GLCALL(function, ...)                                                  \
  function(__VA_ARGS__);                                                       \
  fastware::manualErrorCheck(#function, __FUNCTION__, __LINE__)

#endif // DEBUG_H
