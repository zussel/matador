#ifndef MATADOR_TEMPLATE_PART_HPP
#define MATADOR_TEMPLATE_PART_HPP

#include <string>
#include <memory>
#include <list>

namespace matador {

class json;

namespace http {
namespace detail {

class template_expression;

class template_part
{
public:
  virtual ~template_part() = default;

  virtual std::string render(const json &data) = 0;
};

using template_part_ptr = std::shared_ptr<template_part>;

class static_part : public template_part
{
public:
  explicit static_part(std::string str);

  std::string render(const json &) override;

private:
  std::string str_;
};

class variable_part : public template_part
{
public:
  explicit variable_part(std::string str);

  std::string render(const json &data) override;

private:
  std::string str_;
};

class multi_template_part : public template_part
{
public:
  void push_back(const template_part_ptr &part);

  std::string render(const json &data) override;

  std::list<template_part_ptr>& parts();

private:
  std::list<template_part_ptr> parts_;
};

class loop_template_part : public template_part
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

class if_template_part : public template_part
{
public:
  using t_expression_list = std::list<std::pair<std::shared_ptr<template_expression>, template_part_ptr>>;
  if_template_part(t_expression_list expression_list, template_part_ptr else_part);

  std::string render(const json &data) override;

private:
  t_expression_list expression_list_;

  template_part_ptr else_part_;
};

}
}
}

#endif //MATADOR_TEMPLATE_PART_HPP
