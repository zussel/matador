#include <utility>

#include "matador/sql/column.hpp"
#include "matador/sql/token_visitor.hpp"

namespace matador {

column::column(std::string col)
  : token(COLUMN)
  , name(std::move(col))
{}

column::column(std::string col, bool skip_quotes)
  : token(COLUMN)
  , name(std::move(col))
  , skip_quotes(skip_quotes)
{}

void column::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

columns::columns(const std::initializer_list<std::string> &colnames, t_brackets with_brackets)
  : token(COLUMNS)
  , with_brackets_(with_brackets)
{
  for (auto &&colname : colnames) {
    push_back(std::make_shared<column>(colname));
  }
}

columns::columns(std::initializer_list<column> cols, t_brackets with_brackets)
  : token(COLUMNS)
  , with_brackets_(with_brackets)
{
  for (auto &&col : cols) {
    push_back(std::make_shared<column>(col));
  }
}

columns::columns(std::initializer_list<std::shared_ptr<column>> cols, t_brackets with_brackets)
  : token(COLUMNS)
  , with_brackets_(with_brackets)
{
  columns_.assign(cols);
}

columns::columns(t_brackets with_brackets)
  : token(COLUMNS)
  , with_brackets_(with_brackets)
{}

columns::columns(const columns &x)
  : token(x.type)
  , with_brackets_(x.with_brackets_)
{
  columns_.assign(x.columns_.begin(), x.columns_.end());
}

columns &columns::operator=(const columns &x)
{
  type = x.type;
  with_brackets_ = x.with_brackets_;
  columns_.clear();
  columns_.assign(x.columns_.begin(), x.columns_.end());
  return *this;
}

std::shared_ptr<detail::typed_column> make_typed_varchar_column(const std::string &col, size_t size)
{
  return std::make_shared<detail::typed_varchar_column>(col, size, data_type::type_varchar);
}

namespace detail {

typed_column::typed_column(const std::string &col, data_type t)
  : typed_column(col, t, 0, false)
{}

typed_column::typed_column(const std::string &col, data_type t, std::size_t idx, bool host)
  : column(col)
  , type(t), index(idx), is_host(host)
{}

void typed_column::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

std::shared_ptr<value_column> make_value_column(const std::string &col, const std::string &val, long size)
{
  return std::make_shared<value_column>(col, new value(val, size));
}

std::shared_ptr<value_column> make_value_column(const std::string &col, const char *val, long size)
{
  return std::make_shared<value_column>(col, new value(val, size));
}

std::shared_ptr<value_column> make_value_column(const std::string &col, char *val, long size)
{
  return std::make_shared<value_column>(col, new value(val, size));
}
}

void columns::push_back(const std::shared_ptr<column> &col)
{
  columns_.push_back(col);
}

columns &columns::with_brackets()
{
  with_brackets_ = WITH_BRACKETS;
  return *this;
}

columns &columns::without_brackets()
{
  with_brackets_ = WITHOUT_BRACKETS;
  return *this;
}

void columns::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

columns columns::all()
{
  return all_;
}

column columns::count_all()
{
  return count_all_;
}

columns columns::all_ = columns({column("*", true)}, WITHOUT_BRACKETS);
column columns::count_all_ = column("COUNT(*)", true);

column operator "" _col(const char *name, size_t len)
{
    return column(std::string(name, len));
}

}
