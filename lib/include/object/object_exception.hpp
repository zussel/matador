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

#ifndef OBJECT_EXCEPTION_HPP
#define OBJECT_EXCEPTION_HPP

#include <exception>
#include <string>
#include <sstream>

namespace oos {

class object;

/**
 * Throws an object exception
 * 
 * @param source The source of the exception
 * @param what Detailed information about the exception
 */
//void throw_error(const std::string &source, const std::string &what);

/**
 * @class object_exception
 * @brief An object exception class
 *
 * This kind of exception is thrown, when 
 * an object dealing error occurs.
 */
class object_exception : public std::exception
{
public:
  /**
   * Creates a object_exception
   *
   * @param what The message of the exception.
   */
  object_exception(const char *what)
    : what_(what)
  {
  }

  virtual ~object_exception() throw() {}

  /**
   * Returns the message of the exception
   *
   * @return The message of this exception.
   */
  virtual const char* what() const throw()
  {
    return what_.c_str();
  }
private:
  std::string what_;
  object *obj_;
};

#define throw_object_exception(message) \
  do { \
    std::stringstream msg; \
    msg << message; \
    throw object_exception(msg.str().c_str()); \
  } while(false);

}

#endif /* OBJECT_EXCEPTION_HPP */
