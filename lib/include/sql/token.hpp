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

#include "sql/basic_dialect.hpp"
#include "sql/types.hpp"

#include <memory>
#include <sstream>
#include <vector>
#include <iterator>

namespace oos {

namespace detail {

/// @cond OOS_DEV
struct token
{
  enum t_compile_type {
    PREPARED,
    DIRECT
  };

  explicit token(basic_dialect::t_token tok);
  virtual ~token() {}

  virtual std::string compile(basic_dialect &d, t_compile_type) = 0;

  basic_dialect::t_token type;
};

struct select : public token
{
  select() : token(basic_dialect::SELECT) {}

  virtual std::string compile(basic_dialect &d, t_compile_type) override;
};

struct drop : public token
{
  drop(const std::string &t) : token(basic_dialect::DROP), table(t) {}

  virtual std::string compile(basic_dialect &d, t_compile_type) override;

  std::string table;
};

struct create : public token
{
  create(const std::string &t);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string table;
};

struct insert : public token
{
  insert(const std::string &t);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string table;
};

struct update : public token
{
  update(const std::string &t);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string table;
};

struct remove : public token
{
  remove(const std::string &t);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string table;
};

struct distinct : public token
{
  distinct() : token(basic_dialect::DISTINCT) {}

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;
};

struct set : public token
{
  set() : token(basic_dialect::SET) {}

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;
};

struct values : public token
{
  values() : token(basic_dialect::VALUES) {}

  void push_back(const std::shared_ptr<basic_value> &val) { values_.push_back(val); }

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    return d.compile(*this);
  }

  std::vector<std::shared_ptr<basic_value>> values_;
};

struct basic_value : public token
{
  basic_value(basic_dialect::t_token tok) : token(tok) { }

  virtual std::string compile(basic_dialect &d, token::t_compile_type type);

  virtual std::string str() const = 0;
};

template < class T, class Enabled = void >
struct value;

template < class T >
struct value<T, typename std::enable_if<std::is_scalar<T>::value>::type> : public basic_value
{
  value(T val)
    : basic_value(basic_dialect::VALUE)
//    , column(col)
//    , type(t)
    , val(val)
  { }

  std::string str() const
  {
    std::stringstream str;
    str << val;
    return str.str();
  }

  std::string column;
  data_type_t type;
  T val;
};

template < class T >
struct value<T, typename std::enable_if<
  std::is_same<std::string, T>::value ||
  std::is_same<const char*, T>::value>::type> : public basic_value
{
  value(T val)
    : basic_value(basic_dialect::VALUE)
//    , column(col)
//    , type(t)
    , val(val)
  { }

  std::string str() const
  {
    std::stringstream str;
    str << "'" << val << "'";
    return str.str();
  }

  std::string column;
  data_type_t type;
  T val;
};

struct asc : public token
{
  asc() : token(basic_dialect::ASC) {}

  virtual std::string compile(basic_dialect &d, token::t_compile_type type) override;
};

struct desc : public token
{
  desc() : token(basic_dialect::DESC) {}

  virtual std::string compile(basic_dialect &d, token::t_compile_type type) override;
};

struct from : public token
{
  from(const std::string &t);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string table;
};

struct limit : public token
{
  limit(size_t lmt);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  size_t limit_;
};

struct order_by : public token
{
  order_by(const std::string &col);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string column;
};

struct group_by : public token
{
  group_by(const std::string &col);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string column;
};

struct basic_where : public token
{
  basic_where() : token(basic_dialect::WHERE) {}

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;
};

template < class COND >
struct where : public basic_where
{
  where(const COND &c)
    : cond(c)
  {}

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    return cond.evaluate(compile_type == t_compile_type::PREPARED);
  }

  COND cond;
};

struct column : public token
{
  column(const std::string &col);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string name;

};

struct typed_column : public column
{
  typed_column(const std::string &col, data_type_t t, std::size_t idx, bool host);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  data_type_t type;
  std::size_t index;
  bool is_host;
};

struct identifier_column : public typed_column
{
  identifier_column(const char *n, data_type_t t, size_t idx, bool host) : typed_column(n, t, idx, host) { }

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    return d.compile(*this);
  }
};

struct varchar_column : public typed_column
{
  varchar_column(const char *n, size_t size, data_type_t t, size_t idx, bool host)
    : typed_column(n, t, idx, host)
    , size(size)
  { }

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    return d.compile(*this);
  }

  size_t size;
};

struct columns : public token
{
  enum t_brackets {
    WITH_BRACKETS,
    WITHOUT_BRACKETS
  };

  explicit columns(t_brackets with_brackets);

  void push_back(const std::shared_ptr<column> &col) { columns_.push_back(col); }

  bool with_brackets() const { return with_brackets_ == WITH_BRACKETS; }

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    return d.compile(*this);
  }
  std::vector<std::shared_ptr<column>> columns_;
  t_brackets with_brackets_;
};

}

/// @endcond

}

#endif /* TOKEN_HPP */
