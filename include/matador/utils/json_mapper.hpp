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

  T* from_string(const char *str, bool is_root = true);

  /// @cond OOS_DEV //
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
  void serialize(const char *id, identifier<V> &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_integer()) {
      return;
    }

    pk.value(value_.as<V>());
  }

  void serialize(const char *id, identifier<std::string> &pk)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_string()) {
      return;
    }

    pk.value(value_.as<std::string>());
  }

  template < int SIZE, class V >
  void serialize(const char *id, identifier<varchar<SIZE, V>> &pk)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_string()) {
      return;
    }

    pk.value(value_.as<std::string>());
  }

  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_integer()) {
      return;
    }
    to = value_.as<V>();
  }

  template < class V >
  void serialize(const char *id, V &to, typename std::enable_if<std::is_floating_point<V>::value>::type* = 0)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_real()) {
      return;
    }
    to = value_.as<V>();
  }

  void serialize(const char *id, bool &to)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_boolean()) {
      return;
    }
    to = value_.as<bool>();
  }

  void serialize(const char *id, std::string &to)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_string()) {
      return;
    }
    to = value_.as<std::string>();
  }

  void serialize(const char *id, std::string &to, size_t)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_string()) {
      return;
    }
    to = value_.as<std::string>();
  }

  void serialize(const char *id, date &to)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_string()) {
      return;
    }
    to.set(value_.as<std::string>().c_str(), "%Y-%m-%d");
  }

  void serialize(const char *id, time &to)
  {
    if (key_ != id) {
      return;
    }
    if (!value_.is_string()) {
      return;
    }
    to.set(value_.as<std::string>().c_str());
  }

  template < class V >
  void serialize(const char *id, std::list<V> &cont)
  {
    if (key_ != id) {
      return;
    }

    for (auto &val : value_) {
      cont.push_back(val.template as<V>());
    }
  }

  template < class V >
  void serialize(const char *id, std::vector<V> &cont)
  {
    if (key_ != id) {
      return;
    }

    for (auto &val : value_) {
      cont.push_back(val.template as<V>());
    }
  }

  template < class V >
  void serialize(const char *id, std::set<V> &cont)
  {
    if (key_ != id) {
      return;
    }

    for (auto &val : value_) {
      cont.insert(val.template as<V>());
    }
  }

  template < class V >
  void serialize(const char *id, std::unordered_set<V> &cont)
  {
    if (key_ != id) {
      return;
    }

    for (auto &val : value_) {
      cont.insert(val.template as<V>());
    }
  }
private:
  std::unique_ptr<T> object_;
  json value_;
  std::string key_;
};

template<class T>
T* json_mapper<T>::from_string(const char *str, bool is_root)
{
  object_ = matador::make_unique<T>();
  this->parse_json(str, is_root);
  return object_.release();
}

template<class T>
void json_mapper<T>::on_begin_object()
{

}

template<class T>
void json_mapper<T>::on_object_key(const std::string &key)
{
  key_ = key;
}

template<class T>
void json_mapper<T>::on_end_object()
{

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
  value_ = value;
  access::serialize(*this, *object_);
}

template<class T>
void json_mapper<T>::on_number(typename generic_json_parser<json_mapper<T>>::number_t value)
{
  if (value.is_real) {
    value_ = value.real;
  } else {
    value_ = value.integer;
  }
  access::serialize(*this, *object_);
}

template<class T>
void json_mapper<T>::on_bool(bool value)
{
  value_ = value;
  access::serialize(*this, *object_);
}

template<class T>
void json_mapper<T>::on_null()
{

}
}
#endif //MATADOR_JSON_MAPPER_HPP
