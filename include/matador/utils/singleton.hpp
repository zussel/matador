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

namespace matador {

/**
 * @class singleton
 * @brief Provides only one instance of a type.
 * @tparam T The type of the singleton.
 * 
 * This class implements the singleton pattern.
 * It ensures that only one instance of a class
 * exists.
 * @note The derived class must add a friend declaration
 * for the concrete singleton type.
 */
template < typename T >
class singleton
{
public:
  typedef T value_type; /**< Shortcut for the singletons type */

  /**
   * @brief Access the instance of the class.
   * 
   * The static instance method provides
   * the access to the one instance of the
   * concrete class.
   * 
   * @return The one instance of the class.
   */
  static value_type& instance ()
  {
    static value_type instance_;
    return instance_;
  }
  virtual ~singleton ()
  {
  }

protected:
  singleton() { }
};

}

//template < typename T > T* singleton<T>::instance_ = 0;

#endif /* SINGLETON_HPP */
