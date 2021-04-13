#ifndef MATADOR_TEMPLATE_EXPRESSION_HPP
#define MATADOR_TEMPLATE_EXPRESSION_HPP

#include <string>
#include <functional>

namespace matador {

class json;

namespace http {
namespace detail {

class template_expression
{
public:
  virtual ~template_expression() = default;

  virtual bool evaluate(const json &data) const = 0;
};

bool operator==(const json &a, const std::string &b);
bool operator!=(const json &a, const std::string &b);

class json_value_expression : public template_expression
{
public:
  explicit json_value_expression(std::string value_name);

  bool evaluate(const json &data) const override;

private:
  std::string value_name_;
};

class json_compare_expression : public template_expression
{
public:
  template < typename Function >
  json_compare_expression(std::string value_name, Function &&func)
    : value_name_(std::move(value_name))
    , compare_func_(func)
  {}

  bool evaluate(const json &data) const override;

private:
  std::string value_name_;
  std::function<bool(const json &value)> compare_func_;
};

}
}
}
#endif //MATADOR_TEMPLATE_EXPRESSION_HPP
