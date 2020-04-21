#include "matador/utils/os.hpp"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace matador {
namespace os {
FILE* fopen(const std::string &path, const char *modes)
{
  return fopen(path.c_str(), modes);
}

FILE* fopen(const char *path, const char *modes)
{
#ifdef _WIN32
  FILE *stream;
  fopen_s(&stream, path.c_str(), "a");
  return stream;
#else
  return ::fopen(path, modes);
#endif
}

FILE* freopen(const std::string &path, const char *modes, FILE *stream)
{
  return os::freopen(path.c_str(), modes, stream);
}

FILE* freopen(const char *path, const char *modes, FILE *stream)
{
#ifdef _WIN32
  FILE* redirected_stream;
  freopen_s(&redirected_stream, redirect, "w+", stream);
  return redirected_stream;
#else
  return ::freopen(path, modes, stream);
#endif
}

bool remove(const std::string &name)
{
  return os::remove(name.c_str());
}

bool remove(const char *name)
{
  return ::remove(name) == 0;
}

bool access(const std::string &path, int mode)
{
  return os::access(path.c_str(), mode);
}

bool access(const char *path, int mode)
{
#ifdef _WIN32
  return _access(path, mode) == 0;
#else
  return ::access(path, mode) == 0;
#endif
}

int dup(FILE *stream)
{
#ifdef _WIN32
  return _dup(_fileno(stream));
#else
  return ::dup(fileno(stream));
#endif
}

bool is_readable(const std::string &path)
{
  return is_readable(path.c_str());
}

bool is_readable(const char *path)
{
  return os::access(path, 2);
}

bool is_writable(const std::string &path)
{
  return is_writable(path.c_str());
}

bool is_writable(const char *path)
{
  return os::access(path, 4);
}

bool exists(const std::string &path)
{
  return exists(path.c_str());
}

bool exists(const char *path)
{
  return os::access(path, 0);
}

size_t file_size(FILE *stream)
{
#ifdef _WIN32
  int fd = _fileno(stream);
  return ::_filelength(fd);
#else
  int fd = ::fileno(stream);
  struct stat buf{};
  ::fstat(fd, &buf);
  return buf.st_size;
#endif
}
}
}