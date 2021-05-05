#ifndef MATADOR_TEMPLATE_EXPRESSION_HPP
#define MATADOR_TEMPLATE_EXPRESSION_HPP

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

#include <string>
#include <functional>

namespace matador {

class json;

namespace http {
namespace detail {

class OOS_HTTP_API template_expression
{
public:
  virtual ~template_expression() = default;

  virtual bool evaluate(const json &data) const = 0;
};

OOS_HTTP_API bool operator==(const json &a, const std::string &b);
OOS_HTTP_API bool operator!=(const json &a, const std::string &b);

class OOS_HTTP_API json_value_expression : public template_expression
{
public:
  explicit json_value_expression(std::string value_name);

  bool evaluate(const json &data) const override;

private:
  std::string value_name_;
};

class OOS_HTTP_API json_compare_expression : public template_expression
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
