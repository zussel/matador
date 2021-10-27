#include "matador/http/detail/template_filter_factory.hpp"

namespace matador {
namespace http {
namespace detail {

template_filter_factory::template_filter_factory()
{
  factory_.insert("escape", new factory_type::default_producer<escape_filter>());
  factory_.insert("capfirst", new factory_type::default_producer<capfirst_filter>());
  factory_.insert("upper", new factory_type::default_producer<upper_filter>());
  factory_.insert("lower", new factory_type::default_producer<lower_filter>());
}

std::shared_ptr<template_filter> template_filter_factory::produce(const std::string &key) const
{
  auto value = factory_.create(key);
  return std::shared_ptr<template_filter>(value);
}

}
}
}