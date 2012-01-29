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

#include "database/database_impl.hpp"
#include "database/transaction.hpp"
#include "database/statement.hpp"

namespace oos {

database_impl::database_impl()
{}

database_impl::~database_impl()
{}

transaction_impl* database_impl::create_transaction(transaction &tr) const
{
  return new transaction_impl(tr);
}

bool database_impl::store_statement(const std::string &id, database_impl::statement_impl_ptr stmt)
{
  return true;
}

database_impl::statement_impl_ptr database_impl::statement(const std::string &id) const
{
  return statement_impl_ptr();
}

}
