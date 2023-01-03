#ifndef MATADOR_TEMPLATE_ENGINE_HPP
#define MATADOR_TEMPLATE_ENGINE_HPP

#include "matador/http/export.hpp"

#include "matador/json/json_mapper.hpp"

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

/**
 * This class provides static methods
 * to render data (json, structure or object pointer)
 * into a given format string
 */
class OOS_HTTP_API template_engine
{
public:
  template_engine() = delete;

  /**
   * Render the given format string with the given
   * data object.
   *
   * @tparam T Type of data object.
   * @param format Format string to render.
   * @param data Data object to be rendered in.
   * @return The rendered string
   */
  template < class T >
  static std::string render(const std::string &format, const T &data);

  /**
   * Render the format string with the given
   * object pointer.
   *
   * @tparam T Type of object pointer.
   * @tparam OPT Kind of object pointer.
   * @param format Format string to render.
   * @param data Data object to be rendered in.
   * @return The rendered string
   */
  template< typename T, object_holder_type OPT >
  static std::string render(const std::string &format, const object_pointer<T, OPT> &data);

  /**
   * Render the given format string with the given
   * json data object.
   *
   * @param format Format string to render.
   * @param data Json data object to be rendered in.
   * @return The rendered string
   */
  static std::string render(const std::string &format, const matador::json &data);

  /**
   * Render the given format string with the given
   * json data object.
   *
   * @param format Format string to render.
   * @param len Length of the format string.
   * @param data Json data object to be rendered in.
   * @return The rendered string
   */
  static std::string render(const char *format, size_t len, const matador::json &data);

private:
  static std::string render(const std::shared_ptr<detail::template_part>& part, const matador::json &data);

  static std::shared_ptr<detail::template_part> build(const std::string &format);
  static std::shared_ptr<detail::template_part> build(const char *format, size_t len);
};

template<class T>
std::string template_engine::render(const std::string &format, const T &data)
{
  json_mapper mapper;
  return render(format, mapper.to_json(data));
}

template<typename T, object_holder_type OPT>
std::string template_engine::render(const std::string &format, const object_pointer<T, OPT> &data)
{
  json_object_mapper mapper;
  return render(format, mapper.to_json(data));
}

}
}
#endif //MATADOR_TEMPLATE_ENGINE_HPP
