#include "matador/http/detail/template_part.hpp"
#include "matador/http/detail/template_expression.hpp"

#include <utility>
#include <iostream>

#include "matador/utils/stream.hpp"
#include "matador/utils/json.hpp"

namespace matador {
namespace http {
namespace detail {

void template_part::append_filter(const template_filter_ptr &filter)
{
  if (filter_) {
    filter_->append(filter);
  } else {
    filter_ = filter;
  }
}

json template_part::apply_filter(const json &data)
{
  if (filter_) {
    return filter_->apply(data);
  }
  return data;
}

/*
 * static part implementation
 */
static_part::static_part(std::string str)
  : str_(std::move(str))
{}

std::string static_part::render(const json &)
{
  return str_;
}

/*
 * variable part implementation
 */
variable_part::variable_part(std::string str)
  : str_(std::move(str))
{}

std::string variable_part::render(const json &data)
{
  const auto &val = apply_filter(data.at_path(str_, '.'));

  if (val.is_integer()) {
    return std::to_string(val.as<int>());
  } else if (val.is_real()) {
    return std::to_string(val.as<double>());
  } else if (val.is_boolean()) {
    return std::to_string(val.as<bool>());
  } else if (val.is_string()) {
    return val.as<std::string>();
  } else if (val.is_null()) {
    return "null";
  } else {
    throw std::logic_error("couldn't cats object or array to string");
  }
}

/*
 * multi part implementation
 */
void multi_template_part::push_back(const template_part_ptr &part)
{
  parts_.push_back(part);
}

std::string multi_template_part::render(const json &data)
{
  return make_stream(parts_)
    .map([&data](const template_part_ptr &part) { return part->render(data); })
    .join();
}

std::list<template_part_ptr> &multi_template_part::parts()
{
  return parts_;
}

/*
 * multi part implementation
 */
loop_template_part::loop_template_part(template_part_ptr part, template_part_ptr on_empty_part, std::string list_name, std::string elem_name)
  : part_(std::move(part))
  , loop_part_(std::move(on_empty_part))
  , list_name_(std::move(list_name))
  , elem_name_(std::move(elem_name))
{
}

std::string loop_template_part::render(const json &data)
{
  const json &cont = data.at_path(list_name_, '.');

  if (!cont.is_object() && !cont.is_array()) {
    throw std::logic_error("json object isn't of type array or object");
  }

  if (cont.empty()) {
    return part_->render(data);
  } else {
    std::string result;
    for(const auto &elem : cont) {

      json item = json::object();
      item[elem_name_] = elem;
      item["forloop"]["parent"] = data;

      result.append(loop_part_->render(item));
    }
    return result;
  }
}

if_template_part::if_template_part(t_expression_list expression_list, template_part_ptr else_part)
  : expression_list_(std::move(expression_list))
  , else_part_(std::move(else_part))
{}

std::string if_template_part::render(const json &data)
{
  for (const auto &p : expression_list_) {
    if (p.first->evaluate(data)) {
      return p.second->render(data);
    }
  }
  return else_part_->render(data);
}

}
}
}