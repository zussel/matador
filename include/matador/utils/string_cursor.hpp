#ifndef MATADOR_STRING_CURSOR_HPP
#define MATADOR_STRING_CURSOR_HPP

#include "matador/utils/export.hpp"

namespace matador {

/**
 * A simple cursor class to traverse forward through
 * a character array.
 */
class OOS_UTILS_API string_cursor
{
public:
  /**
   * Initializes the cursor with the given string
   *
   * @param str String to initialize the cursor with
   */
  explicit string_cursor(const char *str);

  /**
   * Assign a string to the cursor class
   *
   * @param str String to assign
   * @return The assigned cursor
   */
  string_cursor& operator=(const char *str);

  /**
   * Gets the character at position i
   *
   * @param i Requested character position
   * @return The requested character
   */
  char operator[](int i) const { return cursor_[i]; }

  /**
   * Checks if the current cursor is a nullptr
   *
   * @return True if current cursor position is nullptr
   */
  bool is_null() const { return cursor_ == nullptr; }

  /**
   * Returns the current cursor
   *
   * @return The current cursor
   */
  const char* operator()() const { return cursor_; }

  /**
   * Syncs the cursor with the new given cursor
   *
   * @param cursor New cursor
   */
  void sync_cursor(const char *cursor) { cursor_ = cursor; }

  /**
   * Skips whitespace for the cursor and
   * returns the first character not a whitespace
   *
   * @return First character not a whitespace
   */
  char skip_whitespace();

  /**
   * Increments cursor position and returns
   * this character.
   *
   * @return Next character in string
   */
  char next_char();

  /**
   * Returns the current character of the string
   *
   * @return The current character
   */
  char current_char() const;

private:
  const char *cursor_ = nullptr;
};

}

#endif //MATADOR_STRING_CURSOR_HPP
