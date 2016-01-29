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

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "sql/sql.hpp"
#include "sql/condition.hpp"

namespace oos {

/// @cond OOS_DEV

struct string_token : public sql::token {
  string_token() {}
  string_token(const std::string &s) : str(s) {}

  virtual std::string get(bool) const {
    return str;
  }

  std::string str;
};

struct result_field_token : public sql::token {
  result_field_token(sql::field_ptr f) : fld(f) {}

  virtual std::string get(bool /*prepared*/) const {
    return fld->name;
  }

  sql::field_ptr fld;
};

struct condition_token : public sql::token
{
  condition_token(const condition &c)
    : cond(c)
  {}
  
  virtual std::string get(bool prepared) const {
    return cond.str(prepared);
  }

  condition cond;
};

struct host_field_token : public string_token {
  host_field_token(sql::field_ptr f) : fld(f) {}
  host_field_token(sql::field_ptr f, const std::string &s)
    : string_token(s), fld(f)
  {}

  virtual std::string get(bool prepared) const {
    if (prepared) {
      return std::string("?");
    } else {
      return str;
    }
  }

  sql::field_ptr fld;
};

/// @endcond

}

#endif /* TOKEN_HPP */
