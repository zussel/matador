//
// Created by sascha on 11.01.20.
//

#ifndef MATADOR_JSON_MAPPER_HPP
#define MATADOR_JSON_MAPPER_HPP

#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/memory.hpp"
#include "matador/utils/json_identifier_mapper.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/has_one.hpp"

namespace matador {

template < class T >
class json_object_mapper : public generic_json_parser<json_object_mapper<T>>
{
public:
  json_object_mapper() : generic_json_parser<json_object_mapper<T>>(this) {}

  T* from_string(const char *str, bool is_root = true);

  /// @cond OOS_DEV //
  void on_begin_object();
  void on_object_key(const std::string &key);
  void on_end_object();

  void on_begin_array();
  void on_end_array();

  void on_string(const std::string &value);
  void on_number(typename generic_json_parser<json_object_mapper<T>>::number_t value);
  void on_bool(bool value);
  void on_null();
  /// @endcond OOS_DEV //

public:
  template < class V >
  void serialize(V &obj);
  void serialize(const char *id, basic_identifier &pk);
  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if<std::is_floating_point<V>::value>::type* = 0);
  void serialize(const char *id, bool &to);
  void serialize(const char *id, std::string &to, size_t);
  void serialize(const char *id, date &to);
  template<class Value>
  void serialize(const char *id, belongs_to<Value> &x, cascade_type);
  template<class Value>
  void serialize(const char *id, has_one<Value> &x, cascade_type);

private:
  std::unique_ptr<T> object_;
  value_t value_;
  std::string key_;

  const char *object_cursor_ = nullptr;
  std::string object_key_;

  json_identifier_mapper id_mapper_;
};

template<class T>
T* json_object_mapper<T>::from_string(const char *str, bool is_root)
{
  std::cout << "start parsing json [" << str << "]\n";
  object_ = matador::make_unique<T>();
  this->parse_json(str, is_root);
  return object_.release();
}

template<class T>
void json_object_mapper<T>::on_begin_object()
{
  std::cout << "starting object [" << key_ << "]\n";
  object_key_ = key_;
  key_.clear();
  object_cursor_ = this->json_cursor();
}

template<class T>
void json_object_mapper<T>::on_object_key(const std::string &key)
{
  if (!object_key_.empty()) {
    std::cout << "skipping setting of key [" << key << "]\n";
    return;
  }
  key_ = key;
  std::cout << "read key [" << key_ << "]\n";
}

template<class T>
void json_object_mapper<T>::on_end_object()
{
  std::cout << "finished object [" << object_key_ << "]\n";
  object_cursor_ = nullptr;
  object_key_.clear();
}

template<class T>
void json_object_mapper<T>::on_begin_array()
{

}

template<class T>
void json_object_mapper<T>::on_end_array()
{

}

template<class T>
void json_object_mapper<T>::on_string(const std::string &value)
{
  if (object_key_.empty()) {
    value_.set(value);
  }
  access::serialize(*this, *object_);
}

template<class T>
void json_object_mapper<T>::on_number(typename generic_json_parser<json_object_mapper<T>>::number_t value)
{
  if (object_key_.empty()) {
    if (value.is_real) {
      value_.set(value.real);
    } else {
      value_.set(value.integer);
    }
  }
  access::serialize(*this, *object_);
}

template<class T>
void json_object_mapper<T>::on_bool(bool value)
{
  if (object_key_.empty()) {
    value_.set(value);
  }
  access::serialize(*this, *object_);
}

template<class T>
void json_object_mapper<T>::on_null()
{
  if (object_cursor_ != nullptr) {
    return;
  }
}

template<class T>
template<class V>
void json_object_mapper<T>::serialize(V &obj)
{
  access::serialize(*this, obj);
}

template<class T>
void json_object_mapper<T>::serialize(const char *id, basic_identifier &pk)
{
  if (key_ != id) {
    return;
  }
  std::cout << "key [" << id << "]: assigning value [" << value_ << "]\n";
  id_mapper_.set_identifier_value(pk, &value_);
}

template<class T>
template<class V>
void json_object_mapper<T>::serialize(const char *id, V &to, typename std::enable_if<
std::is_integral<V>::value && !std::is_same<bool, V>::value>::type *)
{
  if (key_ != id) {
    return;
  }
  to = (V)value_.integer;
}

template<class T>
void json_object_mapper<T>::serialize(const char *id, bool &to)
{
  if (key_ != id) {
    return;
  }
  to = value_.boolean;
}

template<class T>
void json_object_mapper<T>::serialize(const char *id, std::string &to, size_t)
{
  if (key_ != id) {
    return;
  }
  to = value_.str;
}

template<class T>
void json_object_mapper<T>::serialize(const char *id, date &to)
{
  if (key_ != id) {
    return;
  }
  to.set(value_.str.c_str(), "%Y-%m-%d");
}

template<class T>
template<class Value>
void json_object_mapper<T>::serialize(const char *id, belongs_to<Value> &x, cascade_type)
{
  if (object_key_ != id) {
    return;
  }

  json_object_mapper<Value> mapper;
  x = mapper.from_string(object_cursor_, false);
  this->sync_cursor(mapper.json_cursor());
}

template<class T>
template<class Value>
void json_object_mapper<T>::serialize(const char *id, has_one<Value> &x, cascade_type)
{
  if (object_key_ != id) {
    return;
  }

  json_object_mapper<Value> mapper;
  x = mapper.from_string(object_cursor_, false);
  this->sync_cursor(mapper.json_cursor());
}

template<class T>
template<class V>
void json_object_mapper<T>::serialize(const char *id, V &to,
                                      typename std::enable_if<std::is_floating_point<V>::value>::type *)
{
  if (key_ != id) {
    return;
  }
  to = (V)value_.real;
}
}
#endif //MATADOR_JSON_MAPPER_HPP
