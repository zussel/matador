#ifndef MATADOR_JSON_OBJECT_MAPPER_SERIALIZER_HPP
#define MATADOR_JSON_OBJECT_MAPPER_SERIALIZER_HPP

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

#include "matador/json/basic_json_mapper.hpp"
#include "matador/json/json.hpp"
#include "matador/json/json_mapper_serializer.hpp"
#include "matador/json/json_parser.hpp"
#include "matador/utils/date.hpp"

#include "matador/object/has_many.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/has_one.hpp"

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

  template < class V, object_holder_type OPT >
  void serialize(object_pointer<V, OPT> &obj);
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
  void serialize(const char *id, has_many<Value, Container> &x, const char *, const char *, cascade_type, typename std::enable_if<!is_builtin<Value>::value>::type* = 0);
  template < class Value, template <class ...> class Container >
  void serialize(const char *id, has_many<Value, Container> &x, const char *, const char *, cascade_type, typename std::enable_if<is_builtin<Value>::value>::type* = 0);

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

template < class T, object_holder_type OPT >
object_pointer<T, OPT> create_object(object_pointer<T, OPT>&)
{
  auto *val = new typename object_pointer<T, OPT>::object_type;
  return object_pointer<T, OPT>(val);
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

template<class V, object_holder_type OPT>
void json_object_mapper_serializer::serialize(object_pointer<V, OPT> &obj)
{
  access::serialize(*this, *obj);
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

  basic_json_mapper<object_ptr<Value>, json_object_mapper_serializer> mapper;
  auto result = mapper.object_from_string(runtime_data_.cursor.json_cursor_, false);
  x = result;
  runtime_data_.cursor.sync_cursor(mapper.runtime_data().cursor());
}

template<class Value>
void json_object_mapper_serializer::serialize(const char *id, has_one<Value> &x, cascade_type)
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
void json_object_mapper_serializer::serialize(const char *id, has_many<Value, Container> &x, const char *,
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
void json_object_mapper_serializer::serialize(const char *id, has_many<Value, Container> &x, const char *,
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
//    typename has_many_item_holder<Value>::value_type v(val.template as<Value>());
//    x.append(has_many_item_holder<Value>(v, nullptr));
  }
}
/// @endcond

}
}
#endif //MATADOR_JSON_OBJECT_MAPPER_SERIALIZER_HPP
