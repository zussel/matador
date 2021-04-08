#ifndef MATADOR_TEMPLATE_STATE_HPP
#define MATADOR_TEMPLATE_STATE_HPP

#include "matador/utils/json.hpp"

#include <string>
#include <utility>
#include <list>

namespace matador {

class string_cursor;
class json;

namespace http {
namespace detail {

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
  explicit static_part(std::string str)
    : str_(std::move(str))
  {}

  std::string render(const json &) override
  {
    return str_;
  }

private:
  std::string str_;
};

class variable_part : public template_part
{
public:
  explicit variable_part(std::string str)
    : str_(std::move(str))
  {}

  std::string render(const json &data) override
  {
    return data[str_].as<std::string>();
  }

private:
  std::string str_;
};

class multi_template_part : public template_part
{
public:
  std::string render(const json &data) override
  {
    // Todo: provide a steam method to concat strings
    // auto str = make_stream(parts).join();
    std::string result;
    for (const auto &part : parts_) {
      result += part->render(data);
    }
    return result;
  }

private:
  std::list<template_part_ptr> parts_;
};


class template_state
{
public:
  virtual ~template_state() = default;

  virtual void configure(string_cursor &cursor) = 0;
  virtual bool is_executable() const = 0;
  virtual bool is_end_tag(const std::string &tag) const = 0;
  virtual void execute(const json &data) = 0;
  virtual void append(const std::string &part) = 0;
  virtual void append(char c) = 0;
  virtual void on_variable(const std::string &variable, const json &data) = 0;
  virtual std::string str() const = 0;
};

class global_state : public template_state
{
public:
  void execute(const json &data) override;

  void configure(string_cursor &cursor) override;

  bool is_executable() const override;

  bool is_end_tag(const std::string &tag) const override;

  void append(const std::string &part) override;

  void append(char c) override;

  void on_variable(const std::string &variable, const json &data) override;

  std::string str() const override;

private:
  std::string rendered_;
};

class foreach_state : public template_state
{
public:
  void configure(string_cursor &cursor) override;

  bool is_executable() const override;

  bool is_end_tag(const std::string &tag) const override;

  void execute(const json &data) override;

  void append(const std::string &part) override;

  void append(char c) override;

  void on_variable(const std::string &variable, const json &) override;

  std::string str() const override;

private:
  std::string elem_name_;
  std::string list_name_;
  std::string repeatable_;
  std::string content_;
};

}
}
}
#endif //MATADOR_TEMPLATE_STATE_HPP
