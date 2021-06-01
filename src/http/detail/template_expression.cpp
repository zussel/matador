#include "matador/http/detail/template_expression.hpp"

#include "matador/utils/json.hpp"

#include <utility>

#include <iostream>

namespace matador {
namespace http {
namespace detail {

bool operator==(const json &a, const std::string &b)
{
  if (!a.is_string()) {
    return false;
  }
  return a.as<std::string>() == b;
}

bool operator!=(const json &a, const std::string &b)
{
  return !(a == b);
}

json_value_expression::json_value_expression(std::string value_name)
  : value_name_(std::move(value_name))
{}

bool json_value_expression::evaluate(const json &data) const
{
  try {
    const auto &val = data.at_path(value_name_, '.');

    if (val.is_array() || val.is_object() || val.is_string()) {
      return !val.empty();
    } else if (val.is_boolean()) {
      return val.as<bool>();
    } else if (val.is_integer()) {
      return val.as<long>() > 0;
    } else if (val.is_real()) {
      return val.as<double>() > 0;
    }
    return false;
  } catch (std::exception &ex) {
    return false;
  }
}

bool json_compare_expression::evaluate(const json &data) const
{
  const auto &value = data.at_path(value_name_, '.');

  return compare_func_(value);
}

bool json_json_compare_expression::evaluate(const json &data) const
{
  const auto &left = data.at_path(left_name_);
  const auto &right = data.at_path(right_name_);

  return compare_func_(left, right);
}

}
}
}
