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
#include "database/session.hpp"

namespace oos {

statement_impl::~statement_impl()
{}

void statement_impl::prepare(const std::string &sql)
{
  sql_ = sql;
}

std::string statement_impl::sql() const
{
  return sql_;
}

statement::statement(session &db)
  : impl_(0)
{
  impl_ = db.create_statement_impl();
}

statement::statement(session &db, const std::string &sql)
  : impl_(0)
  , sql_(sql)
{
  impl_ = db.create_statement_impl();
}

statement::~statement()
{}

result_ptr statement::execute()
{
  impl_->prepare(sql_);

  return result_ptr();
}

result_ptr statement::execute(const std::string &sql)
{
  sql_ = sql;
  return execute();
}

}
