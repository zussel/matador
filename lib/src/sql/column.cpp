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

namespace detail {

columns::columns(t_brackets with_brackets)
  : token(basic_dialect::COLUMNS)
  , with_brackets_(with_brackets)
{}

typed_column::typed_column(const std::string &col, data_type_t t, std::size_t idx, bool host)
  : column(col)
  , type(t), index(idx), is_host(host)
{}

std::string typed_column::compile(basic_dialect &d) const
{
  return d.compile(*this);
}

}
}
