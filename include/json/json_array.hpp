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

#ifndef JSON_ARRAY_HPP
#define JSON_ARRAY_HPP

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

#include <vector>
#include <iostream>

namespace oos {

class OOS_API json_array : public json_value
{
public:
  typedef std::vector<json_value*> t_value_vector;
  typedef t_value_vector::const_iterator const_iterator;
  typedef t_value_vector::iterator iterator;
  typedef t_value_vector::size_type size_type;

public:
  json_array();
  virtual ~json_array();

  virtual bool parse(std::istream &in);
  virtual void print(std::ostream &out) const;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  size_type size() const;
  bool empty() const;

private:
  t_value_vector value_vector_;
};

}

#endif /* JSON_ARRAY_HPP */