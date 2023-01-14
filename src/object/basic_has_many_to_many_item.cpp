#include <utility>

#include "matador/object/basic_has_many_to_many_item.hpp"

namespace matador {

basic_has_many_to_many_item::basic_has_many_to_many_item(std::string left_column, std::string right_column)
  : left_column_(std::move(left_column))
  , right_column_(std::move(right_column))
{}

std::string basic_has_many_to_many_item::left_column() const
{
  return left_column_;
}

std::string basic_has_many_to_many_item::right_column() const
{
  return right_column_;
}

}