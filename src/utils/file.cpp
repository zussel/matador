#include "matador/utils/file.hpp"
#include "matador/utils/os.hpp"

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
  path_ = path;
  stream_ = os::fopen(path, mode);
}

void file::close()
{
  if (stream_ != nullptr) {
    fclose(stream_);
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
}
