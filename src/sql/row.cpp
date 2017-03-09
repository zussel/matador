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

#include "matador/sql/row.hpp"

namespace matador {

row::row()
{}

row::~row()
{}

bool row::add_column(const std::string &column)
{
  return add_column(column, std::make_shared<null_value>());
}

bool row::add_column(const std::string &column, const std::shared_ptr<detail::basic_value> &value)
{
  if (has_column(column)) {
    return false;
  }

  columns_.push_back(column);
  return values_.insert({column, value}).second;
}

bool row::has_column(const std::string &column) const
{
  return values_.find(column) != values_.end();
}

void row::set(const std::string &column, const std::shared_ptr<detail::basic_value> &value)
{
  values_.at(column) = value;
}

void row::clear()
{
  columns_.clear();
  values_.clear();
}

}
