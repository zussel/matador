//
// Created by sascha on 3/2/16.
//

#ifndef OOS_COLUMN_HPP
#define OOS_COLUMN_HPP

#include "sql/token.hpp"
#include "sql/value.hpp"

namespace oos {

struct column : public detail::token
{
  column(const std::string &col);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) const override;

  std::string name;
};

namespace detail {

struct typed_column : public oos::column
{
  typed_column(const std::string &col, data_type_t t, std::size_t idx, bool host);

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) const override;

  data_type_t type;
  std::size_t index;
  bool is_host;
};

struct identifier_column : public typed_column
{
  identifier_column(const char *n, data_type_t t, size_t idx, bool host) : typed_column(n, t, idx, host) { }

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) const override
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

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) const override
  {
    return d.compile(*this);
  }

  size_t size;
};

template < class T >
struct value_column : public column
{

  value_column(const std::string &col, T val)
    : column(col)
    , value_(val)
  { }

  value<T> value_;
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

  virtual std::string compile(basic_dialect &d, t_compile_type compile_type) const override
  {
    return d.compile(*this);
  }
  std::vector<std::shared_ptr<column>> columns_;
  t_brackets with_brackets_;
};

}
}
#endif //OOS_COLUMN_HPP
