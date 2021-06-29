#include "matador/http/detail/template_filter.hpp"

#include "matador/utils/html.hpp"

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

}
}
}