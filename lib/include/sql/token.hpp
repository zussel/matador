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
  drop() : token(basic_dialect::DROP) {}

  virtual std::string compile(basic_dialect &d, t_compile_type) override;
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

struct basic_value : public token
{
  basic_value(basic_dialect::t_token tok) : token(tok) { }
};

template < class T >
struct value : public basic_value
{
  value(const std::string &col, data_type_t t, T val)
    : basic_value(basic_dialect::VALUE)
    , column(col)
    , type(t)
    , val(val)
  { }

  virtual std::string compile(basic_dialect &d, token::t_compile_type type) override
  {
    return d.compile(*this);
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

struct where : public token
{
  where() : token(basic_dialect::WHERE) {}

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;
};

struct column : public token
{
  column(const std::string &col, data_type_t t, std::size_t idx, bool host);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override;

  std::string name;
  data_type_t type;
  std::size_t index;
  bool is_host;
};

struct identifier_column : public column
{
  identifier_column(const char *n, data_type_t t, size_t idx, bool host) : column(n, t, idx, host) { }

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    return name;
  }
};

struct varchar_column : public column
{
  varchar_column(const char *n, data_type_t t, size_t idx, bool host, size_t size)
    : column(n, t, idx, host)
    , size(size)
  { }

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    return column::compile(d, compile_type);
  }

  size_t size;
};

struct columns : public token
{
  columns() : token(basic_dialect::COLUMNS) {}

  void push_back(const std::string &col) { columns_.push_back(col); }

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    std::stringstream cols;
    if (columns_.size() > 1) {
      std::copy(columns_.begin(), columns_.end() - 1, std::ostream_iterator<std::string>(cols, ", "));
    }
    if (!columns_.empty()) {
      cols << columns_.back() << " ";
    }
    return cols.str();
  }
  std::vector<std::string> columns_;
};

struct basic_condition : public token
{
  basic_condition(basic_dialect::t_token tok) : token(tok) { }
};

template < class COND >
struct condition : public basic_condition
{
  condition(const COND &c)
    : basic_condition(basic_dialect::CLAUSE)
    , cond(c)
  {}

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) override
  {
    return cond.evaluate(compile_type == t_compile_type::PREPARED);
  }

  COND cond;
};

}

/// @endcond

}

#endif /* TOKEN_HPP */
