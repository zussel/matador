#ifndef MATADOR_JSON_MAPPER_HPP
#define MATADOR_JSON_MAPPER_HPP

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

#include "matador/utils/basic_json_mapper.hpp"
#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/memory.hpp"

#include "matador/object/basic_has_many.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/has_one.hpp"

namespace matador {

/// @cond MATADOR_DEV
class MATADOR_OBJECT_API json_object_mapper_serializer
{
public:
  explicit json_object_mapper_serializer(details::mapper_runtime &runtime_data);

public:
  template < class V >
  void serialize(V &obj);
  template < class V >
  void serialize(const char *id, identifier<V> &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  void serialize(const char *id, identifier<std::string> &pk);
  template < int SIZE, class V >
  void serialize(const char *id, identifier<varchar<SIZE, V>> &pk);
  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if<std::is_floating_point<V>::value>::type* = 0);
  void serialize(const char *id, bool &to);
  void serialize(const char *id, std::string &to);
  void serialize(const char *id, std::string &to, size_t);
  void serialize(const char *id, date &to);
  void serialize(const char *id, time &to);
  template<class Value>
  void serialize(const char *id, belongs_to<Value> &x, cascade_type);
  template<class Value>
  void serialize(const char *id, has_one<Value> &x, cascade_type);
  template < class Value, template <class ...> class Container >
  void serialize(const char *id, basic_has_many<Value, Container> &x, const char *, const char *, cascade_type);

private:
  details::mapper_runtime &runtime_data_;
};
/// @endcond

/**
 * @brief Shortcut to the json object mapper
 *
 * The json_object_mapper is used to map a json
 * string to an matador typed object. This means it
 * can contain relation objects like has_many, has_one
 * or belongs_to.
 */
template < class T >
using json_object_mapper = basic_json_mapper<T, json_object_mapper_serializer>;

/// @cond MATADOR_DEV
template<class V>
void json_object_mapper_serializer::serialize(V &obj)
{
  access::serialize(*this, obj);
}

template<class V>
void json_object_mapper_serializer::serialize(const char *id, identifier<V> &pk, typename std::enable_if<
std::is_integral<V>::value && !std::is_same<bool, V>::value>::type *)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_integer()) {
    return;
  }

  pk.value(runtime_data_.value.as<V>());
}

template<int SIZE, class V>
void json_object_mapper_serializer::serialize(const char *id, identifier<varchar<SIZE, V>> &pk)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_string()) {
    return;
  }

  pk.value(runtime_data_.value.as<std::string>());
}

template<class V>
void json_object_mapper_serializer::serialize(const char *id, V &to, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type *)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_integer()) {
    return;
  }
  to = runtime_data_.value.as<V>();
}

template<class V>
void json_object_mapper_serializer::serialize(const char *id, V &to, typename std::enable_if<std::is_floating_point<V>::value>::type *)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_real()) {
    return;
  }
  to = runtime_data_.value.as<V>();
}

template<class Value>
void json_object_mapper_serializer::serialize(const char *id, belongs_to<Value> &x, cascade_type)
{
  if (runtime_data_.object_key != id) {
    return;
  }

  json_object_mapper<Value> mapper;
  x = new Value;
  *x = mapper.object_from_string(runtime_data_.cursor.json_cursor_, false);
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().cursor());
}

template<class Value>
void json_object_mapper_serializer::serialize(const char *id, has_one<Value> &x, cascade_type)
{
  if (runtime_data_.object_key != id) {
    return;
  }

  json_object_mapper<Value> mapper;
  auto result = mapper.object_from_string(runtime_data_.cursor.json_cursor_, false);
  x = new Value;
  *x = std::move(result);
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().cursor());
}

template<class Value, template <class ...> class Container>
void json_object_mapper_serializer::serialize(const char *id, basic_has_many<Value, Container> &x, const char *,
                                              const char *, cascade_type)
{
  if (runtime_data_.object_key != id) {
    return;
  }

  json_object_mapper<Value> mapper;
  auto elements = mapper.array_from_string(runtime_data_.json_array_cursor, false);
  for (auto &&item : elements) {
    typename has_many_item_holder<Value>::object_type val(new Value(item));
    x.append(has_many_item_holder<Value>(val, nullptr));
  }
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().json_array_cursor);
}
/// @endcond
}
#endif //MATADOR_JSON_MAPPER_HPP
