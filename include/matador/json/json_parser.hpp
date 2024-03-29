#ifndef REACTOR_JSON_PARSER_HPP
#define REACTOR_JSON_PARSER_HPP

#include "matador/json/export.hpp"

#include "json.hpp"
#include "generic_json_parser.hpp"

#include <stack>

namespace matador {

/**
 * @class json_parser
 * @brief Parse a json formatted stream or string
 *
 * This class parse a given input stream or string
 * into a json serializable representation.
 * The result will be a matador::json_value serializable.
 */
class OOS_JSON_API json_parser : public generic_json_parser<json_parser>
{
public:
  /**
   * Creates a new json_parser instance
   */
  json_parser() = default;

  /**
   * @brief parse a const character string.
   *
   * Parses a const character string and
   * returns a json_value serializable representing
   * the json structure.
   *
   * @param str The json const character string.
   * @return A json_value structure.
   */
  json parse(const char *str);

  /**
   * @brief parse a std::string.
   *
   * Parses a std::string and
   * returns a json_value serializable representing
   * the json structure.
   *
   * @param str The json std::string.
   * @return A json_value structure.
   */
  json parse(const std::string &str);

  /// @cond OOS_DEV //
  void on_begin_object();
  void on_object_key(const std::string &key);
  void on_end_object();

  void on_begin_array();
  void on_end_array();

  void on_string(const std::string &value);
  void on_real(double value);
  void on_integer(long long value);
  void on_bool(bool value);
  void on_null();
  /// @endcond OOS_DEV //

private:
  void on_value(const json &value);

private:
  json value_;

  std::string key_;
  std::stack<json*> state_stack_;
};

}

#endif //REACTOR_JSON_PARSER_HPP
