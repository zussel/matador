//
// Created by sascha on 11.01.20.
//

#ifndef MATADOR_JSON_MAPPER_HPP
#define MATADOR_JSON_MAPPER_HPP

#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"
#include "matador/utils/memory.hpp"
#include "matador/utils/time.hpp"

namespace matador {

template < class T >
class json_mapper : public generic_json_parser<json_mapper<T>>
{
public:
  json_mapper() = default;

  std::unique_ptr<T> object_from_string(const char *str, bool is_root = true);
  void object_from_string(const char *str, T *obj, bool is_root = true);

  std::vector<std::unique_ptr<T>> array_from_string(const char *str, bool is_root = true);

  /// @cond OOS_DEV //
  void on_parse_object(bool is_root);
  void on_begin_object();
  void on_object_key(const std::string &key);
  void on_end_object();

  void on_begin_array();
  void on_end_array();

  void on_string(const std::string &value);
  void on_number(typename generic_json_parser<json_mapper<T>>::number_t value);
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
  std::unique_ptr<T> object_;
  json value_;
  std::string key_;

  const char *object_cursor_ = nullptr;
  std::string object_key_;
};

template<class T>
std::unique_ptr<T> json_mapper<T>::object_from_string(const char *str, bool is_root)
{
  object_ = matador::make_unique<T>();
  this->parse_json_object(str, is_root);
  return std::move(object_);
}

template<class T>
void json_mapper<T>::object_from_string(const char *str, T *obj, bool is_root)
{
  object_ = matador::make_unique<T>();
  object_.reset(obj);
  this->parse_json_object(str, is_root);
  object_.release();
}

template<class T>
std::vector<std::unique_ptr<T>> json_mapper<T>::array_from_string(const char *str, bool is_root)
{
  std::vector<std::unique_ptr<T>> result;
  this->parse_json_array(str, is_root);
  return result;
}

template<class T>
void json_mapper<T>::on_parse_object(bool is_root)
{
  generic_json_parser::on_parse_object(is_root);
}

template<class T>
void json_mapper<T>::on_begin_object()
{
  object_key_ = key_;
  key_.clear();
  object_cursor_ = this->json_cursor();
}

template<class T>
void json_mapper<T>::on_object_key(const std::string &key)
{
  if (!object_key_.empty()) {
    return;
  }
  key_ = key;
}

template<class T>
void json_mapper<T>::on_end_object()
{
  object_cursor_ = nullptr;
  object_key_.clear();
}

template<class T>
void json_mapper<T>::on_begin_array()
{

}

template<class T>
void json_mapper<T>::on_end_array()
{

}

template<class T>
void json_mapper<T>::on_string(const std::string &value)
{
  if (object_key_.empty()) {
    value_ = value;
  }
  access::serialize(*this, *object_);
}

template<class T>
void json_mapper<T>::on_number(typename generic_json_parser<json_mapper<T>>::number_t value)
{
  if (object_key_.empty()) {
    if (value.is_real) {
      value_ = value.real;
    } else {
      value_ = value.integer;
    }
  }
  access::serialize(*this, *object_);
}

template<class T>
void json_mapper<T>::on_bool(bool value)
{
  if (object_key_.empty()) {
    value_ = value;
  }
  access::serialize(*this, *object_);
}

template<class T>
void json_mapper<T>::on_null()
{

}

template<class T>
template<class V>
void json_mapper<T>::serialize(const char *id, V &obj, typename std::enable_if<std::is_class<V>::value>::type *)
{
  if (object_key_ != id) {
    return;
  }
  json_mapper<V> mapper;
  mapper.object_from_string(object_cursor_, &obj, false);
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
void json_mapper<T>::serialize(const char *id, std::vector<V> &cont, typename std::enable_if<!std::is_class<V>::value>::type*)
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
void json_mapper<T>::serialize(const char *id, std::vector<V> &, typename std::enable_if<std::is_class<V>::value>::type*)
{
  if (key_ != id) {
    return;
  }

  json_mapper<V> mapper;
  mapper.array_from_string(object_cursor_, false);
  this->sync_cursor(mapper.json_cursor());

  for (auto &val : value_) {
    std::cout << "got object " << val << " for vector\n";
  }
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
