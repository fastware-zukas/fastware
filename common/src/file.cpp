#include <fastware/file.h>

#include <cassert>
#include <cstdio>

File::File(const char *filename) {
  file_handle_ = fopen(filename, "rb");
  if (file_handle_) {
    fseek(file_handle_, 0, SEEK_END);
    long len = ftell(file_handle_);
    fseek(file_handle_, 0, SEEK_SET);
    size_ = static_cast<size_t>(len);
  } else {
    size_ = 0;
  }
}

File::~File() {
  if (file_handle_)
    fclose(file_handle_);
}

size_t File::size() const { return size_; }

size_t File::read(char *buffer, size_t read_size) {
  assert(size_ >= read_size);
  size_t res = fread(buffer, 1, read_size, file_handle_);
  return res;
}
