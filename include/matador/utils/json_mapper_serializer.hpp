#ifndef MATADOR_JSON_MAPPER_SERIALIZER_HPP
#define MATADOR_JSON_MAPPER_SERIALIZER_HPP

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

#include <set>
#include <unordered_set>
#include <cstddef>

namespace matador {
namespace detail {

/// @cond MATADOR_DEV
class OOS_UTILS_API json_mapper_serializer
{
public:
  explicit json_mapper_serializer(details::mapper_runtime &runtime_data)
    : runtime_data_(runtime_data)
  {}

public:
  template < typename T >
  T create();

  template < class V >
  void serialize(V &obj);

  template < class V >
  void serialize(const char *id, V &obj, typename std::enable_if<std::is_class<V>::value>::type* = 0);
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
  template < class V >
  void serialize(const char *id, std::list<V> &cont);
  template < class V >
  void serialize(const char *id, std::vector<V> &cont, typename std::enable_if<!std::is_class<V>::value>::type* = 0);
  template < class V >
  void serialize(const char *id, std::vector<V> &cont, typename std::enable_if<std::is_class<V>::value>::type* = 0);
  template < class V >
  void serialize(const char *id, std::set<V> &cont);
  template < class V >
  void serialize(const char *id, std::unordered_set<V> &cont);

private:
  details::mapper_runtime &runtime_data_;
};

template<typename T>
T json_mapper_serializer::create()
{
  return std::move(T());
}

template<class V>
void json_mapper_serializer::serialize(V &obj)
{
  access::serialize(*this, obj);
}

template<class V>
void json_mapper_serializer::serialize(const char *id, V &obj, typename std::enable_if<std::is_class<V>::value>::type *)
{
  if (runtime_data_.object_key != id) {
    return;
  }
  basic_json_mapper<V, json_mapper_serializer> mapper;
  mapper.object_from_string(runtime_data_.cursor.json_cursor_, &obj, false);
  runtime_data_.cursor.sync_cursor(mapper.cursor().json_cursor_);
}

template<class V>
void json_mapper_serializer::serialize(const char *id, identifier<V> &pk, typename std::enable_if<
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

template<class V>
void json_mapper_serializer::serialize(const char *id, V &to, typename std::enable_if<std::is_floating_point<V>::value>::type *)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_real()) {
    return;
  }
  to = runtime_data_.value.as<V>();
}

template<class V>
void json_mapper_serializer::serialize(const char *id, V &to, typename std::enable_if<
  std::is_integral<V>::value && !std::is_same<bool, V>::value>::type *)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_integer()) {
    return;
  }
  to = runtime_data_.value.as<V>();
}

template<int SIZE, class V>
void json_mapper_serializer::serialize(const char *id, identifier<varchar<SIZE, V>> &pk)
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
void json_mapper_serializer::serialize(const char *id, std::unordered_set<V> &cont)
{
  if (runtime_data_.key != id) {
    return;
  }

  for (auto &val : runtime_data_.value) {
    if (!val.template fits_to_type<V>()) {
      continue;
    }
    cont.insert(val.template as<V>());
  }
}

template<class V>
void json_mapper_serializer::serialize(const char *id, std::vector<V> &cont, typename std::enable_if<!std::is_class<V>::value>::type*)
{
  if (runtime_data_.key != id) {
    return;
  }

  for (auto &val : runtime_data_.value) {
    if (!val.template fits_to_type<V>()) {
      continue;
    }
    cont.push_back(val.template as<V>());
  }
}

template<class V>
void json_mapper_serializer::serialize(const char *id, std::vector<V> &cont, typename std::enable_if<std::is_class<V>::value>::type*)
{
  if (runtime_data_.object_key != id) {
    return;
  }

  basic_json_mapper<V, json_mapper_serializer> mapper;
  cont = mapper.array_from_string(runtime_data_.json_array_cursor, false);
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().json_array_cursor);
}

template<class V>
void json_mapper_serializer::serialize(const char *id, std::list<V> &cont)
{
  if (runtime_data_.key != id) {
    return;
  }

  for (auto &val : runtime_data_.value) {
    if (!val.template fits_to_type<V>()) {
      continue;
    }
    cont.push_back(val.template as<V>());
  }
}

template<class V>
void json_mapper_serializer::serialize(const char *id, std::set<V> &cont)
{
  if (runtime_data_.key != id) {
    return;
  }

  for (auto &val : runtime_data_.value) {
    if (!val.template fits_to_type<V>()) {
      continue;
    }
    cont.insert(val.template as<V>());
  }
}

/// @endcond

}
}
#endif //MATADOR_JSON_MAPPER_SERIALIZER_HPP
