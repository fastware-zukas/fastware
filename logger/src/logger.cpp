#include <fastware/logger.h>

#include <fastware/math.h>
#include <fastware/memory.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <stdarg.h>

#include <atomic>

namespace {
static struct logger_store_t {
  fastware::memory::allocator_t *allocator;
  struct log_buffer_t {
    fastware::memory::memblk block;
    std::atomic_uint32_t position;
    std::atomic_uint32_t writers;
  } buffers[2];
  std::atomic_int buffer_idx;
  int8_t __padding[4];
} _logger_store;
} // namespace

void fastware::logger::init_logger(memory::allocator_t *parent_allocator,
                                   int64_t buffer_size) {

  _logger_store.allocator = parent_allocator;

  _logger_store.buffers[0].block =
      memory::allocate(parent_allocator, buffer_size);
  atomic_init(&(_logger_store.buffers[0].position), 0);
  atomic_init(&(_logger_store.buffers[0].writers), 0);
  _logger_store.buffers[1].block =
      memory::allocate(parent_allocator, buffer_size);
  atomic_init(&(_logger_store.buffers[1].position), 0);
  atomic_init(&(_logger_store.buffers[1].writers), 0);
  atomic_init(&(_logger_store.buffer_idx), 0);
}

void fastware::logger::deinit_logger() {
  memory::deallocate(_logger_store.allocator, _logger_store.buffers[0].block);
  memory::deallocate(_logger_store.allocator, _logger_store.buffers[1].block);
}

void fastware::logger::log(const char *__restrict format, ...) {

  // Start with 6 chars + 22 timestamp + 1 char space = 29 chars (max)
  // '[INFO][timestamp] '
  // End with new line char = 2 byte '\n\n' Total added size = 31 bytes
  constexpr int32_t meta_size = 31;
  constexpr int32_t buffer_size = 4196;
  char buffer[buffer_size]{};
  va_list args;
  va_start(args, format);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
  const int32_t buffer_length = vsnprintf(buffer, buffer_size, format, args);
#pragma GCC diagnostic pop
  va_end(args);
  const uint32_t total_buffer_length =
      static_cast<uint32_t>(buffer_length + meta_size);

  const int32_t idx = atomic_load_explicit(&(_logger_store.buffer_idx),
                                           std::memory_order_acquire);
  logger_store_t::log_buffer_t &log_buffer = _logger_store.buffers[idx];
  atomic_fetch_add_explicit(&(log_buffer.writers), 1,
                            std::memory_order_acq_rel);
  const uint32_t pos = atomic_fetch_add_explicit(
      &(log_buffer.position), total_buffer_length, std::memory_order_acq_rel);
  char *buffer_ptr = &(static_cast<char *>(log_buffer.block.ptr))[pos];

  auto now =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();

  snprintf(buffer_ptr, total_buffer_length, "[INFO][%lu] %.*s\n\n", now,
           buffer_length, buffer);
  atomic_fetch_sub_explicit(&(log_buffer.writers), 1,
                            std::memory_order_acq_rel);
}

void fastware::logger::flush() {
  const int32_t idx = atomic_fetch_xor_explicit(&(_logger_store.buffer_idx), 1,
                                                std::memory_order_acq_rel);
  logger_store_t::log_buffer_t &log_buffer = _logger_store.buffers[idx];
  while (atomic_load_explicit(&(log_buffer.writers), std::memory_order_acquire))
    ;
  const int64_t length = atomic_exchange_explicit(&(log_buffer.position), 0,
                                                  std::memory_order_acq_rel);
  if (length > 0) {
    fwrite(static_cast<char *>(log_buffer.block.ptr), sizeof(char),
           static_cast<uint64_t>(length), stdout);
    fflush(stdout);
  }
}
