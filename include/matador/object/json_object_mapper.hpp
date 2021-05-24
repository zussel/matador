#ifndef MATADOR_JSON_OBJECT_MAPPER_HPP
#define MATADOR_JSON_OBJECT_MAPPER_HPP

#ifdef _MSC_VER
#ifdef matador_object_EXPORTS
    #define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define MATADOR_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4275)
#else
#define MATADOR_OBJECT_API
#endif

#include "matador/object/json_object_serializer.hpp"
#include "matador/object/json_object_mapper_serializer.hpp"
#include "matador/object/object_json_serializer.hpp"

namespace matador {

/**
 * @class json_object_mapper
 *
 * conversions
 * string   <->    json    <->    object_ptr
 *
 * string to_string(object_ptr)
 * string to_string(object_view)
 *
 * json to_json(object_ptr)
 * json to_json(object_view)
 *
 * object_ptr to_object(json)
 * object_ptr to_object(string)
 * array<object_ptr> to_objects(json)
 * array<object_ptr> to_objects(string)
 */
class json_object_mapper
{
public:
  json_object_mapper() = default;

  template< typename T, object_holder_type OPT >
  std::string to_string(const object_pointer<T, OPT> &obj, const json_format &format = json_format::compact);
  template < class T >
  std::string to_string(const std::vector<T> &array, const json_format &format = json_format::compact);
  template < class T >
  std::string to_string(const object_view<T> &array, const json_format &format = json_format::compact);

  template< typename T, object_holder_type OPT >
  json to_json(const object_pointer<T, OPT> &obj);
  template < class T >
  json to_json(const object_view<T> &obj);

  template < class T >
  object_ptr<T> to_object(const json &js);
  template < class T >
  std::vector<object_ptr<T>> to_objects(const json &js);

  template < class T >
  object_ptr<T> to_object(const std::string &str);
  template < class T >
  object_ptr<T> to_object(const char *str);
  template < class T >
  std::vector<object_ptr<T>> to_objects(const std::string &str);
  template < class T >
  std::vector<object_ptr<T>> to_objects(const char *str);

private:
  json_object_serializer json_object_serializer_;
  object_json_serializer object_json_serializer_;
};

template<typename T, object_holder_type OPT>
std::string json_object_mapper::to_string(const object_pointer<T, OPT> &obj, const json_format &format)
{
  return json_object_serializer_.to_json(obj, format);
}

template<class T>
std::string json_object_mapper::to_string(const std::vector<T> &array, const json_format &format)
{
  return json_object_serializer_.to_json(array, format);
}

template<class T>
std::string json_object_mapper::to_string(const object_view<T> &array, const json_format &format)
{
  return json_object_serializer_.to_json(array, format);
}

template<typename T, object_holder_type OPT>
json json_object_mapper::to_json(const object_pointer<T, OPT> &obj)
{
  return object_json_serializer_.to_json(obj);
}

template<class T>
json json_object_mapper::to_json(const object_view<T> &objects)
{
  return object_json_serializer_.to_json(objects);
}

template<class T>
object_ptr<T> json_object_mapper::to_object(const json &js)
{
  return to_object<T>(js.str());
}

template<class T>
std::vector<object_ptr<T>> json_object_mapper::to_objects(const json &js)
{
  return to_objects<T>(js.str());
}

template<class T>
object_ptr<T> json_object_mapper::to_object(const std::string &str)
{
  return to_object<T>(str.c_str());
}

template<class T>
object_ptr<T> json_object_mapper::to_object(const char *str)
{
  basic_json_mapper<object_ptr<T>, detail::json_object_mapper_serializer> mapper;
  return mapper.object_from_string(str);
}

template<class T>
std::vector<object_ptr<T>> json_object_mapper::to_objects(const std::string &str)
{
  return to_objects<T>(str.c_str());
}

template<class T>
std::vector<object_ptr<T>> json_object_mapper::to_objects(const char *str)
{
  basic_json_mapper<object_ptr<T>, detail::json_object_mapper_serializer> mapper;
  return mapper.array_from_string(str);
}
}

#endif //MATADOR_JSON_OBJECT_MAPPER_HPP
