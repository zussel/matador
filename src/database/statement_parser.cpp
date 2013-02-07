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

#include "database/statement_parser.hpp"

namespace oos {

statement_parser::statement_parser(const param_func_t &result_func, const param_func_t &host_func,
                                   char result_indicator, char host_indicator)
  : state_(SQL_BEGIN)
  , result_indicator_(result_indicator)
  , host_indicator_(host_indicator)
  , result_func_(result_func)
  , host_func_(host_func)
{
}

std::string statement_parser::parse(const std::string &sql)
{
  std::string stmt;
  std::string token;
  int result_index = 0;
  int host_index = 0;
  std::string::size_type len = sql.size();
  char end = '\0';
  for(std::string::size_type i = 0; i < len; ++i) {
    char c = sql[i];
    switch (state_) {
      case SQL_BEGIN:
        if (c == result_indicator_) {
          state_ = SQL_BEGIN_RESULT;
        } else if (c == host_indicator_) {
          state_ = SQL_BEGIN_HOST;
        } else if (c == '\'' || c == '"') {
          state_ = SQL_STRING;
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
          state_ = SQL_RESULT;
        } else {
          throw std::logic_error("first result character must be alpha");
        }
        break;
      case SQL_RESULT:
        if (std::isalnum(c) || c == '_') {
          token += c;
          stmt += c;
        } else if (c == ' ' || c == ',') {
          state_ = SQL_BEGIN;
          stmt += c;
          result_func_(token, result_index++);
        } else {
          throw std::logic_error("invalid character");
        }
        break;
      case SQL_BEGIN_HOST:
        if (std::isalpha(c)) {
          token = c;
          state_ = SQL_HOST;
        } else {
          throw std::logic_error("first host character must be alpha");
        }
        break;
      case SQL_HOST:
        if (std::isalnum(c) || c == '_') {
          token += c;
        } else if (c == ' ' || c == ',' || c == ')') {
          state_ = SQL_BEGIN;
          stmt += '?';
          stmt += c;
          host_func_(token, host_index++);
        } else {
          throw std::logic_error("invalid character");
        }
        break;
      case SQL_STRING:
        if (c == end) {
          state_ = SQL_BEGIN;
        }
        stmt += c;
        break;
      default:
        break;
    };
  }
  return stmt;
}

}
