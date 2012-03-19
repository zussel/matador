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

#ifndef DATABASE_EXCEPTION_HPP
#define DATABASE_EXCEPTION_HPP

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

#include <exception>
#include <string>

namespace oos {

/**
 * @class object_exception
 * @brief An object exception class
 *
 * This kind of exception is thrown, when 
 * an object dealing error occurs.
 */
class OOS_API database_exception : public std::exception
{
public:
  /**
   * Creates a database_exception
   *
   * @param source The source of the exception.
   * @param what The message of the exception.
   */
  database_exception(const char *source, const char *what);

  virtual ~database_exception() throw();

  /**
   * Returns the message of the exception
   *
   * @return The message of this exception.
   */
  virtual const char* what() const throw();
  
  /**
   * Returns the source of the exception
   *
   * @return The source of this exception.
   */
  const char* source() const throw();

private:
  std::string source_;
  std::string what_;
};

}

#endif /* DATABASE_EXCEPTION_HPP */
