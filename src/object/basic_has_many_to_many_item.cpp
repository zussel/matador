#include "matador/object/basic_has_many_to_many_item.hpp"

namespace matador {

basic_has_many_to_many_item::basic_has_many_to_many_item(object_proxy *left_proxy, object_proxy *right_proxy,
                                                         const std::string &left_column, const std::string &right_column)
  : left_column_(left_column)
  , right_column_(right_column)
  , left_proxy_(left_proxy)
  , right_proxy_(right_proxy)
{}

std::string basic_has_many_to_many_item::left_column() const
{
  return left_column_;
}

std::string basic_has_many_to_many_item::right_column() const
{
  return right_column_;
}

object_proxy *basic_has_many_to_many_item::left_proxy() const
{
  return left_proxy_;
}

object_proxy *basic_has_many_to_many_item::right_proxy() const
{
  return right_proxy_;
}

}