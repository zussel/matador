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

#ifndef JSON_BOOL_HPP
#define JSON_BOOL_HPP

#ifdef _MSC_VER
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

#include <stdexcept>

namespace oos {

/**
 * @class json_bool
 * @brief Class representing the json bool type.
 * 
 * This class represents the json bool type.
 */
class OOS_API json_bool : public json_type
{
public:
  json_bool(void);
  /**
   * Create a new json_bool value and
   * initialize it with the given value.
   * 
   * @param val The value of json_bool.
   */
  json_bool(bool val);

  /**
   * Creates a new json_bool and initializes
   * the value with the given json_value.
   * 
   * @param x The json_value to set.
   */
  json_bool(const json_value &x);

  virtual ~json_bool(void);

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  /**
   * Return the current value.
   * 
   * @return The current value.
   */
  bool value() const;
  
  /**
   * Set a new bool value.
   * 
   * @param val The new bool value.
   */
  void value(bool val);

private:
  static const char *true_string;
  static const char *false_string;

  bool value_;
};

}

#endif /* JSON_BOOL_HPP */
