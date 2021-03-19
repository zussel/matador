#ifndef MATADOR_TEMPLATE_STATE_FACTORY_HPP
#define MATADOR_TEMPLATE_STATE_FACTORY_HPP

#include "matador/utils/singleton.hpp"
#include "matador/utils/factory.hpp"

#include "matador/http/detail/template_state.hpp"

namespace matador {
namespace http {
namespace detail {

class template_state_factory : public singleton<template_state_factory>
{
private:
  using factory_type = factory<std::string, template_state>;

public:
  template_state_factory();

  std::shared_ptr<template_state> produce(const std::string &key) const;

private:
  factory_type factory_;
};

}
}
}
#endif //MATADOR_TEMPLATE_STATE_FACTORY_HPP
