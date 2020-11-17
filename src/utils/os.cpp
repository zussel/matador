#include "matador/utils/os.hpp"

#include <sys/stat.h>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <algorithm>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
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
  fopen_s(&stream, path, "a");
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
  freopen_s(&redirected_stream, path, "w+", stream);
  return redirected_stream;
#else
  return ::freopen(path, modes, stream);
#endif
}

bool fclose(FILE *f)
{
  return ::fclose(f) == 0;
}

bool remove(const std::string &name)
{
  return os::remove(name.c_str());
}

bool remove(const char *name)
{
  return ::remove(name) == 0;
}

bool rename(const std::string &old_name, const std::string &new_name)
{
  return os::rename(old_name.c_str(), new_name.c_str());
}

bool rename(const char *old_name, const char *new_name)
{
  return ::rename(old_name, new_name) == 0;
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

bool mkdir(const std::string &dirname)
{
  return os::mkdir(dirname.c_str());
}

bool mkdir(const char *dirname)
{
#ifdef _WIN32
  return ::_mkdir(dirname) == 0;
#else
  return ::mkdir(dirname, S_IRWXU) == 0;
#endif
}

bool chdir(const std::string &dirname)
{
  return os::chdir(dirname.c_str());
}

bool chdir(const char *dirname)
{
#ifdef _WIN32
  return _chdir(dirname) == 0;
#else
  return ::chdir(dirname) == 0;
#endif
}

bool rmdir(const std::string &dirname)
{
  return os::rmdir(dirname.c_str());
}

bool rmdir(const char *dirname)
{
#ifdef _WIN32
  return _rmdir(dirname) == 0;
#else
  return ::rmdir(dirname) == 0;
#endif
}

std::string get_current_dir()
{
  char buffer[1024];
#ifdef _WIN32
  char *dir = _getcwd(buffer, 1024);
#else
  char *dir = ::getcwd(buffer, 1024);
#endif
  if (dir == nullptr) {
#ifdef _WIN32
    ::strerror_s(buffer, 1024, errno);
    throw std::logic_error(buffer);
#else
    throw std::logic_error(::strerror(errno));
#endif
  }
  return std::string(buffer);
}

bool mkpath(const std::string &path)
{
  return os::mkpath(path.c_str());
}

#ifdef _WIN32
char DIR_SEPARATOR = '\\';
const char* DIR_SEPARATOR_STRING = "\\";
#else
char DIR_SEPARATOR = '/';
const char* DIR_SEPARATOR_STRING = "/";
#endif

bool mkpath(const char *path)
{
  char tmp[256];
  char *p = nullptr;
  size_t len;

  snprintf(tmp, sizeof(tmp),"%s",path);
  len = strlen(tmp);
  if(tmp[len - 1] == DIR_SEPARATOR)
    tmp[len - 1] = 0;
  for(p = tmp + 1; *p; p++) {
    if (*p == DIR_SEPARATOR) {
      *p = 0;
      if (!os::mkdir(tmp)) {
        return false;
      }
      *p = DIR_SEPARATOR;
    }
  }
  return os::mkdir(tmp);
}

bool rmpath(const std::string &path)
{
  return os::rmpath(path.c_str());
}

bool rmpath(const char *path)
{
  // change next to last path segment

  std::vector<char> pathcopy(path, path+::strlen(path)+1);
  std::vector<std::string> segments;
#ifdef _WIN32
  char *next_token = nullptr;
  char *segment = ::strtok_s(pathcopy.data(), DIR_SEPARATOR_STRING, &next_token);
#else
  char *segment = ::strtok(pathcopy.data(), DIR_SEPARATOR_STRING);
#endif

  while (segment != nullptr) {
    os::chdir(segment);
    segments.emplace_back(segment);
#ifdef _WIN32
    segment = ::strtok_s(nullptr, DIR_SEPARATOR_STRING, &next_token);
#else
    segment = ::strtok(nullptr, DIR_SEPARATOR_STRING);
#endif
  }

  auto first = segments.rbegin();
  for (auto it=first; it!=segments.rend(); ++it) {
    os::chdir("..");
    os::rmdir(*it);
  }
  return true;
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

std::string build_path(const std::string &a, const std::string &b)
{
  return a + DIR_SEPARATOR_STRING + b;
}

}
}