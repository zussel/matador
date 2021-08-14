#ifndef MATADOR_JSON_MAPPER_HPP
#define MATADOR_JSON_MAPPER_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include "matador/utils/basic_json_mapper.hpp"
#include "matador/utils/json_serializer.hpp"
#include "matador/utils/json_mapper_serializer.hpp"
#include "matador/utils/json.hpp"

#include <set>
#include <unordered_set>
#include <cstddef>


namespace matador {

/**
 * @class json_mapper
 *
 * conversions
 * string   <->    json    <->    object
 *
 * string to_string(json)
 * string to_string(object)
 * string to_string(array<object>)
 *
 * json to_json(object)
 * json to_json(array<object>)
 * json to_json(string)
 *
 * object to_object(json)
 * object to_object(string)
 * array<object> to_objects(json)
 * array<object> to_objects(string)
 */
class OOS_UTILS_API json_mapper
{
public:
  json_mapper() = default;

  std::string to_string(const json &js, const json_format &format = json_format::compact);
  template < class T >
  std::string to_string(const T &obj, const json_format &format = json_format::compact);
  template < class T >
  std::string to_string(const std::vector<T> &array, const json_format &format = json_format::compact);

  template < class T >
  json to_json(const T &obj);
  json to_json(const std::string &str);
  json to_json(const char *str);

  template < class T >
  T to_object(const json &js);
  template < class T >
  std::vector<T> to_objects(const json &js);

  template < class T >
  T to_object(const std::string &str);
  template < class T >
  T to_object(const char *str);
  template < class T >
  std::vector<T> to_objects(const std::string &str);
  template < class T >
  std::vector<T> to_objects(const char *str);

private:
  json_serializer json_serializer_;
  json_parser json_parser_;
};

template < class T >
std::string json_mapper::to_string(const T &obj, const json_format &format)
{
  return json_serializer_.to_json(obj, format);
}

template < class T >
std::string json_mapper::to_string(const std::vector<T> &array, const json_format &format)
{
  return json_serializer_.to_json_array(array, format);
}

template < class T >
json json_mapper::to_json(const T &obj)
{
  return to_json(to_string(obj));
}

template < class T >
T json_mapper::to_object(const json &js)
{
  return to_object<T>(js.str());
}

template < class T >
T json_mapper::to_object(const std::string &str)
{
  return to_object<T>(str.c_str());
}

template < class T >
T json_mapper::to_object(const char *str)
{
  basic_json_mapper<T, detail::json_mapper_serializer> mapper;
  return mapper.object_from_string(str);
}

template < class T >
std::vector<T> json_mapper::to_objects(const json &js)
{
  return to_objects<T>(js.str());
}

template < class T >
std::vector<T> json_mapper::to_objects(const std::string &str)
{
  return to_objects<T>(str.c_str());
}

template < class T >
std::vector<T> json_mapper::to_objects(const char *str)
{
  basic_json_mapper<T, detail::json_mapper_serializer> mapper;
  return mapper.array_from_string(str);
}

}
#endif //MATADOR_JSON_MAPPER_HPP