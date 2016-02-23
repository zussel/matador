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

namespace oos {

namespace detail {

struct field
{
  field(const char *n, data_type_t t, std::size_t i, bool h)
    : name(n), type(t), index(i), is_host(h)
  {}
  std::string name;
  data_type_t type;
  std::size_t index;
  bool is_host;
};

typedef std::shared_ptr<field> field_ptr;

}
/// @cond OOS_DEV
struct token {
  virtual ~token() {}

  virtual std::string get(bool) const = 0;
};

struct string_token : public token {
  string_token() {}
  string_token(const std::string &s) : str(s) {}

  virtual std::string get(bool) const {
    return str;
  }

  std::string str;
};

struct result_field_token : public token {
  result_field_token(detail::field_ptr f) : fld(f) {}

  virtual std::string get(bool /*prepared*/) const {
    return fld->name;
  }

  detail::field_ptr fld;
};

template < class COND >
struct condition_token : public token
{
  condition_token(const COND &c)
    : cond(c)
  {}
  
  virtual std::string get(bool prepared) const {
    return cond.evaluate(prepared);
  }

  COND cond;
};

struct host_field_token : public string_token {
  host_field_token(detail::field_ptr f) : fld(f) {}
  host_field_token(detail::field_ptr f, const std::string &s)
    : string_token(s), fld(f)
  {}

  virtual std::string get(bool prepared) const {
    if (prepared) {
      return std::string("?");
    } else {
      return str;
    }
  }

  detail::field_ptr fld;
};

/// @endcond

}

#endif /* TOKEN_HPP */
