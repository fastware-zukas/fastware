#include <fastware/stopwatch.h>

#include <fastware/clock.h>
#include <fastware/logger.h>

namespace fastware {

namespace utils {

stopwatch::stopwatch(const char *context_name, const char *function_name)
    : d_context_name(context_name), d_function_name(function_name),
      d_start_time(clock::system_time()) {}

stopwatch::~stopwatch() {
  int64_t duration = clock::system_time() - d_start_time;
  logger::log("[Metrics] %s %s: %.2f us", d_context_name, d_function_name,
              float(duration) / 1000.f);
}
} // namespace utils
} // namespace fastware