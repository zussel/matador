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

#include "matador/utils/string.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/identifiable_holder.hpp"
#include "matador/utils/identifier.hpp"

#include "matador/sql/sql.hpp"

#include <cstring>

namespace matador {

namespace mysql {

mysql_statement::mysql_statement(mysql_connection &db, const matador::sql &stmt)
  : statement_impl(db.dialect(), stmt)
  , result_size(0)
  , host_size(0)
  , stmt_(mysql_stmt_init(db.handle()))
{
  // parse sql to create result and host arrays
  result_size = columns().size();
  host_size = bind_vars().size();

  bind_.resize(result_size);
  info_.resize(result_size);

  if (host_size) {
    host_array.resize(host_size);
    is_null_vector.assign(host_size, false);
  }

  int res = mysql_stmt_prepare(stmt_, str().c_str(), str().size());

  throw_stmt_error(res, stmt_, "mysql", str());
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
  while (!host_array.empty()) {
    if (host_array.back().buffer != nullptr) {
      delete [] static_cast<char*>(host_array.back().buffer);
    }
    host_array.pop_back();
  }

  result_size = 0;
  host_size = 0;
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
  if (host_size > 0) {
    int res = mysql_stmt_bind_param(stmt_, &host_array.front());
    throw_stmt_error(res, stmt_, "mysql", str());
  }

  int res = mysql_stmt_execute(stmt_);
  throw_stmt_error(res, stmt_, "mysql", str());
  res = mysql_stmt_store_result(stmt_);
  throw_stmt_error(res, stmt_, "mysql", str());
  current_result = new mysql_prepared_result(this, stmt_, bind_, info_);
  return current_result;
}

void mysql_statement::bind_value(std::size_t index, enum_field_types, char x)
{
  MYSQL_BIND &bind = host_array[index];
  if (bind.buffer == nullptr) {
    size_t s = sizeof(char);
    bind.buffer = new char[s];
    bind.buffer_length = (unsigned long)s;
  }
  *static_cast<char*>(bind.buffer) = x;
  bind.buffer_type = MYSQL_TYPE_VAR_STRING;
  is_null_vector[index] = false;
  bind.is_null = &is_null_vector[index];
}

void mysql_statement::bind_value(std::size_t index, enum_field_types, unsigned char x)
{
  MYSQL_BIND &bind = host_array[index];
  if (bind.buffer == nullptr) {
    size_t s = sizeof(unsigned char);
    bind.buffer = new unsigned char[s];
    bind.buffer_length = (unsigned long)s;
  }
  *static_cast<unsigned char*>(bind.buffer) = x;
  bind.buffer_type = MYSQL_TYPE_VAR_STRING;
  is_null_vector[index] = false;
  bind.is_null = &is_null_vector[index];
}

void mysql_statement::bind_null(std::size_t index)
{
  MYSQL_BIND &bind = host_array[index];
  is_null_vector[index] = true;
  bind.is_null = &is_null_vector[index];
}

detail::parameter_binder_impl *mysql_statement::binder() const
{
  return nullptr;
}

}

}
