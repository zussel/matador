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
  /**
   * Create a new json_number value and
   * initialize it with the given value.
   * 
   * @param val The value of json_bool.
   */
  json_number(double val);
  
  /**
   * Copy from given json_number.
   * 
   * @param x The json_number to copy from.
   */
  json_number(const json_number &x);
  
  /**
   * Copy from given json_value.
   * 
   * @param x The json_value to copy from.
   */
  json_number(const json_value &x);

  /**
   * Assign from given json_number.
   * 
   * @param x The json_number to assign from.
   * @return Initialized json_number.
   */
  json_number& operator=(const json_number &x);

  /**
   * Assign from given number.
   * 
   * @param val The number to assign from.
   * @return Initialized json_number.
   */
  json_number& operator=(double val);

  /**
   * Assign from given json_value.
   * 
   * @param x The json_value to assign from.
   * @return Initialized json_number.
   */
  json_number& operator=(const json_value &x);
  virtual ~json_number();

  /**
   * Less operator for json_number.
   * 
   * @param x The json_number to compare with.
   * @return True if this is less than x.
   */
  bool operator<(const json_number &x) const;

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  /**
   * Return the current value.
   * 
   * @return The current value.
   */
  double value() const;
  
  /**
   * Set a new json_number value.
   * 
   * @param val The value to set.
   */
  void value(double val);

private:
  double value_;
};

}

#endif /* JSON_NUMBER_HPP */
