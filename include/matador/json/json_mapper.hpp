#ifndef MATADOR_JSON_MAPPER_HPP
#define MATADOR_JSON_MAPPER_HPP

#include "matador/json/export.hpp"

#include "matador/json/basic_json_mapper.hpp"
#include "json_serializer.hpp"
#include "json_mapper_serializer.hpp"
#include "json.hpp"

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
class OOS_JSON_API json_mapper
{
public:
  /**
   * Default constructor
   */
  json_mapper() = default;

  /**
   * Converts a given json object into a string where
   * the format is defined by the given json format object.
   *
   * @param js Json object to be converted
   * @param format Format of the output string
   * @return The json string
   */
  std::string to_string(const json &js, const json_format &format = json_format::compact);

  /**
   * Converts the given object into a json string where
   * the format is defined by the given json format object.
   *
   * @tparam T Type of the object to convert
   * @param obj Object to convert
   * @param format Format of the output string
   * @return The json string
   */
  template < class T >
  std::string to_string(const T &obj, const json_format &format = json_format::compact);

  /**
   * Converts the given array of objects into a json string where
   * the format is defined by the given json format object.
   *
   * @tparam T Type of the objects to convert
   * @param array Array of objects to convert
   * @param format Format of the output string
   * @return The json string
   */
  template < class T >
  std::string to_string(const std::vector<T> &array, const json_format &format = json_format::compact);

  /**
   * Converts the given object into a json object.
   *
   * @tparam T Type of the object to convert
   * @param obj Object to convert
   * @return The json object
   */
  template < class T >
  json to_json(const T &obj);

  /**
   * Converts the given json string into a json object.
   *
   * @param str Json string to convert
   * @return The json object
   */
  json to_json(const std::string &str);

  /**
   * Converts the given json string into a json object.
   *
   * @param str Json string to convert
   * @return The json object
   */
  json to_json(const char *str);

  /**
   * Converts a given json object into the concrete object.
   *
   * @tparam T Type of the object to create
   * @param js Json object to convert
   * @return The object created from the json object
   */
  template < class T >
  T to_object(const json &js);

  /**
   * Converts a json object of type array into an
   * array concrete objects.
   *
   * @tparam T Type of the objects to create
   * @param js Json to convert
   * @return The array of created objects
   */
  template < class T >
  std::vector<T> to_objects(const json &js);

  /**
   * Convert a given json string into a concrete object
   *
   * @tparam T Type of the object to create
   * @param str Json string to convert
   * @return The created object
   */
  template < class T >
  T to_object(const std::string &str);

  /**
   * Convert a given json string into a concrete object
   *
   * @tparam T Type of the object to create
   * @param str Json string to convert
   * @return The created object
   */
  template < class T >
  T to_object(const char *str);

  /**
   * Converts a given json string representing
   * an array into a vector of objects.
   *
   * @tparam T Type of the objects to create
   * @param str Json array string to convert
   * @return The array of created objects
   */
  template < class T >
  std::vector<T> to_objects(const std::string &str);

  /**
   * Converts a given json string representing
   * an array into a vector of objects.
   *
   * @tparam T Type of the objects to create
   * @param str Json array string to convert
   * @return The array of created objects
   */
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