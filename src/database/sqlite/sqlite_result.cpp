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

#include "database/sqlite/sqlite_result.hpp"

namespace oos {

namespace sqlite {

sqlite_result::~sqlite_result()
{}

sqlite_static_result::sqlite_static_result()
  : pos_(0)
{}

sqlite_static_result::~sqlite_static_result()
{}

bool sqlite_static_result::next()
{
  return ++pos_ > rows_.size();
}

row* sqlite_static_result::current() const
{
  return rows_.at(pos_);
}

void sqlite_static_result::push_back(row *r)
{
  rows_.push_back(r);
}

}

}
