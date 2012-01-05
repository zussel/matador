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
#include "database/database.hpp"

namespace oos {

statement_impl::~statement_impl()
{}

statement::statement(database &db)
  : impl_(0)
{
  impl_ = db.create_statement_impl();
}

statement::~statement()
{}

result* statement::execute(const std::string &sql)
{
  return 0;
}

row* statement::step()
{
  return impl_->step();
}

}
