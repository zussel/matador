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

#include "tools/convert.hpp"

#include <cstring>

namespace oos {

void
convert(const varchar_base &from, std::string &to)
{
  to = from.str();
}

void
convert(const std::string &from, varchar_base &to)
{
  to = from;
}

void
convert(const std::string &from, std::string &to)
{
  to = from;
}

void
convert(const varchar_base &from, varchar_base &to)
{
  to = from;
}

void convert(const date &from, int &to)
{
  to = from.julian_date();
}

void convert(const date &from, long &to)
{
  to = from.julian_date();
}

void convert(const date &from, unsigned int &to)
{
  to = from.julian_date();
}

void convert(const date &from, unsigned long &to)
{
  to = from.julian_date();
}

void convert(const char *from, bool &to)
{
  char *ptr;
  to = strtoul(from, &ptr, 10) > 0;
  if (errno == ERANGE || (to == 0 && ptr == from)) {
    throw std::bad_cast();
  }
}

void convert(const bool &from, bool &to)
{
  to = from;
}

void convert(int from, date &to)
{
  to.set(from);
}

void convert(unsigned int from, date &to)
{
  to.set((int)from);
}

void convert(long from, date &to)
{
  to.set((int)from);
}

void convert(unsigned long from, date &to)
{
  to.set((int)from);
}

}
