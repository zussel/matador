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

#include <iostream>
#include "sql/sql.hpp"
#include "sql/token.hpp"
//#include "sql/condition.hpp"

namespace oos {

sql::~sql()
{
  reset();
}

void sql::append(detail::token *tok)
{
  std::shared_ptr<detail::token> tokptr(tok);
  token_list_.push_back(tokptr);
}

//void sql::append(const condition &c)
//{
//  field_ptr f(new column(c.column().c_str(), c.type(), host_field_vector_.size(), true));
//
//  token_list_.push_back(new condition_token(c));
//  host_field_map_.insert(std::make_pair(c.column(), f));
//  host_field_vector_.push_back(f);
//}

std::string sql::prepare() const
{
  return generate(true);
}

std::string sql::direct() const
{
  return generate(false);
}

void sql::reset()
{
  token_list_.clear();
}

std::string sql::generate(bool prepared) const
{
  std::string str;
  token_list_t::const_iterator first = token_list_.begin();
  token_list_t::const_iterator last = token_list_.end();
  while (first != last) {
    first++;
//    const token *t = *first++;
//    str += t->get(prepared);
  }
  return str;
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

std::string sql::str(bool prepared) const
{
  return generate(prepared);
}

std::string sql::compile(basic_dialect &dialect)
{
  std::string result;
  for(auto tokptr : token_list_) {
    result.append(tokptr->compile(dialect));
  }
  return result;
}


}
