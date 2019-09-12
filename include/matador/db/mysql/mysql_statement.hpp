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

#ifndef MYSQL_STATEMENT_HPP
#define MYSQL_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"

#include "matador/db/mysql/mysql_result_info.hpp"
#include "matador/db/mysql/mysql_parameter_binder.hpp"

#ifdef _MSC_VER
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <string>
#include <vector>
#include <type_traits>

namespace matador {

class varchar_base;
class time;
class date;
class basic_identifier;

namespace mysql {

class mysql_connection;
class mysql_prepared_result;

class mysql_statement : public matador::detail::statement_impl
{
public:
  mysql_statement(mysql_connection &db, const matador::sql &stmt);
  ~mysql_statement() override;

  void clear() override;
  detail::result_impl* execute() override;
  void reset() override;

  void unlink_result(mysql_prepared_result *result);
  
protected:
  detail::parameter_binder_impl *binder() const override;

private:

//  void bind_null(std::size_t index);

private:
  MYSQL_STMT *stmt_ = nullptr;

  mysql_prepared_result *current_result = nullptr;

  std::unique_ptr<mysql_parameter_binder> binder_;
};

}

}

#endif /* MYSQL_STATEMENT_HPP */
