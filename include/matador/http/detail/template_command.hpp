#ifndef MATADOR_TEMPLATE_COMMAND_HPP
#define MATADOR_TEMPLATE_COMMAND_HPP

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
#include "matador/utils/stream.hpp"

#include <string>
#include <utility>
#include <list>

namespace matador {

class string_cursor;
class json;

namespace http {
namespace detail {

/// @cond MATADOR_DEV

class template_part;
class multi_template_part;

class OOS_HTTP_API template_command
{
public:
  virtual ~template_command() = default;

  virtual std::shared_ptr<template_part> parse(string_cursor &cursor) = 0;
};

class OOS_HTTP_API foreach_command : public template_command
{
public:
  std::shared_ptr<template_part> parse(string_cursor &cursor) override;

private:
  std::shared_ptr<multi_template_part> loop_part_;
};

class OOS_HTTP_API if_else_command : public template_command
{
public:
  std::shared_ptr<template_part> parse(string_cursor &cursor) override;
};

class OOS_HTTP_API include_command : public template_command
{
public:
  std::shared_ptr<template_part> parse(string_cursor &cursor) override;
};

/// @endcond

}
}
}
#endif //MATADOR_TEMPLATE_COMMAND_HPP
