#include "matador/http/detail/template_state_factory.hpp"

namespace matador {
namespace http {
namespace detail {

template_state_factory::template_state_factory()
{
  factory_.insert("for", new factory_type::default_producer<foreach_state>());
}

std::shared_ptr<template_state> template_state_factory::produce(const std::string &key) const
{
  auto value = factory_.create(key);
  return std::shared_ptr<template_state>(value);
}

}
}
}