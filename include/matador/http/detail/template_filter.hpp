#ifndef MATADOR_TEMPLATE_FILTER_HPP
#define MATADOR_TEMPLATE_FILTER_HPP

#include "matador/http/export.hpp"

#include "matador/json/json.hpp"

#include <string>
#include <memory>

namespace matador {
namespace http {
namespace detail {

/// @cond MATADOR_DEV

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

class OOS_HTTP_API capfirst_filter : public template_filter
{
public:
  json evaluate(const json &data) override;
};

class OOS_HTTP_API upper_filter : public template_filter
{
public:
  json evaluate(const json &data) override;
};

class OOS_HTTP_API lower_filter : public template_filter
{
public:
  json evaluate(const json &data) override;
};

/// @endcond

}
}
}
#endif //MATADOR_TEMPLATE_FILTER_HPP
