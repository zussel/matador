#include "matador/object/basic_has_many_to_many_item.hpp"

namespace matador {

basic_has_many_to_many_item::basic_has_many_to_many_item(const std::string &left_column, const std::string &right_column)
  : left_column_(left_column)
  , right_column_(right_column)
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