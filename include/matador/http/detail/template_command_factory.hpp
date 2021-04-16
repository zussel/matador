#ifndef MATADOR_TEMPLATE_COMMAND_FACTORY_HPP
#define MATADOR_TEMPLATE_COMMAND_FACTORY_HPP

#include "matador/utils/singleton.hpp"
#include "matador/utils/factory.hpp"

#include "matador/http/detail/template_command.hpp"

namespace matador {
namespace http {
namespace detail {

class template_command_factory : public singleton<template_command_factory>
{
private:
  using factory_type = factory<std::string, template_command>;

public:
  template_command_factory();

  std::shared_ptr<template_command> produce(const std::string &key) const;

private:
  factory_type factory_;
};

}
}
}
#endif //MATADOR_TEMPLATE_COMMAND_FACTORY_HPP
