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

#ifndef SQLITE_STATEMENT_HPP
#define SQLITE_STATEMENT_HPP

#include "matador/db/sqlite/sqlite_parameter_binder.hpp"

#include "matador/sql/statement_impl.hpp"

#include <string>
#include <vector>
#include <memory>

struct sqlite3_stmt;

namespace matador {

namespace sqlite {

class sqlite_connection;

class sqlite_statement : public matador::detail::statement_impl
{
public:
  sqlite_statement(sqlite_connection &db, const matador::sql &sql);
  ~sqlite_statement() override;

  void clear() override;
  detail::result_impl* execute() override;
  void reset() override;

protected:

  detail::parameter_binder_impl *binder() const override;

private:
  sqlite_connection &db_;
  sqlite3_stmt *stmt_;

  std::unique_ptr<sqlite_parameter_binder> binder_;
};

}

}

#endif /* SQLITE_STATEMENT_HPP */
