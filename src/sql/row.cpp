#include "matador/sql/row.hpp"

namespace matador {

bool row::add_column(const std::string &column)
{
  return add_column(std::make_shared<matador::column>(column));
}

bool row::add_column(const column_ptr &col)
{
  if (has_column(col->name)) {
    return false;
  }

  columns_.push_back(col);
  return columns_by_name_.insert({col->name, col}).second;
}

bool row::has_column(const std::string &column) const
{
  return columns_by_name_.find(column) != columns_by_name_.end();
}

void row::set(const std::string &column, const std::shared_ptr<value> &value)
{
  columns_by_name_.at(column)->val.value_ = value->value_;
}

void row::clear()
{
  columns_.clear();
  columns_by_name_.clear();
}

}
