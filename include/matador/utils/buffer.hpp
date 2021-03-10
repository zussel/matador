#ifndef MATADOR_BUFFER_HPP
#define MATADOR_BUFFER_HPP

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

#include <array>
#include <cstddef>
#include <string>

namespace matador {

/**
 * Simple buffer class with a fixed
 * size of 16384 bytes.
 */
class OOS_UTILS_API buffer
{
public:
  /**
   * Creates an empty buffer
   */
  buffer() = default;

  /**
   * Copy constructs from given buffer x
   *
   * @param x Buffer to copy from
   */
  buffer(const buffer &x) = default;

  /**
   * Copy assigns from given buffer x
   *
   * @param x Buffer to assign from
   * @return The assign buffer
   */
  buffer& operator=(const buffer &x) = default;

  /**
   * Destroys the buffer
   */
  ~buffer() = default;

  /**
   * Appends the given chunk of the given size
   * to the buffer. If the new buffer size exceeds
   * the maximum size of the buffer an out of bound
   * exception is thrown
   *
   * @param chunk Chunk to append
   * @param size Size of the chunk to append
   */
  void append(const char *chunk, std::size_t size);

  /**
   * Appends the given buffer to this buffer. If
   * the new buffer size exceeds the maximum size
   * of the buffer an out of bound exception is thrown
   *
   * @param buf Buffer to append
   */
  void append(const buffer &buf);

  /**
   * Appends the given string to this buffer. If
   * the new buffer size exceeds the maximum size
   * of the buffer an out of bound exception is thrown
   *
   * @param str String to append
   */
  void append(const std::string &str);

  /**
   * Pointer to the beginning of the buffer data
   *
   * @return Pointer to the beginning of the data
   */
  char* data();

  /**
   * Pointer to the beginning of the buffer data
   *
   * @return Pointer to the beginning of the data
   */
  const char* data() const;

  /**
   * Returns the capacity of the buffer i.e. 16384 bytes
   *
   * @return The capacity of the buffer
   */
  std::size_t capacity() const;

  /**
   * Returns the current size of buffer data.
   *
   * @return The current size of the buffer
   */
  std::size_t size() const;

  /**
   * Sets the new size of the buffer
   *
   * @param s Size of the buffer
   */
  void size(std::size_t s);

  /**
   * Returns true if the buffer is empty
   *
   * @return True if empty
   */
  bool empty() const;

  /**
   * Empties the buffer
   */
  void clear();

private:
  std::array<char, 16384> buf_{};
  std::size_t size_ = 0;
};

}

#endif //MATADOR_BUFFER_HPP
