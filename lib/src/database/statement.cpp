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

#include "database/statement.hpp"

#include "object/serializable.hpp"

using namespace std::placeholders;

namespace oos {

statement::~statement()
{}

int statement::bind(serializable *o)
{
  reset();
  host_index = 0;
  o->serialize(*this);
  return host_index;
}

std::string statement::str() const
{
  return sql_;
}

void statement::str(const std::string &s)
{
  sql_ = s;
}

}
