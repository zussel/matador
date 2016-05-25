//
// Created by sascha on 3/2/16.
//

#include "sql/column.hpp"

namespace oos {

column::column(const std::string &col)
  : token(basic_dialect::COLUMN)
  , name(col)
{}

std::string column::compile(basic_dialect &d) const
{
  return d.compile(*this);
}


columns::columns(std::initializer_list<column> cols, t_brackets with_brackets)
  : token(basic_dialect::COLUMNS)
  , with_brackets_(with_brackets)
{
  for (auto &&col : cols) {
    push_back(std::make_shared<column>(col));
  }
}

columns::columns(t_brackets with_brackets)
  : token(basic_dialect::COLUMNS)
  , with_brackets_(with_brackets)
{}

namespace detail {

typed_column::typed_column(const std::string &col, data_type_t t, std::size_t idx, bool host)
  : column(col)
  , type(t), index(idx), is_host(host)
{}

std::string typed_column::compile(basic_dialect &d) const
{
  return d.compile(*this);
}

}

void columns::push_back(const std::shared_ptr<column> &col) { columns_.push_back(col); }

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

std::string columns::compile(basic_dialect &d) const
{
  return d.compile(*this);
}

columns columns::all()
{
  return all_;
}

columns columns::all_ = columns({column("*")}, WITHOUT_BRACKETS);

}
