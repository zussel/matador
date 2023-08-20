#ifndef MATADOR_JSON_OBJECT_MAPPER_HPP
#define MATADOR_JSON_OBJECT_MAPPER_HPP

#include "matador/object/export.hpp"

#include "matador/object/json_object_serializer.hpp"
#include "matador/object/json_object_mapper_serializer.hpp"
#include "matador/object/object_json_serializer.hpp"

namespace matador {

/**
 * @class json_object_mapper
 *
 * conversions
 * string   <->    json    <->    unique_ptr
 *
 * string to_string(object_ptr)
 * string to_string(object_view)
 *
 * json to_json_string(object_ptr)
 * json to_json_string(object_view)
 *
 * unique_ptr to_object(json)
 * unique_ptr to_object(string)
 * array<shared_ptr> to_objects(json)
 * array<shared_ptr> to_objects(string)
 */
class json_object_mapper
{
public:
  /**
   * Default constructor
   */
  json_object_mapper() = default;

  /**
   * Converts an object_ptr, has_one or belongs_to object
   * into a json string with the given json format
   *
   * @tparam T Type of the object_ptr
   * @param obj Object to convert
   * @param format Json format object
   * @return The json formatted string
   */
  template< typename T >
  std::string to_string(const object_ptr<T> &obj, const json_format &format = json_format::compact);

  /**
   * Converts an array of objects
   * into a json string with the given json format
   *
   * @tparam T Type of objects
   * @param array Array to convert
   * @param format Json format object
   * @return The json formatted string
   */
  template < class T >
  std::string to_string(const std::vector<T> &array, const json_format &format = json_format::compact);

  /**
   * Converts an object_view
   * into a json string with the given json format
   *
   * @tparam T Type of objects
   * @param array object_view to convert
   * @param format Json format object
   * @return The json formatted string
   */
  template < class T >
  std::string to_string(const object_view<T> &array, const json_format &format = json_format::compact);

  /**
   * Convert an object_ptr, has_one or belongs_to object
   * into an json object
   *
   * @tparam T Type of the object_ptr
   * @param obj Object to convert
   * @return The json object
   */
  template< typename T >
  json to_json(const object_ptr<T> &obj);

  /**
   * Convert the elements of on object_view
   * into a json object
   *
   * @tparam T Type of objects
   * @param objects object_view to convert
   * @return The json object
   */
  template < class T >
  json to_json(const object_view<T> &objects);

  /**
   * Converts the json object into an
   * object of a specific type
   *
   * @tparam T Type of the object to create
   * @param js json object to convert
   * @return The newly created object as unique_ptr
   */
  template < class T >
  std::unique_ptr<T> to_object(const json &js);

  /**
   * Converts a json object of type array
   * into a vector of shared_ptr object of given type
   *
   * @tparam T Type of the objects to create
   * @param js json object to convert
   * @return The vector of created objects
   */
  template < class T >
  std::vector<std::shared_ptr<T>> to_objects(const json &js);

  /**
   * Convert a json string into a new object
   * wrapped into a unique_ptr
   *
   * @tparam T Type of the object to create
   * @param str Json string to convert
   * @return
   */
  template < class T >
  std::unique_ptr<T> to_object(const std::string &str);

  /**
   * Convert a json string into a new object
   * wrapped into a unique_ptr
   *
   * @tparam T Type of the object to create
   * @param str Json string to convert
   * @return
   */
  template < class T >
  std::unique_ptr<T> to_object(const char *str);

  /**
   * Convert a json array string into a new array of
   * objects wrapped into a unique_ptr
   *
   * @tparam T Type of the object to create
   * @param str Json string to convert
   * @return Array of created objects
   */
  template < class T >
  std::vector<std::shared_ptr<T>> to_objects(const std::string &str);

  /**
   * Convert a json array string into a new array of
   * objects wrapped into a unique_ptr
   *
   * @tparam T Type of the object to create
   * @param str Json string to convert
   * @return Array of created objects
   */
  template < class T >
  std::vector<std::shared_ptr<T>> to_objects(const char *str);

private:
  json_object_serializer json_object_serializer_;
  object_json_serializer object_json_serializer_;
};

template<typename T>
std::string json_object_mapper::to_string(const object_ptr<T> &obj, const json_format &format)
{
  return json_object_serializer_.to_json_string(obj, format);
}

template<class T>
std::string json_object_mapper::to_string(const std::vector<T> &array, const json_format &format)
{
  return json_object_serializer_.to_json_string(array, format);
}

template<class T>
std::string json_object_mapper::to_string(const object_view<T> &array, const json_format &format)
{
  return json_object_serializer_.to_json_string(array, format);
}

template<typename T>
json json_object_mapper::to_json(const object_ptr<T> &obj)
{
  return object_json_serializer_.to_json(obj);
}

template<class T>
json json_object_mapper::to_json(const object_view<T> &objects)
{
  return object_json_serializer_.to_json(objects);
}

template<class T>
std::unique_ptr<T> json_object_mapper::to_object(const json &js)
{
  return to_object<T>(js.str());
}

template<class T>
std::vector<std::shared_ptr<T>> json_object_mapper::to_objects(const json &js)
{
  return to_objects<T>(js.str());
}

template<class T>
std::unique_ptr<T> json_object_mapper::to_object(const std::string &str)
{
  return to_object<T>(str.c_str());
}

template<class T>
std::unique_ptr<T> json_object_mapper::to_object(const char *str)
{
basic_json_mapper<std::unique_ptr<T>, detail::json_object_mapper_serializer> mapper;
  return mapper.object_from_string(str);
}

template<class T>
std::vector<std::shared_ptr<T>> json_object_mapper::to_objects(const std::string &str)
{
  return to_objects<T>(str.c_str());
}

template<class T>
std::vector<std::shared_ptr<T>> json_object_mapper::to_objects(const char *str)
{
  basic_json_mapper<std::shared_ptr<T>, detail::json_object_mapper_serializer> mapper;
  return mapper.array_from_string(str);
}
}

#endif //MATADOR_JSON_OBJECT_MAPPER_HPP
