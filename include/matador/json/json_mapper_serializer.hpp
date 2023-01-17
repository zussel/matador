#ifndef MATADOR_JSON_MAPPER_SERIALIZER_HPP
#define MATADOR_JSON_MAPPER_SERIALIZER_HPP

#include "matador/json/export.hpp"

#include "matador/json/basic_json_mapper.hpp"

#include <set>
#include <unordered_set>
#include <cstddef>

namespace matador {
namespace detail {

/// @cond MATADOR_DEV
class OOS_JSON_API json_mapper_serializer
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
  void on_attribute(const char *id, V &obj, typename std::enable_if<std::is_class<V>::value>::type* = 0);
  template < class V >
  void on_primary_key(const char *id, identifier<V> &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  void on_primary_key(const char *id, identifier<std::string> &pk);
  template < int SIZE, class V >
  void on_primary_key(const char *id, identifier<varchar<SIZE, V>> &pk);
  template < class V >
  void on_attribute(const char *id, V &to, long size = -1, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  template < class V >
  void on_attribute(const char *id, V &to, long size = -1, typename std::enable_if<std::is_floating_point<V>::value>::type* = 0);
  void on_attribute(const char *id, bool &to, long size = -1);
  void on_attribute(const char *id, std::string &to, long size = -1);
  void on_attribute(const char *id, date &to, long size = -1);
  void on_attribute(const char *id, time &to, long size = -1);
  template < class V >
  void on_attribute(const char *id, std::list<V> &cont, long size = -1);
  template < class V >
  void on_attribute(const char *id, std::vector<V> &cont, long size = -1, typename std::enable_if<!std::is_class<V>::value>::type* = 0);
  template < class V >
  void on_attribute(const char *id, std::vector<V> &cont, long size = -1, typename std::enable_if<std::is_class<V>::value>::type* = 0);
  template < class V >
  void on_attribute(const char *id, std::set<V> &cont, long size = -1);
  template < class V >
  void on_attribute(const char *id, std::unordered_set<V> &cont, long size = -1);

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
void json_mapper_serializer::on_attribute(const char *id, V &obj, typename std::enable_if<std::is_class<V>::value>::type *)
{
  if (runtime_data_.object_key != id) {
    return;
  }
  basic_json_mapper<V, json_mapper_serializer> mapper;
  mapper.object_from_string(runtime_data_.cursor.json_cursor_, &obj, false);
  runtime_data_.cursor.sync_cursor(mapper.cursor().json_cursor_);
}

template<class V>
void json_mapper_serializer::on_primary_key(const char *id, identifier<V> &pk, typename std::enable_if<
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
void json_mapper_serializer::on_attribute(const char *id, V &to, long /*size*/, typename std::enable_if<std::is_floating_point<V>::value>::type *)
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
void json_mapper_serializer::on_attribute(const char *id, V &to, long /*size*/, typename std::enable_if<
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
void json_mapper_serializer::on_primary_key(const char *id, identifier<varchar<SIZE, V>> &pk)
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
void json_mapper_serializer::on_attribute(const char *id, std::unordered_set<V> &cont, long /*size*/)
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
void json_mapper_serializer::on_attribute(const char *id, std::vector<V> &cont, long /*size*/, typename std::enable_if<!std::is_class<V>::value>::type*)
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
void json_mapper_serializer::on_attribute(const char *id, std::vector<V> &cont, long /*size*/, typename std::enable_if<std::is_class<V>::value>::type*)
{
  if (runtime_data_.object_key != id) {
    return;
  }

  basic_json_mapper<V, json_mapper_serializer> mapper;
  cont = mapper.array_from_string(runtime_data_.json_array_cursor, false);
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().json_array_cursor);
}

template<class V>
void json_mapper_serializer::on_attribute(const char *id, std::list<V> &cont, long /*size*/)
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
void json_mapper_serializer::on_attribute(const char *id, std::set<V> &cont, long /*size*/)
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
