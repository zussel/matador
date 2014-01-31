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

#include "database/database_exception.hpp"

namespace oos {

database_exception::database_exception(const char *source, const char *what)
  : source_(source)
  , what_(what)
{
}

database_exception::~database_exception() throw()
{}

const char* database_exception::what() const throw()
{
  return what_.c_str();
}

const char* database_exception::source() const throw()
{
  return source_.c_str();
}

}
