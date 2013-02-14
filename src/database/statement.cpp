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

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include "database/statement_creator.hpp"
#include "database/database.hpp"

#include <functional>

using namespace std::placeholders;

namespace oos {

statement::~statement()
{}

void statement::prepare(const std::string &sql)
{  
  sql_ = statement::parse(sql, std::bind(&statement::on_result_field, this, _1, _2),
                          std::bind(&statement::on_host_field, this, _1, _2));
}

std::string statement::parse(const std::string &sql,
                             const param_func_t &result_func, const param_func_t &host_func,
                             char result_indicator, char host_indicator)
{
  // set start state
  state_t state = SQL_BEGIN;
  // statement to return
  std::string stmt;
  // found token
  std::string token;
  // result param count
  int result_index = 0;
  // host param count
  int host_index = 0;
  std::string::size_type len = sql.size();
  char end = '\0';
  for(std::string::size_type i = 0; i < len; ++i) {
    char c = sql[i];
    switch (state) {
      case SQL_BEGIN:
        if (c == result_indicator) {
          state = SQL_BEGIN_RESULT;
        } else if (c == host_indicator) {
          state = SQL_BEGIN_HOST;
        } else if (c == '\'' || c == '"') {
          state = SQL_STRING;
          stmt += c;
          end = c;
        } else {
          stmt += c;
        }
        break;
      case SQL_BEGIN_RESULT:
        if (std::isalpha(c)) {
          token = c;
          stmt += c;
          state = SQL_RESULT;
        } else {
          throw std::logic_error("first result character must be alpha");
        }
        break;
      case SQL_RESULT:
        if (std::isalnum(c) || c == '_') {
          token += c;
          stmt += c;
        } else if (c == ' ' || c == ',') {
          state = SQL_BEGIN;
          stmt += c;
          result_func(token, result_index++);
        } else {
          throw std::logic_error("invalid character");
        }
        break;
      case SQL_BEGIN_HOST:
        if (std::isalpha(c)) {
          token = c;
          state = SQL_HOST;
        } else {
          throw std::logic_error("first host character must be alpha");
        }
        break;
      case SQL_HOST:
        if (std::isalnum(c) || c == '_') {
          token += c;
        } else if (c == ' ' || c == ',' || c == ')') {
          state = SQL_BEGIN;
          stmt += '?';
          stmt += c;
          host_func(token, host_index++);
        } else {
          throw std::logic_error("invalid character");
        }
        break;
      case SQL_STRING:
        if (c == end) {
          state = SQL_BEGIN;
        }
        stmt += c;
        break;
      default:
        break;
    };
  }
  return stmt;
}

std::string statement::sql() const
{
  return sql_;
}

}
