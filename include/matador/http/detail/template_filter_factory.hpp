#ifndef MATADOR_TEMPLATE_FILTER_FACTORY_HPP
#define MATADOR_TEMPLATE_FILTER_FACTORY_HPP

#ifdef _MSC_VER
#ifdef matador_http_EXPORTS
    #define OOS_HTTP_API __declspec(dllexport)
    #define EXPIMP_HTTP_TEMPLATE
  #else
    #define OOS_HTTP_API __declspec(dllimport)
    #define EXPIMP_HTTP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_HTTP_API
#endif

#include "matador/utils/singleton.hpp"
#include "matador/utils/factory.hpp"

#include "matador/http/detail/template_filter.hpp"

namespace matador {
namespace http {
namespace detail {

/// @cond MATADOR_DEV

class OOS_HTTP_API template_filter_factory : public singleton<template_filter_factory>
{
private:
  using factory_type = factory<std::string, template_filter>;

public:
  template_filter_factory();

  std::shared_ptr<template_filter> produce(const std::string &key) const;

private:
  factory_type factory_;
};

/// @endcond

}
}
}

#endif //MATADOR_TEMPLATE_FILTER_FACTORY_HPP
