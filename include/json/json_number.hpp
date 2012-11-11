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

#include "json/json_type.hpp"
#include "json/json_value.hpp"
#include "json/json_string.hpp"

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

namespace oos {

/**
 * @class json_number
 * @brief Representing json number type
 * 
 */
class OOS_API json_number : public json_type
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

private:
  double value_;
};

}

#endif /* JSON_NUMBER_HPP */
