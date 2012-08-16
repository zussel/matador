/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4355)
#else
  #define OOS_API
#endif

#include "json/generic_json_parser.hpp"
#include "json/json_value.hpp"

#include <iostream>
#include <string>
#include <stack>

namespace oos {

class OOS_API json_parser : public generic_json_parser<json_parser>
{
public:
  json_parser();
  virtual ~json_parser();

  json_value parse(std::istream &in);
  json_value parse(const char *str);
  json_value parse(std::string &str);

  void on_begin_object();
  void on_object_key(const std::string &key);
  void on_end_object();

  void on_begin_array();
  void on_end_array();

  void on_string(const std::string &value);
  void on_number(double value);
  void on_bool(bool value);
  void on_null();

private:
  json_value value_;

  std::string key_;
  /*
   * the boolean value indicates wether the
   * value is an object (true) or an array
   * (false)
   */
  typedef std::pair<bool, json_value> state_value_pair_t;
  std::stack<state_value_pair_t> state_stack_;
};

}

#endif /* JSON_PARSER_HPP */
