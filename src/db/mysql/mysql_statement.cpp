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

#include "matador/db/mysql/mysql_statement.hpp"
#include "matador/db/mysql/mysql_connection.hpp"
#include "matador/db/mysql/mysql_exception.hpp"
#include "matador/db/mysql/mysql_prepared_result.hpp"
#include "matador/db/mysql/mysql_constants.hpp"

#include "matador/sql/sql.hpp"

#include <matador/utils/memory.hpp>

namespace matador {

namespace mysql {

mysql_statement::mysql_statement(mysql_connection &db, const matador::sql &stmt)
  : statement_impl(db.dialect(), stmt)
  , stmt_(mysql_stmt_init(db.handle()))
{
  if (mysql_stmt_prepare(stmt_, str().c_str(), str().size()) != 0) {
    throw_stmt_error(stmt_, "mysql", str());
  }

  binder_ = matador::make_unique<mysql_parameter_binder>(columns().size(), bind_vars().size());
}

mysql_statement::~mysql_statement()
{
  clear();
  mysql_stmt_close(stmt_);
}

void mysql_statement::reset()
{
  mysql_stmt_reset(stmt_);
}

void mysql_statement::unlink_result(mysql_prepared_result *result)
{
  if (result == current_result) {
    current_result = nullptr;
  }
}

void mysql_statement::clear()
{
  binder_->reset();
  if (current_result != nullptr) {
    current_result->free();
  } else {
    mysql_stmt_free_result(stmt_);
  }
}

detail::result_impl* mysql_statement::execute()
{
  if (current_result != nullptr) {
    current_result->free();
  }
  if (!binder_->host_array().empty()) {
    if (mysql_stmt_bind_param(stmt_, binder_->host_array().data()) != 0) {
      throw_stmt_error(stmt_, "mysql", str());
    }
  }

  if (mysql_stmt_execute(stmt_) != 0) {
    throw_stmt_error(stmt_, "mysql", str());
  }
  if (mysql_stmt_store_result(stmt_) != 0) {
    throw_stmt_error(stmt_, "mysql", str());
  }
  current_result = new mysql_prepared_result(this, stmt_, binder_->bindings(), binder_->result_infos());
  return current_result;
}

//void mysql_statement::bind_null(std::size_t index)
//{
//  MYSQL_BIND &bind = host_array[index];
//  is_null_vector[index] = true;
//  bind.is_null = &is_null_vector[index];
//}

detail::parameter_binder_impl *mysql_statement::binder() const
{
  return binder_.get();
}

}

}
