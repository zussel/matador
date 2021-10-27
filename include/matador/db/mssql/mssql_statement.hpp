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

#ifndef MSSQL_STATEMENT_HPP
#define MSSQL_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"
#include "matador/sql/sql.hpp"

#include "matador/db/mssql/mssql_exception.hpp"
#include "mssql_parameter_binder.hpp"

#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#include <string>
#include <vector>
#include <type_traits>

namespace matador {

namespace mssql {

class mssql_connection;

class mssql_statement : public detail::statement_impl
{
public:
  mssql_statement(mssql_connection &db, const matador::sql &stmt);
  ~mssql_statement() override;

  void clear() override;
  detail::result_impl* execute() override;
  void reset() override;
  
//  virtual int column_count() const;
//  virtual const char* column_name(int i) const;


protected:
  detail::parameter_binder_impl *binder() const override;

private:
  void create_statement();

private:
  enum { NUMERIC_LEN = 21 };

  bool bind_null_ = false;

  SQLHANDLE stmt_ = nullptr;
  SQLHANDLE db_ = nullptr;

  std::unique_ptr<mssql_parameter_binder> binder_;
};

}

}

#endif /* MSSQL_STATEMENT_HPP */
