#ifndef LOGGER_H
#define LOGGER_H

#include <fastware/fastware_def.h>

namespace fastware {

namespace memory {
typedef struct allocator_t allocator_t;
}

namespace logger {
void init_logger(memory::allocator_t *parent_allocator, int64_t buffer_size);

void log(const char *__restrict format, ...);

void flush();

void deinit_logger();

} // namespace logger

} // namespace fastware
#endif // LOGGER_H
