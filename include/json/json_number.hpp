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

#ifndef JSON_NUMBER_HPP
#define JSON_NUMBER_HPP

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
#include "json/json_string.hpp"

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

namespace oos {

class OOS_API json_number
{
public:
  json_number();
  json_number(double val);
  json_number(const json_number &x);
  json_number(const json_value &x);
  json_number& operator=(const json_number &x);
  json_number& operator=(double val);
  json_number& operator=(const json_value &x);
  virtual ~json_number();

  bool operator<(const json_number &x) const;

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  double value() const;
  void value(double val);

  json_value& operator[](const std::string &) { throw std::logic_error("json_number has no key access operator"); }
  json_value& operator[](size_t ) { throw std::logic_error("json_number has no index access operator"); }
  const json_value& operator[](size_t ) const { throw std::logic_error("json_number has no index access operator"); }
  void push_back(const json_value &) { throw std::logic_error("json_number has no push_back method"); }

  friend OOS_API std::istream& operator>>(std::istream &str, json_number &value);
  friend OOS_API std::ostream& operator<<(std::ostream &str, const json_number &value);

private:
  double value_;
};

}

#endif /* JSON_NUMBER_HPP */
