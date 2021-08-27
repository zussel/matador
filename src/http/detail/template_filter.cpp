#include "matador/http/detail/template_filter.hpp"

#include "matador/utils/html.hpp"

#include <algorithm>

namespace matador {
namespace http {
namespace detail {
void template_filter::append(const std::shared_ptr<template_filter> &filter)
{
  if (next_) {
    next_->append(filter);
  } else {
    next_ = filter;
  }
}

json template_filter::apply(const json &data)
{
  if (next_) {
    return next_->apply(evaluate(data));
  } else {
    return evaluate(data);
  }
}

json escape_filter::evaluate(const json &data)
{
  return html::escape(data.as<std::string>());
}

json capfirst_filter::evaluate(const json &data)
{
  if (!data.is_string()) {
    return data;
  }
  auto val = data.as<std::string>();
  if (!val.empty()) {
    val[0] = toupper(val[0]);
    return val;
  } else {
    return data;
  }
}

json upper_filter::evaluate(const json &data)
{
  if (!data.is_string()) {
    return data;
  }
  auto val = data.as<std::string>();
  std::transform(val.begin(), val.end(), val.begin(), ::toupper);
  return val;
}

json lower_filter::evaluate(const json &data)
{
  if (!data.is_string()) {
    return data;
  }
  auto val = data.as<std::string>();
  std::transform(val.begin(), val.end(), val.begin(), ::tolower);
  return val;
}
}
}
}