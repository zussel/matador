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

#ifndef JSON_TYPE_HPP
#define JSON_TYPE_HPP

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

#include "json/json_parser.hpp"

#include <string>

namespace oos {

class json_value;

class OOS_API json_type
{
protected:
  explicit json_type(const std::string &name);

public:
  virtual ~json_type();

  virtual bool parse(std::istream &in) = 0;
  virtual void print(std::ostream &out) const = 0;

  virtual json_value& operator[](const std::string &key);
  virtual json_value& operator[](size_t index);
  virtual const json_value& operator[](size_t index) const;
  virtual void push_back(const json_value &x);

  std::string type() const;

  /*
  friend OOS_API std::istream& operator>>(std::istream &str, json_type &value);
  */
  friend OOS_API std::ostream& operator<<(std::ostream &str, const json_type &value);

protected:
  void type(const std::string &type);

protected:
  static json_parser parser;

private:
  std::string type_;
};

}

#endif /* JSON_TYPE_HPP */
