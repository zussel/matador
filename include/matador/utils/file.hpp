#ifndef MATADOR_FILE_HPP
#define MATADOR_FILE_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
#define OOS_UTILS_API __declspec(dllexport)
#define EXPIMP_UTILS_TEMPLATE
#else
#define OOS_UTILS_API __declspec(dllimport)
#define EXPIMP_UTILS_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include <string>

namespace matador {

class OOS_UTILS_API file
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