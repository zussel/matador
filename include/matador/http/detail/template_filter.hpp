#ifndef MATADOR_TEMPLATE_FILTER_HPP
#define MATADOR_TEMPLATE_FILTER_HPP

#include "matador/utils/json.hpp"

#include <string>
#include <memory>

namespace matador {
namespace http {
namespace detail {

class template_filter
{
public:
  virtual ~template_filter() = default;

  virtual json evaluate(const json &data) = 0;

  json execute(const json &data);
  void append(const std::shared_ptr<template_filter> &filter);

private:
  std::shared_ptr<template_filter> next_;
};

class escape_filter : public template_filter
{
public:
  json evaluate(const json &data) override;
};

}
}
}
#endif //MATADOR_TEMPLATE_FILTER_HPP
