#ifndef MATADOR_BUFFER_VIEW_HPP
#define MATADOR_BUFFER_VIEW_HPP

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

#include <cstring>
#include <string>
#include <array>

namespace matador {

/**
 * @brief Cursor based string buffer
 *
 * This class represents a character string
 * with a cursor representing the position
 * in the string.
 *
 * The class can be used when a string must
 * be processed and copying should be prevented
 */
class OOS_UTILS_API buffer_view
{
public:
  /**
   * Default constructor
   */
  buffer_view() = default;

  /**
   * Initialize a buffer_view with a character string
   *
   * @param start Start of the character string
   * @param size Length of the character string
   */
  buffer_view(const char *start, size_t size);

  /**
   * Initialize a buffer_view with a character string
   *
   * @param start Start of the character string
   */
  explicit buffer_view(const char *start);

  /**
   * Initialize a buffer_view with a string
   *
   * @param str String to initialize with
   */
  explicit buffer_view(const std::string &str);

  /**
   * Initialize a buffer_view with an array of characters
   *
   * @tparam S Size of the array
   * @param ar Array of characters
   */
  template<std::size_t S>
  explicit buffer_view(const std::array<char, S> &ar)
    : start_(ar.data())
    , cursor_(const_cast<char*>(ar.data()))
    , size_(S)
  {}

  /**
   * Move copy constructor
   *
   * @param x buffer_view to copy move from
   */
  buffer_view(buffer_view &&x) noexcept;

  /**
   * Copy constructor
   *
   * @param x buffer_view to copy from
   */
  buffer_view(const buffer_view &x) = default;

  /**
   * Move assign constructor
   *
   * @param x buffer_view to move assign from
   * @return The moved buffer_view
   */
  buffer_view& operator=(buffer_view &&x) noexcept;

  /**
   * Assign constructor
   *
   * @param x buffer_view to assign from
   * @return The assigned buffer_view
   */
  buffer_view& operator=(const buffer_view &x) = default;

  /**
   * Assign the given string to the buffer_view
   *
   * @param str String to assign
   * @return The assigned buffer_view
   */
  buffer_view& operator=(const std::string &str);

  /**
   * Bump the cursor len characters forward.
   * If the new position is after the end of the
   * view the position is set to the end
   *
   * @param len Number of characters to bump forward
   */
  void bump(size_t len);

  /**
   * Get the data of the string starting from the
   * current cursor position
   *
   * @return The string starting at the current position
   */
  const char* data() const;

  /**
   * Get the data of the string starting from the
   * current cursor position
   *
   * @return The string starting at the current position
   */
  char* data();

  /**
   * Returns the remaining size of the buffer_view
   *
   * @return The remaining size
   */
  size_t size() const;

  /**
   * Returns the total capacity of the buffer_view
   *
   * @return Returns the total capacity
   */
  size_t capacity() const;

  /**
   * Returns true if the buffer_view is empty
   * meaning that no characters were used at all
   * and the cursor points to the very beginning of
   * the string
   *
   * @return True if buffer_view is in initial state
   */
  bool empty() const;

  /**
   * Returns true if the buffer_view is full
   * meaning that all characters are used and the
   * cursor point to the very and of the internal
   * string
   *
   * @return True if the cursor points to the end of the string
   */
  bool full() const;

  /**
   * Resets the cursor to the initial
   * (first) position of the internal string
   */
  void clear();

private:
  const char *start_ = nullptr;
  char *cursor_ = nullptr;
  size_t size_ = 0;
};

}
#endif //MATADOR_BUFFER_VIEW_HPP
