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

/**
 * File class representing file stream
 */
class OOS_UTILS_API file
{
public:
  /**
   * Creates an uninitialized file.
   */
  file() = default;

  /**
   * Creates and open a file stream.
   *
   * Internally fopen() is used so the modes
   * are the same:
   *
   * "r"	read: Open file for input operations. The file must exist.
   * "w"	write: Create an empty file for output operations. If a
   *      file with the same name already exists, its contents are discarded and
   *      the file is treated as a new empty file.
   * "a"	append: Open file for output at the end of a file. Output operations
   *      always write data at the end of the file, expanding it. Repositioning
   *      operations (fseek, fsetpos, rewind) are ignored. The file is created
   *      if it does not exist.
   * "r+"	read/update: Open a file for update (both for input and output).
   *      The file must exist.
   * "w+"	write/update: Create an empty file and open it for update (both for input
   *      and output). If a file with the same name already exists its contents
   *      are discarded and the file is treated as a new empty file.
   * "a+" append/update: Open a file for update (both for input and output) with
   *      all output operations writing data at the end of the file. Repositioning
   *      operations (fseek, fsetpos, rewind) affects the next input operations, but
   *      output operations move the position back to the end of file. The file is
   *      created if it does not exist.
   *
   * @param path Path of the file to create
   * @param mode
   */
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
