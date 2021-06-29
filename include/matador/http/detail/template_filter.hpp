#ifndef MATADOR_TEMPLATE_FILTER_HPP
#define MATADOR_TEMPLATE_FILTER_HPP

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

#include "matador/utils/json.hpp"

#include <string>
#include <memory>

namespace matador {
namespace http {
namespace detail {

class OOS_HTTP_API template_filter
{
public:
  virtual ~template_filter() = default;

  json apply(const json &data);
  void append(const std::shared_ptr<template_filter> &filter);

protected:
  virtual json evaluate(const json &data) = 0;

private:
  std::shared_ptr<template_filter> next_;
};

class OOS_HTTP_API escape_filter : public template_filter
{
public:
  json evaluate(const json &data) override;
};

}
}
}
#endif //MATADOR_TEMPLATE_FILTER_HPP
