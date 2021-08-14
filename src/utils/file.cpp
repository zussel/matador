#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"

#include <stdexcept>

namespace matador {

file::file(const char *path, const char *mode)
  : file(std::string(path), mode)
{}

file::file(const std::string &path, const char *mode)
{
  open(path, mode);
}

file::~file()
{
  close();
}

void file::open(const char *path, const char *mode)
{
  open(std::string(path), mode);
}

void file::open(const std::string &path, const char *mode)
{
  close();
  path_ = path;
  stream_ = os::fopen(path, mode);
}

void file::close()
{
  if (stream_ != nullptr) {
    os::fclose(stream_);
    stream_ = nullptr;
  }
}

size_t file::size() const
{
  return os::file_size(stream_);
}

std::string file::path() const
{
  return path_;
}

FILE *file::stream() const
{
  return stream_;
}

bool file::is_open() const
{
  return stream_ != nullptr;
}

std::string read_as_text(const file &f)
{
  if (!f.is_open()) {
    throw std::logic_error("file is not open");
  }

// obtain file size:
  fseek (f.stream() , 0 , SEEK_END);
  size_t size = ftell(f.stream());
  rewind (f.stream());

  std::string result(size, '\0');

  auto ret = fread(const_cast<char*>(result.data()), 1, size, f.stream());

  if (ret == 0 && ferror(f.stream())) {
    char error_buffer[1024];
    os::strerror(errno, error_buffer, 1024);
    throw std::logic_error(error_buffer);
  }

  return result;
}
}
