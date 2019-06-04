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

#include "matador/sql/sql.hpp"
#include "matador/sql/basic_dialect.hpp"

#include <iostream>

namespace matador {

sql::sql()
  : command_type_(t_query_command::UNKNOWN)
{ }

sql::~sql()
{
  reset(t_query_command::UNKNOWN);
}

void sql::append(const std::shared_ptr<detail::token> &tokptr)
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

std::string sql::command() const
{
  switch (command_type_) {
    case t_query_command::CREATE:
      return "create";
    case t_query_command::DROP:
      return "drop";
    case t_query_command::INSERT:
      return "insert";
    case t_query_command::UPDATE:
      return "update";
    case t_query_command::REMOVE:
      return "delete";
    case t_query_command::SELECT:
      return "select";
    case t_query_command::UNKNOWN:
    default:
      return "unknown";
  }
}

std::string sql::table_name() const
{
  return table_name_;
}

void sql::table_name(const std::string &tname)
{
  table_name_ = tname;
}

namespace detail {
query::query(const sql &s)
  : token(NONE), sql_(s)
{ }

void query::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

}
}
