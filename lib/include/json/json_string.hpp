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

#include "json/json_value.hpp"
#include "json/json_type.hpp"

#include <string>
#include <stdexcept>

namespace oos {

/**
 * @class json_string
 * @brief Class representing the json string type.
 * 
 * This class represents the json string type.
 */
class OOS_API json_string : public json_type
{
public:
  json_string();
  /**
   * Creates a new json_string and initializes
   * the value with the given string.
   * 
   * @param val The string to set.
   */
  json_string(const std::string &val);
  
  /**
   * Creates a new json_string and initializes
   * the value with the given json_value.
   * 
   * @param x The json_value to set.
   */
  json_string(const json_value &x);

  /**
   * Creates a new json_string and initializes
   * the value with the given string.
   * 
   * @param val The string to set.
   */
  json_string(const char *val);

  /**
   * Creates a new json_string and initializes
   * the value from the given json_string.
   * 
   * @param x The json_string to set.
   */
  json_string(const json_string &x);
  
  /**
   * Assign from a json_string
   * 
   * @param x The json_string to assign from.
   * @return This.
   */
  json_string& operator=(const json_string &x);

  /**
   * Assign from a json_value
   * 
   * @param x The json_value to assign from.
   * @return This.
   */
  json_string& operator=(const json_value &x);

  /**
   * Assign from a std::string
   * 
   * @param x The std::string to assign from.
   * @return This.
   */
  json_string& operator=(const std::string &x);
  virtual ~json_string();
 
  /**
   * Less operator for json_string. Internaly
   * the less operator of std::string is used.
   * 
   * @param x The json_string to compare with.
   * @return True if this is less than x.
   */
  bool operator<(const json_string &x) const;

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  /**
   * Return the current value.
   * 
   * @return The current value.
   */
  std::string value() const;
  
  /**
   * Set a new value.
   * 
   * @param val The new value to set.
   */
  void value(const std::string &val);

  /**
   * Append a new to the current json_string
   *
   * @param c The character to append.
   */
  void push_back(char c);

private:
  std::string value_;
};

}

#endif /* JSON_STRING_HPP */
