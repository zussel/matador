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

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

template < typename T >
class singleton
{
public:
  static T& instance ()
  {
    static T instance_;
    return instance_;
  }
  virtual ~singleton ()
  {
  }

protected:
  singleton() { }
};

//template < typename T > T* singleton<T>::instance_ = 0;

#endif /* SINGLETON_HPP */
