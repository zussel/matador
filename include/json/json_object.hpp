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

#ifndef JSON_OBJECT_HPP
#define JSON_OBJECT_HPP

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

namespace oos {

class OOS_API json_object
{
public:
  typedef std::map<json_string, json_value> t_string_value_map;
  typedef t_string_value_map::const_iterator const_iterator;
  typedef t_string_value_map::iterator iterator;
  typedef t_string_value_map::size_type size_type;

public:
  json_object();
  json_object(const json_value &x);
  json_object& operator=(const json_value &x);
  virtual ~json_object();

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  void clear();
  bool empty() const;
  size_type size() const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  bool contains(const std::string &key);
  json_value& operator[](const std::string &key);
  void insert(const json_string &key, const json_value &value);

  json_value& operator[](size_t index) { throw std::logic_error("json_object has no index access operator"); }
  const json_value& operator[](size_t index) const { throw std::logic_error("json_object has no index access operator"); }
  void push_back(const json_value &x) { throw std::logic_error("json_object has no push_back method"); }

private:
  t_string_value_map string_value_map_;
};

}

#endif /* JSON_OBJECT_HPP */
