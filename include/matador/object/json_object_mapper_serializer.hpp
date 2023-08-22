#ifndef MATADOR_JSON_OBJECT_MAPPER_SERIALIZER_HPP
#define MATADOR_JSON_OBJECT_MAPPER_SERIALIZER_HPP

#include "matador/object/export.hpp"

#include "matador/json/basic_json_mapper.hpp"
#include "matador/json/json.hpp"
#include "matador/json/json_mapper_serializer.hpp"
#include "matador/json/json_parser.hpp"
#include "matador/utils/date.hpp"

#include "matador/object/has_many.hpp"

namespace matador {
namespace detail {

/// @cond MATADOR_DEV
class MATADOR_OBJECT_API json_object_mapper_serializer
{
public:
  explicit json_object_mapper_serializer(details::mapper_runtime &runtime_data);

public:
  template < typename T >
  T create();

  template < class V >
  void serialize(V &obj);
  template < class V >
  void serialize(std::unique_ptr<V> &obj);
  template < class V >
  void serialize(std::shared_ptr<V> &obj);

  template < class V >
  void serialize(object_ptr<V> &obj);
  template < class V >
  void on_primary_key(const char *id, V &pk, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  void on_primary_key(const char *id, std::string &pk, const field_attributes &/*attr*/ = null_attributes);
  template < class V >
  void on_attribute(const char *id, V &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  template < class V >
  void on_attribute(const char *id, V &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_floating_point<V>::value>::type* = 0);
  template < class E >
  void on_attribute(const char *id, E &to, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_enum<E>::value>::type* = 0);
  void on_attribute(const char *id, bool &to, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, std::string &to, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, date &to, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, time &to, const field_attributes &/*attr*/ = null_attributes);
  template<class Value>
  void on_belongs_to(const char *id, object_ptr<Value> &x, cascade_type);
  template<class Value>
  void on_has_one(const char *id, object_ptr<Value> &x, cascade_type);
  template < class Value, template <class ...> class Container >
  void on_has_many(const char *id, has_many<Value, Container> &x, const char *, const char *, cascade_type, typename std::enable_if<!is_builtin<Value>::value>::type* = 0);
  template < class Value, template <class ...> class Container >
  void on_has_many(const char *id, has_many<Value, Container> &x, const char *, const char *, cascade_type, typename std::enable_if<is_builtin<Value>::value>::type* = 0);

private:
  details::mapper_runtime &runtime_data_;
};

template < class T >
T create_object(T&)
{
  return T();
}

template < class T >
std::unique_ptr<T> create_object(std::unique_ptr<T>&)
{
  return std::unique_ptr<T>(new T);
}

template < class T >
std::shared_ptr<T> create_object(std::shared_ptr<T>&)
{
  return std::shared_ptr<T>(new T);
}

template < class T >
object_ptr<T> create_object(object_ptr<T>&)
{
  auto *val = new typename object_ptr<T>::object_type;
  return object_ptr<T>(val);
}

template< typename T >
T json_object_mapper_serializer::create()
{
  T t;
  return create_object(t);
}

template<class V>
void json_object_mapper_serializer::serialize(V &obj)
{
  access::serialize(*this, obj);
}

template<class V>
void json_object_mapper_serializer::serialize(std::unique_ptr<V> &obj)
{
  access::serialize(*this, *obj);
}

template<class V>
void json_object_mapper_serializer::serialize(std::shared_ptr<V> &obj)
{
  access::serialize(*this, *obj);
}

template<class V>
void json_object_mapper_serializer::serialize(object_ptr<V> &obj)
{
  access::serialize(*this, *obj);
}

template<class V>
void json_object_mapper_serializer::on_primary_key(const char *id, V &pk, const field_attributes &/*attr*/, typename std::enable_if<
  std::is_integral<V>::value && !std::is_same<bool, V>::value>::type *)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_integer()) {
    return;
  }

  pk = runtime_data_.value.as<V>();
}

template<class V>
void json_object_mapper_serializer::on_attribute(const char *id, V &to, const field_attributes &/*attr*/, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type *)
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
void json_object_mapper_serializer::on_attribute(const char *id, V &to, const field_attributes &/*attr*/, typename std::enable_if<std::is_floating_point<V>::value>::type *)
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
void json_object_mapper_serializer::on_attribute(const char *id, V &to, const field_attributes &/*attr*/, typename std::enable_if<std::is_enum<V>::value>::type *)
{
  if (runtime_data_.key != id) {
    return;
  }
  if (!runtime_data_.value.is_number()) {
    return;
  }
  to = static_cast<V>(runtime_data_.value.as<int>());
}

template<class Value>
void json_object_mapper_serializer::on_belongs_to(const char *id, object_ptr<Value> &x, cascade_type)
{
  if (runtime_data_.object_key != id) {
    return;
  }

  basic_json_mapper<object_ptr<Value>, json_object_mapper_serializer> mapper;
  auto result = mapper.object_from_string(runtime_data_.cursor.json_cursor_, false);
  x = result;
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().cursor());
}

template<class Value>
void json_object_mapper_serializer::on_has_one(const char *id, object_ptr<Value> &x, cascade_type)
{
  if (runtime_data_.object_key != id) {
    return;
  }

  basic_json_mapper<object_ptr<Value>, json_object_mapper_serializer> mapper;
  auto result = mapper.object_from_string(runtime_data_.cursor.json_cursor_, false);
  x = result;
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().cursor());
}

template<class Value, template <class ...> class Container>
void json_object_mapper_serializer::on_has_many(const char *id, has_many<Value, Container> &x, const char *,
                                              const char *, cascade_type, typename std::enable_if<!is_builtin<Value>::value>::type*)
{
  if (runtime_data_.object_key != id) {
    return;
  }

  basic_json_mapper<typename has_many_item_holder<Value>::object_type, json_object_mapper_serializer> mapper;
  auto elements = mapper.array_from_string(runtime_data_.json_array_cursor, false);
  for (auto &&item : elements) {
    typename has_many_item_holder<Value>::value_type val(item);
    x.append(has_many_item_holder<Value>(val, nullptr));
  }
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().json_array_cursor);
}

template<class Value, template <class ...> class Container>
void json_object_mapper_serializer::on_has_many(const char *id, has_many<Value, Container> &x, const char *,
                                              const char *, cascade_type, typename std::enable_if<is_builtin<Value>::value>::type*)
{
  if (runtime_data_.key != id) {
    return;
  }

  for (auto &val : runtime_data_.value) {
    if (!val.template fits_to_type<Value>()) {
      continue;
    }
    x.insert(x.end(), val.template as<Value>());
  }
}
/// @endcond

}
}
#endif //MATADOR_JSON_OBJECT_MAPPER_SERIALIZER_HPP
