//
// Created by sascha on 11.01.20.
//

#ifndef MATADOR_JSON_MAPPER_HPP
#define MATADOR_JSON_MAPPER_HPP

#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"
#include "matador/utils/memory.hpp"
#include "matador/utils/time.hpp"

#include <set>
#include <unordered_set>

namespace matador {

template < class T >
class json_mapper : public generic_json_parser<json_mapper<T>>
{
public:
  json_mapper() = default;

  T object_from_string(const char *str, bool check_for_eos = true);
  void object_from_string(const char *str, T *obj, bool check_for_eos = true);

  std::vector<T> array_from_string(const char *str, bool check_for_eos = true);

  /// @cond OOS_DEV //
  void on_parse_object(bool check_for_eos);
//  void on_parse_array(bool check_for_eos);
  void on_begin_object();
  void on_object_key(const std::string &key);
  void on_end_object();

  void on_begin_array();
  void on_end_array();

  void on_string(const std::string &value);
  void on_integer(long long value);
  void on_real(double value);
  void on_bool(bool value);
  void on_null();
  /// @endcond OOS_DEV //

public:
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
  T object_;
  std::vector<T> array_;
  json value_;
  std::string key_;

  std::string object_key_;
  std::string array_key_;
  bool is_array_ = false;
  bool is_object_ = false;

  const char *json_array_cursor_ = nullptr;
};

template<class T>
T json_mapper<T>::object_from_string(const char *str, bool check_for_eos)
{
  key_.clear();
  object_ = T();
  this->parse_json_object(str, check_for_eos);
  return std::move(object_);
}

template<class T>
void json_mapper<T>::object_from_string(const char *str, T *obj, bool check_for_eos)
{
  key_.clear();
  object_ = T();
  this->parse_json_object(str, check_for_eos);
  *obj = std::move(object_);
}

template<class T>
std::vector<T> json_mapper<T>::array_from_string(const char *str, bool check_for_eos)
{
  key_.clear();
  array_.clear();
  this->parse_json_array(str, check_for_eos);
  return array_;
}

template<class T>
void json_mapper<T>::on_parse_object(bool check_for_eos)
{
//  if (key_.empty()) {
  if (!is_object_) {
    generic_json_parser<json_mapper<T>>::on_parse_object(check_for_eos);
  } else {
    object_key_ = key_;
    access::serialize(*this, object_);
    object_key_.clear();
  }
}

//template<class T>
//void json_mapper<T>::on_parse_array(bool check_for_eos)
//{
//  if (key_.empty()) {
//    generic_json_parser<json_mapper<T>>::on_parse_array(check_for_eos);
//  } else {
//    object_key_ = key_;
//    access::serialize(*this, object_);
//    object_key_.clear();
//  }
//}

template<class T>
void json_mapper<T>::on_begin_object()
{
  if (is_array_) {
    is_object_ = true;
    object_ = T();
  } else {
    object_key_ = key_;
  }
  key_.clear();
}

template<class T>
void json_mapper<T>::on_object_key(const std::string &key)
{
//  if (!object_key_.empty()) {
//    return;
//  }
  key_ = key;
}

template<class T>
void json_mapper<T>::on_end_object()
{
  object_key_.clear();
  key_.clear();
  if (is_array_) {
    is_object_ = false;
    array_.emplace_back(std::move(object_));
  }
}

template<class T>
void json_mapper<T>::on_begin_array()
{
  is_array_ = true;
  array_key_ = key_;
  json_array_cursor_ = this->json_cursor();
  if (!object_key_.empty()) {
    access::serialize(*this, object_);
    this->sync_cursor(this->json_cursor() - 1);
  } else {
    value_ = json::array();
  }
}

template<class T>
void json_mapper<T>::on_end_array()
{
  is_array_ = false;
  array_key_.clear();
  json_array_cursor_ = nullptr;
  if (object_key_.empty()) {
    access::serialize(*this, object_);
  }
}

template< class T, class V >
void try_push_back(std::vector<T> &, const V &)
{
}

template< class T >
void try_push_back(std::vector<T> &vec, const std::string &value, typename std::enable_if<std::is_convertible<T, std::string>::value>::type*)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, long long value, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, double value, typename std::enable_if<std::is_floating_point<T>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, bool value, typename std::enable_if<std::is_same<T, bool>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &, nullptr_t)
{
}

template<class T>
void json_mapper<T>::on_string(const std::string &value)
{
  if (object_key_.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, value);
    } else /*if (is_array_ && is_object_)*/ {
      value_ = value;
      access::serialize(*this, object_);
    }
  } else {
    value_ = value;
    access::serialize(*this, object_);
  }
}

template < class T >
void json_mapper<T>::on_integer(long long value)
{
  if (object_key_.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, value);
    } else /*if (is_array_ && is_object_)*/ {
      value_ = value;
      access::serialize(*this, object_);
    }
  } else {
    value_ = value;
    access::serialize(*this, object_);
  }
}

template < class T >
void json_mapper<T>::on_real(double value)
{
  if (object_key_.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, value);
    } else /*if (is_array_ && is_object_)*/ {
      value_ = value;
      access::serialize(*this, object_);
    }
  } else {
    value_ = value;
    access::serialize(*this, object_);
  }
}

template<class T>
void json_mapper<T>::on_bool(bool value)
{
  if (object_key_.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, value);
    } else /*if (is_array_ && is_object_)*/ {
      value_ = value;
      access::serialize(*this, object_);
    }
  } else {
    value_ = value;
    access::serialize(*this, object_);
  }
}

template<class T>
void json_mapper<T>::on_null()
{
  if (object_key_.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, nullptr);
    } else {
      value_ = nullptr;
      access::serialize(*this, object_);
    }
  } else {
    value_ = nullptr;
    access::serialize(*this, object_);
  }
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, V &obj, typename std::enable_if<std::is_class<V>::value>::type *)
{
  if (object_key_ != id) {
    return;
  }
  json_mapper<V> mapper;
  mapper.object_from_string(this->json_cursor(), &obj, false);
  this->sync_cursor(mapper.json_cursor());
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, identifier<V> &pk, typename std::enable_if<
std::is_integral<V>::value && !std::is_same<bool, V>::value>::type *)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_integer()) {
    return;
  }

  pk.value(value_.as<V>());
}

template<class T>
void json_mapper<T>::serialize(const char *id, identifier<std::string> &pk)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_string()) {
    return;
  }

  pk.value(value_.as<std::string>());
}

template<class T>
void json_mapper<T>::serialize(const char *id, std::string &to)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_string()) {
    return;
  }
  to = value_.as<std::string>();
}

template<class T>
void json_mapper<T>::serialize(const char *id, bool &to)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_boolean()) {
    return;
  }
  to = value_.as<bool>();
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, V &to, typename std::enable_if<std::is_floating_point<V>::value>::type *)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_real()) {
    return;
  }
  to = value_.as<V>();
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, V &to, typename std::enable_if<
std::is_integral<V>::value && !std::is_same<bool, V>::value>::type *)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_integer()) {
    return;
  }
  to = value_.as<V>();
}

template<class T>
template<int SIZE, class V>
void json_mapper<T>::serialize(const char *id, identifier<varchar<SIZE, V>> &pk)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_string()) {
    return;
  }

  pk.value(value_.as<std::string>());
}

template<class T>
void json_mapper<T>::serialize(const char *id, std::string &to, size_t)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_string()) {
    return;
  }
  to = value_.as<std::string>();
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, std::unordered_set<V> &cont)
{
  if (key_ != id) {
    return;
  }

  for (auto &val : value_) {
    if (!val.template fits_to_type<V>()) {
      continue;
    }
    cont.insert(val.template as<V>());
  }
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, std::vector<V> &v, typename std::enable_if<!std::is_class<V>::value>::type*)
{
  if (key_ != id) {
    return;
  }

  for (auto &val : value_) {
    if (!val.template fits_to_type<V>()) {
      continue;
    }
    v.push_back(val.template as<V>());
  }
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, std::vector<V> &v, typename std::enable_if<std::is_class<V>::value>::type*)
{
  if (object_key_ != id) {
    return;
  }

  json_mapper<V> mapper;
  v = mapper.array_from_string(json_array_cursor_, false);
  this->sync_cursor(mapper.json_cursor());
}

template<class T>
void json_mapper<T>::serialize(const char *id, date &to)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_string()) {
    return;
  }
  to.set(value_.as<std::string>().c_str(), "%Y-%m-%d");
}

template<class T>
void json_mapper<T>::serialize(const char *id, time &to)
{
  if (key_ != id) {
    return;
  }
  if (!value_.is_string()) {
    return;
  }
  to.set(value_.as<std::string>().c_str());
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, std::list<V> &cont)
{
  if (key_ != id) {
    return;
  }

  for (auto &val : value_) {
    if (!val.template fits_to_type<V>()) {
      continue;
    }
    cont.push_back(val.template as<V>());
  }
}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, std::set<V> &cont)
{
  if (key_ != id) {
    return;
  }

  for (auto &val : value_) {
    if (!val.template fits_to_type<V>()) {
      continue;
    }
    cont.insert(val.template as<V>());
  }
}

}
#endif //MATADOR_JSON_MAPPER_HPP