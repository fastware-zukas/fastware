#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <fastware/fastware_def.h>

namespace fastware {

namespace memory {
typedef struct allocator_t allocator_t;
}

namespace utils {

class stopwatch {

public:
  stopwatch(const char *context_name, const char *function_name);
  ~stopwatch();

private:
  const char *d_context_name;
  const char *d_function_name;
  const int64_t d_start_time;
};

#define METRIC(context)                                                        \
  fastware::utils::stopwatch VARNAME(__stop_watch_)(#context, __func__)

} // namespace utils
} // namespace fastware
#endif // STOPWATCH_H
