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

#include "sql/sql.hpp"
#include "sql/basic_dialect.hpp"

#include <iostream>

namespace oos {

sql::sql()
  : command_type_(t_query_command::UNKNOWN)
{

}

sql::~sql()
{
  reset(t_query_command::UNKNOWN);
}

void sql::append(const std::shared_ptr<detail::token> tokptr)
{
  token_list_.push_back(tokptr);
}

void sql::append(detail::token *tok)
{
  std::shared_ptr<detail::token> tokptr(tok);
  token_list_.push_back(tokptr);
}

void sql::append(const sql &stmt)
{
  append(new detail::query(stmt));
}

void sql::reset(t_query_command command_type)
{
  command_type_ = command_type;
  token_list_.clear();
}

unsigned int sql::type_size(data_type_t type)
{
  switch(type) {
    case type_char:
      return sizeof(char);
    case type_short:
      return sizeof(short);
    case type_int:
      return sizeof(int);
    case type_long:
      return sizeof(long);
    case type_unsigned_char:
      return sizeof(unsigned char);
    case type_unsigned_short:
      return sizeof(unsigned short);
    case type_unsigned_int:
      return sizeof(unsigned int);
    case type_unsigned_long:
      return sizeof(unsigned long);
    case type_bool:
      return sizeof(bool);
    case type_float:
      return sizeof(float);
    case type_double:
      return sizeof(double);
    case type_char_pointer:
    case type_varchar:
    case type_text:
      return 64;
    default:
      throw std::logic_error("unknown type");
  }
}

namespace detail {
query::query(const sql &s)
  : token(NONE), sql_(s)
{ }

void query::accept(token_visitor &visitor)
{
  visitor.visit(*this);
//  std::string result("(");
//  return result.append(sql_.compile(d)).append(")");
}

}
}
