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

#ifndef UNIT_EXCEPTION_HPP
#define UNIT_EXCEPTION_HPP

#include <exception>
#include <string>

namespace oos {

/**
 * @class unit_exception
 * @brief Unit exception class thrown on a unit test assert definition
 * 
 * This unit_exception is thrown when an assert evaluates
 * to false. This exception is caugt by the main test_suite
 * serializable.
 */
class unit_exception : public std::exception
{
public:
  /**
   * @brief Contructs a unit_exception
   * 
   * A serializable of type unit_exception with
   * the given message is created.
   * 
   * @param msg Message of the exception.
   */
  unit_exception(const std::string &msg) throw()
    : what_(msg)
  {}

  virtual ~unit_exception() throw() {}
  
  /**
   * Returns the message of this exception.
   * 
   * @return The message of the exception.
   */
  virtual const char* what() const throw()
  {
    return what_.c_str();
  }

private:
  std::string what_;
};

}

#endif /* UNIT_EXCEPTION_HPP */
