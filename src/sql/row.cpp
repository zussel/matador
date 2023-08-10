#include "matador/sql/row.hpp"

namespace matador {

bool row::add_column(const std::string &column)
{
  return add_column(column, std::make_shared<null_value>());
}

bool row::add_column(const std::string &column, const std::shared_ptr<value> &value)
{
  if (has_column(column)) {
    return false;
  }

  columns_.push_back(column);
  return values_.insert({column, value}).second;
}

bool row::has_column(const std::string &column) const
{
  return values_.find(column) != values_.end();
}

void row::set(const std::string &column, const std::shared_ptr<value> &value)
{
  values_.at(column) = value;
}

void row::clear()
{
  columns_.clear();
  values_.clear();
}

}
