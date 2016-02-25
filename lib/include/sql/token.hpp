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

#include <memory>
#include <sstream>
#include <vector>
#include <iterator>
#include <mpif-sizeof.h>

namespace oos {

namespace detail {

/// @cond OOS_DEV
struct token {
  enum t_compile_type {
    PREPARED,
    DIRECT
  };

  virtual ~token() {}

  explicit token(dialect::t_token tok) : type(tok) {}

  virtual std::string compile(dialect *d, t_compile_type compile_type) {
    std::stringstream part;
    part << d->token(type) << " ";
    return part.str();
  }

  dialect::t_token type;;
};

struct select : public token
{
  select() : token(dialect::SELECT) {}
};

struct create : public token
{
  create(const std::string &t) : token(dialect::CREATE_TABLE), table(t) {}

  virtual std::string compile(dialect *d, t_compile_type compile_type) override
  {
    std::string result = token::compile(d, compile_type);
    result += " " + table + " ";
    return result;
  }
  std::string table;
};

struct update : public token
{
  update() : token(dialect::UPDATE) {}
};

struct distinct : public token
{
  distinct() : token(dialect::DISTINCT) {}
};

struct from : public token
{
  from(const std::string &t) : token(dialect::FROM), table(t) {}

  virtual std::string compile(dialect *d, t_compile_type compile_type) override
  {
    std::string result = token::compile(d, compile_type);
    result += table + " ";
    return result;
  }
  std::string table;
};

struct where : public token
{
  where() : token(dialect::WHERE) {}
};

struct column : public token
{
  column(const char *n, data_type_t t, std::size_t idx, bool host)
    : token(dialect::COLUMN)
    , name(n), type(t), index(idx), is_host(host)
  {}

  virtual std::string compile(dialect *d, t_compile_type compile_type) override
  {
    return name;
  }

  std::string name;
  data_type_t type;
  std::size_t index;
  bool is_host;
};

struct identifier_column : public column
{
  identifier_column(const char *n, data_type_t t, size_t idx, bool host) : column(n, t, idx, host) { }

  virtual std::string compile(dialect *d, t_compile_type compile_type) override
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

  virtual std::string compile(dialect *d, t_compile_type compile_type) override
  {
    return column::compile(d, compile_type);
  }

  size_t size;
};
struct columns : public token
{
  columns() : token(dialect::COLUMNS) {}

  void push_back(const std::string &col) { columns_.push_back(col); }

  virtual std::string compile(dialect *d, t_compile_type compile_type) override
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

template < class COND >
struct condition_token : public token
{
  condition_token(const COND &c)
    : token(dialect::CLAUSE)
    , cond(c)
  {}

  virtual std::string compile(dialect *d, t_compile_type compile_type) override
  {
    return cond.evaluate(compile_type == t_compile_type::PREPARED);
  }

  COND cond;
};

}

/// @endcond

}

#endif /* TOKEN_HPP */
