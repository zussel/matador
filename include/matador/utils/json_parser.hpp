//
// Created by sascha on 22.12.19.
//

#ifndef REACTOR_JSON_PARSER_HPP
#define REACTOR_JSON_PARSER_HPP

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

#include "matador/utils/json.hpp"
#include "matador/utils/generic_json_parser.hpp"

#include <stack>

namespace matador {

/**
 * @class json_parser
 * @brief Parse a json formatted stream or string
 *
 * This class parse a given input stream or string
 * into a json serializable representation.
 * The result will be a oos::json_value serializable.
 */
class OOS_UTILS_API json_parser : public generic_json_parser<json_parser>
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
