/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VALUE_HPP
#define VALUE_HPP

#include <typeinfo>

namespace oos {

template < class T >
class value;

class value_base
{
public:
  virtual ~value_base() {}

  template < class T >
  T get()
  {
    value<T> *v = dynamic_cast<value<T>* >(this);
    if (v) {
      return v->get<T>();
    } else {
      throw std::bad_cast();
    }
  }

private:
  template < class T >
  T operator()();
};

template < class T >
class value : public value_base
{
public:
  value(const T &val)
    : value_(val)
  {}
  virtual ~value() { /*delete value_;*/ }

  template < class V >
  V get()
  {
    return value_;
  }

private:
  T value_;
};

}

#endif /* VALUE_HPP */
