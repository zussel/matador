#ifndef MATADOR_TEMPLATE_PART_HPP
#define MATADOR_TEMPLATE_PART_HPP

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

#include "matador/http/detail/template_filter.hpp"

#include <string>
#include <memory>
#include <list>

namespace matador {

class json;

namespace http {
namespace detail {

/// @cond MATADOR_DEV

class template_expression;

using template_filter_ptr = std::shared_ptr<template_filter>;

class OOS_HTTP_API template_part
{
public:
  virtual ~template_part() = default;

  virtual std::string render(const json &data) = 0;

  void append_filter(const template_filter_ptr &filter);

protected:
  json apply_filter(const json &data);

private:
  template_filter_ptr filter_;
};

using template_part_ptr = std::shared_ptr<template_part>;

class OOS_HTTP_API static_part : public template_part
{
public:
  explicit static_part(std::string str);

  std::string render(const json &) override;

private:
  std::string str_;
};

class OOS_HTTP_API variable_part : public template_part
{
public:
  explicit variable_part(std::string str);

  std::string render(const json &data) override;

private:
  std::string str_;
};

class OOS_HTTP_API multi_template_part : public template_part
{
public:
  void push_back(const template_part_ptr &part);

  std::string render(const json &data) override;

  std::list<template_part_ptr>& parts();

private:
  std::list<template_part_ptr> parts_;
};

class OOS_HTTP_API loop_template_part : public template_part
{
public:
  loop_template_part(template_part_ptr part, template_part_ptr on_empty_part, std::string list_name, std::string elem_name);

  std::string render(const json &data) override;

private:
  template_part_ptr part_;
  template_part_ptr loop_part_;
  std::string list_name_;
  std::string elem_name_;
};

class OOS_HTTP_API if_template_part : public template_part
{
public:
  using t_expression_list = std::list<std::pair<std::shared_ptr<template_expression>, template_part_ptr>>;
  if_template_part(t_expression_list expression_list, template_part_ptr else_part);

  std::string render(const json &data) override;

private:
  t_expression_list expression_list_;

  template_part_ptr else_part_;
};

/// @endcond

}
}
}

#endif //MATADOR_TEMPLATE_PART_HPP
