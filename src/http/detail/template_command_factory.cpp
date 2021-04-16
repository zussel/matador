#include "matador/http/detail/template_command_factory.hpp"

namespace matador {
namespace http {
namespace detail {

template_command_factory::template_command_factory()
{
  factory_.insert("for", new factory_type::default_producer<foreach_command>());
  factory_.insert("if", new factory_type::default_producer<if_else_command>());
  factory_.insert("include", new factory_type::default_producer<include_command>());
}

std::shared_ptr<template_command> template_command_factory::produce(const std::string &key) const
{
  auto value = factory_.create(key);
  return std::shared_ptr<template_command>(value);
}

}
}
}