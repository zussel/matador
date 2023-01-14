#ifndef MATADOR_JSON_FORMAT_HPP
#define MATADOR_JSON_FORMAT_HPP

#include "matador/json/export.hpp"

namespace matador {

/**
 * The json_format class defines parameters
 * how the json is formatted when it is
 * written as a string.
 */
class OOS_JSON_API json_format
{
public:
  /**
   * Default constructor
   */
  json_format() = default;

  /**
   * Creates a json format with
   * enables line break if the given
   * parameter is set to true
   *
   * @param enable_line_break If true json string json has line breaks
   */
  explicit json_format(bool enable_line_break);

  /**
   * Creates a json format with given number
   * of indentation spaces
   *
   * @param indentation Number of indentation spaces
   */
  explicit json_format(unsigned indentation);

  /**
   * Creates a json format object with flags
   * for enabling line breaks and skip empty
   * json values. Furthermore the number of
   * indentation spaces is defined.
   *
   * @param enable_line_break If true json string json has line breaks
   * @param skip_empty If true empty json values are skipped
   * @param indentation Number of indentation spaces
   */
  json_format(bool enable_line_break, bool skip_empty, unsigned indentation);

  /**
   * True if line breaks are enabled.
   *
   * @return True if line breaks are enabled
   */
  bool show_line_break() const;

  /**
   * True if empty json should be skipped.
   * @return True if empty json should be skipped
   */
  bool skip_empty();

  /**
   * Return the number of indentation spaces.
   *
   * @return Return the number of indentation spaces
   */
  unsigned indentation() const;

  /**
   * Returns the indentation character.
   *
   * @return Returns the indentation character
   */
  char indentation_char() const;

  /**
   * A default json format object for compact
   * json formatting. Without spaces, linebreaks
   * and indentation.
   */
  static json_format compact;

  /**
   * A default json format object resulting in
   * a pretty readable json string.
   */
  static json_format pretty;

private:
  bool enable_line_break_ = true;
  bool skip_empty_ = true;
  unsigned indentation_ = 2;
  char indentation_char_ = ' ';
};

}

#endif //MATADOR_JSON_FORMAT_HPP
