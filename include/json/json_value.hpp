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

#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

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

#include <istream>

namespace oos {

class OOS_API json_value
{
public:
  virtual ~json_value() {}
  
  virtual bool parse(std::istream &in) = 0;
  virtual void print(std::ostream &out) const = 0;

  static json_value* create(std::istream &in);

  friend OOS_API std::istream& operator>>(std::istream &str, json_value &value);
  friend OOS_API std::ostream& operator<<(std::ostream &str, const json_value &value);
};

/*
class OOS_API json_value
{
public:
  json_value();
  json_value(const json_value &x);
  json_value(const json_object &x);
  json_value(const json_array &x);
  json_value(const json_number &x);
  json_value(const json_string &x);
  json_value(const json_bool &x);
  json_value(const json_null &x);
  ~json_value();
  
  json_value& operator[](const std::string &key);
  json_value& operator[](size_t index);

  template < class T >
  T get() const
  {
    
  }

  class json_type
  {
  };

private:
  json_type *type_;
};
*/
}

#endif /* JSON_VALUE_HPP */
