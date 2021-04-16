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
 *
 * The open methods uses internally fopen() thus
 * the open modes are the same:
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
   * @param path Path of the file to create
   * @param mode The file mode to open
   */
  file(const char *path, const char *mode);

  /**
   * Creates and open a file stream.
   *
   * @param path Path of the file to create
   * @param mode The file mode to open
   */
  file(const std::string &path, const char *mode);
  file(const file&) = delete;
  file operator=(const file&) = delete;
  ~file();

  /**
   * Opens a file stream with the given path.
   * If the file is already open it is closed
   *
   * @param path Path of the file to create
   * @param mode The file mode to open
   */
  void open(const char *path, const char *mode);

  /**
   * Opens a file stream with the given path.
   * If the file is already open it is closed
   *
   * @param path Path of the file to create
   * @param mode The file mode to open
   */
  void open(const std::string &path, const char *mode);

  /**
   * Closes the file stream if it is open
   */
  void close();

  /**
   * Returns the size of the file
   *
   * @return The size of the file
   */
  size_t size() const;

  /**
   * Returns the path to the file
   *
   * @return The path to the file
   */
  std::string path() const;

  /**
   * Returns the internal file stream pointer
   *
   * @return The internal file stream pointer
   */
  FILE* stream() const;

  /**
   * Returns true if file is open.
   *
   * @return True if file is open
   */
  bool is_open() const;

private:
  std::string path_;
  FILE *stream_ = nullptr;
};

OOS_UTILS_API std::string read_as_text(const file &f);

}
#endif //MATADOR_FILE_HPP
