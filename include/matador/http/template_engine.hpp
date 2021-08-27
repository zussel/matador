#ifndef MATADOR_TEMPLATE_ENGINE_HPP
#define MATADOR_TEMPLATE_ENGINE_HPP

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

#include "matador/utils/json_mapper.hpp"

#include "matador/object/json_object_mapper.hpp"

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

class OOS_HTTP_API template_engine
{
public:
  template_engine() = delete;

  static std::shared_ptr<detail::template_part> build(const std::string &format);
  static std::shared_ptr<detail::template_part> build(const char *format, size_t len);

  static std::string render(const std::shared_ptr<detail::template_part>& part, const matador::json &data);

  template < class T >
  static std::string render(const std::string &format, const T &data);

  template< typename T, object_holder_type OPT >
  static std::string render(const std::string &format, const object_pointer<T, OPT> &data);

  static std::string render(const std::string &format, const matador::json &data);
  static std::string render(const char *format, size_t len, const matador::json &data);
};

template<class T>
std::string template_engine::render(const std::string &format, const T &data)
{
  json_mapper mapper;
  return render(mapper.to_json(data));
}

template<typename T, object_holder_type OPT>
std::string template_engine::render(const std::string &format, const object_pointer<T, OPT> &data)
{
  json_object_mapper mapper;
  return render(mapper.to_json(data));
}

}
}
#endif //MATADOR_TEMPLATE_ENGINE_HPP
