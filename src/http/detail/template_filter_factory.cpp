#include "matador/http/detail/template_filter_factory.hpp"

namespace matador {
namespace http {
namespace detail {

template_filter_factory::template_filter_factory()
{
  factory_.insert("escape", new factory_type::default_producer<escape_filter>());
}

std::shared_ptr<template_filter> template_filter_factory::produce(const std::string &key) const
{
  auto value = factory_.create(key);
  return std::shared_ptr<template_filter>(value);
}

}
}
}