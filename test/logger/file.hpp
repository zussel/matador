#ifndef MATADOR_FILE_HPP
#define MATADOR_FILE_HPP

#include <string>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

namespace filehelper {

bool validate_log_file_line(const std::string &filename, int line_index, const std::string &level, const std::string &src, const std::string &msg);

class file
{
public:
  enum access_type
  {
    EXISTS = 0,
    WRITABLE = 2,
    READABLE = 4,
    READABLE_WRITABLE = 6
  };

#ifdef _MSC_VER
  static bool access(const char *path, access_type mode)
  {
    return _access(path, mode) == 0;
  }
#else
  static bool access(const char *path, access_type mode)
  {
    return ::access(path, mode) == 0;
  }
#endif

  static bool is_readable(const char *path)
  {
    return file::access(path, READABLE);
  }

  static bool is_writable(const char *path)
  {
    return file::access(path, WRITABLE);
  }

  static bool exists(const char *path)
  {
    return file::access(path, EXISTS);
  }
};

class std_stream_switcher
{
public:
  explicit std_stream_switcher(FILE *str, const char* redirect)
    : stream(str)
  {
    fflush(stream);
    fgetpos(stream, &pos);
    fd = dup(fileno(stream));
    freopen(redirect, "w+", stream);
  }

  ~std_stream_switcher()
  {
    fflush(stream);
    dup2(fd, fileno(stream));
    close(fd);
    clearerr(stream);
    fsetpos(stream, &pos);
  }
private:
  FILE *stream = nullptr;
  int fd = 0;
  fpos_t pos = {};
};

}

#endif //MATADOR_FILE_HPP
