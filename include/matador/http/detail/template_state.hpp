#ifndef MATADOR_TEMPLATE_STATE_HPP
#define MATADOR_TEMPLATE_STATE_HPP

#include "matador/utils/json.hpp"
#include "matador/utils/stream.hpp"

#include <string>
#include <utility>
#include <list>

namespace matador {

class string_cursor;
class json;

namespace http {
namespace detail {

class template_part;
class multi_template_part;

class template_state
{
public:
  virtual ~template_state() = default;

  virtual std::shared_ptr<template_part> parse(string_cursor &cursor) = 0;
};

class foreach_state : public template_state
{
public:
  std::shared_ptr<template_part> parse(string_cursor &cursor) override;

private:
  std::shared_ptr<multi_template_part> loop_part_;
};

class if_else_state : public template_state
{
public:
  std::shared_ptr<template_part> parse(string_cursor &cursor) override;
};
}
}
}
#endif //MATADOR_TEMPLATE_STATE_HPP
