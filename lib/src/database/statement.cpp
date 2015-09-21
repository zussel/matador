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

using namespace std::placeholders;

namespace oos {

statement::statement()
  : p(nullptr)
{ }

statement::statement(detail::statement_impl *impl)
  : p(impl)
{ }

statement::~statement()
{}

statement::statement(statement &&x)
  : p(x.p)
{
  x.p = nullptr;
}

statement &statement::operator=(statement &&x)
{
  p = x.p;
  x.p = nullptr;
  return *this;
}

int statement::bind(serializable *o)
{
  return p->bind(o);
}

void statement::clear()
{
  p->clear();
}

result statement::execute()
{
  return p->execute();
}

void statement::reset()
{
  p->reset();
}

std::string statement::str() const
{
  return p->str();
}

}
