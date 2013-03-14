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

#include "object/object_convert.hpp"

namespace oos {

void convert(const object_base_ptr &from, long &to)
{
  to = from.id();
}

void convert(const long &, object_base_ptr &)
{
  std::cout << "converting id to object_ptr\n";
}

void convert(const object_base_ptr &, char*, int)
{
  throw std::bad_cast();
}

void convert(const object_base_ptr &, char*, int, int)
{
  throw std::bad_cast();
}

/*
void
convert(const object_base_ptr &from, object_ptr<object> &to)
{
  std::cout << "6 " << from.ptr()->id() << "\n";
  to.reset(from.ptr());
}
*/
}
