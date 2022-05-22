#ifndef FILE_H
#define FILE_H

#include <cstddef>

class File {
public:
  File(const char *filename);
  ~File();

  size_t size() const;
  size_t read(char *buffer, size_t read_size);

private:
  struct _IO_FILE *file_handle_;
  size_t size_;
};

#endif // FILE_H
