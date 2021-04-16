#ifndef MATADOR_TEMPLATE_ENGINE_HPP
#define MATADOR_TEMPLATE_ENGINE_HPP

#include <string>
#include <stack>
#include <list>
#include <memory>

namespace matador {

class json;

namespace http {

namespace detail {

class template_command;
class template_part;

}

class template_engine
{
public:
  template_engine() = delete;

  static std::shared_ptr<detail::template_part> build(const std::string &format);
  static std::shared_ptr<detail::template_part> build(const char *format, size_t len);

  static std::string render(const std::shared_ptr<detail::template_part>& part, const matador::json &data);

  static std::string render(const std::string &format, const matador::json &data);
  static std::string render(const char *format, size_t len, const matador::json &data);
};

}
}
#endif //MATADOR_TEMPLATE_ENGINE_HPP
