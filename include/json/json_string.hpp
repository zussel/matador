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

#ifndef JSON_STRING_HPP
#define JSON_STRING_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "json/json_value.hpp"

#include <string>
#include <iostream>

namespace oos {

class OOS_API json_string
{
public:
  json_string();
  json_string(const std::string &val);
  json_string(const json_value &x);
  json_string(const char *val);
  json_string(const json_string &x);
  json_string& operator=(const json_string &x);
  json_string& operator=(const json_value &x);
  json_string& operator=(const std::string &x);
  virtual ~json_string();
 
  bool operator<(const json_string &x) const;

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  std::string value() const;
  void value(const std::string &val);

  void push_back(char c);

  json_value& operator[](const std::string &key) { throw std::logic_error("json_string has no key access operator"); }
  json_value& operator[](size_t index) { throw std::logic_error("json_string has no index access operator"); }
  const json_value& operator[](size_t index) const { throw std::logic_error("json_string has no index access operator"); }
  void push_back(const json_value &x) { throw std::logic_error("json_string has no push_back method"); }

private:
  std::string value_;
};

}

#endif /* JSON_STRING_HPP */