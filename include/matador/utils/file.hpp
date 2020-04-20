#ifndef MATADOR_FILE_HPP
#define MATADOR_FILE_HPP

#include <string>

namespace matador {

class file
{
public:
  file() = default;
  file(const char *path, const char *mode);
  file(const std::string &path, const char *mode);
  file(const file&) = delete;
  file operator=(const file&) = delete;
  ~file();

  void open(const char *path, const char *mode);
  void open(const std::string &path, const char *mode);

  void close();

  size_t size() const;

  std::string path() const;

  FILE* stream() const;

private:
  std::string path_;
  FILE *stream_ = nullptr;
};
}
#endif //MATADOR_FILE_HPP
