#ifndef MATADOR_BASIC_JSON_MAPPER_HPP
#define MATADOR_BASIC_JSON_MAPPER_HPP

#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/memory.hpp"
#include "matador/utils/time.hpp"

namespace matador {

namespace details {
struct mapper_runtime {
  json value;
  std::string key;
  std::string object_key;
  std::string array_key;
  const char *json_cursor = nullptr;
  const char *json_array_cursor = nullptr;
};

}

template < class T, class S >
class basic_json_mapper : public generic_json_parser<basic_json_mapper<T, S> >
{
public:
  typedef generic_json_parser<basic_json_mapper<T, S> > base;

  basic_json_mapper() : serializer_(mapper_runtime_)
  {
    mapper_runtime_.json_cursor = this->json_cursor();
  }

  T object_from_string(const char *str, bool check_for_eos = true);
  void object_from_string(const char *str, T *obj, bool check_for_eos = true);

  std::vector<T> array_from_string(const char *str, bool check_for_eos = true);

  /// @cond OOS_DEV //
  void on_parse_object(bool check_for_eos);
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

  const details::mapper_runtime& runtime_data() const
  {
    return mapper_runtime_;
  }
protected:

  T object_;
  std::vector<T> array_;

  details::mapper_runtime mapper_runtime_;

  bool is_array_ = false;
  bool is_object_ = false;

  S serializer_;
};

template<class T, class C>
T basic_json_mapper<T, C>::object_from_string(const char *str, bool check_for_eos)
{
  mapper_runtime_.key.clear();
  mapper_runtime_.json_cursor = str;
  object_ = T();
  this->parse_json_object(str, check_for_eos);
  return std::move(object_);
}

template<class T, class C>
void basic_json_mapper<T, C>::object_from_string(const char *str, T *obj, bool check_for_eos)
{
  mapper_runtime_.key.clear();
  mapper_runtime_.json_cursor = str;
  object_ = T();
  this->parse_json_object(str, check_for_eos);
  *obj = std::move(object_);
}

template<class T, class C>
std::vector<T> basic_json_mapper<T, C>::array_from_string(const char *str, bool check_for_eos)
{
  mapper_runtime_.key.clear();
  mapper_runtime_.json_cursor = str;
  array_.clear();
  this->parse_json_array(str, check_for_eos);
  return array_;
}

template<class T, class C>
void basic_json_mapper<T, C>::on_parse_object(bool check_for_eos)
{
  if (mapper_runtime_.key.empty()) {
    base::on_parse_object(check_for_eos);
  } else {
    mapper_runtime_.object_key = mapper_runtime_.key;
    access::serialize(serializer_, object_);
    mapper_runtime_.object_key.clear();
  }
}

template<class T, class C>
void basic_json_mapper<T, C>::on_begin_object()
{
  if (is_array_) {
    is_object_ = true;
    object_ = T();
  } else {
    mapper_runtime_.object_key = mapper_runtime_.key;
  }
  mapper_runtime_.key.clear();
}

template<class T, class C>
void basic_json_mapper<T, C>::on_object_key(const std::string &key)
{
  mapper_runtime_.key = key;
}

template<class T, class C>
void basic_json_mapper<T, C>::on_end_object()
{
  mapper_runtime_.object_key.clear();
  mapper_runtime_.key.clear();
  if (is_array_) {
    is_object_ = false;
    array_.emplace_back(std::move(object_));
  }
}

template<class T, class C>
void basic_json_mapper<T, C>::on_begin_array()
{
  is_array_ = true;
  mapper_runtime_.array_key = mapper_runtime_.key;
  mapper_runtime_.json_array_cursor = this->json_cursor();
  if (!mapper_runtime_.object_key.empty()) {
    access::serialize(serializer_, object_);
    this->sync_cursor(this->json_cursor() - 1);
  } else {
    mapper_runtime_.value = json::array();
  }
}

template<class T, class C>
void basic_json_mapper<T, C>::on_end_array()
{
  is_array_ = false;
  mapper_runtime_.array_key.clear();
  mapper_runtime_.json_array_cursor = this->json_cursor();
  if (!mapper_runtime_.key.empty()) {
    access::serialize(serializer_, object_);
  }
}

template< class T, class V >
void try_push_back(std::vector<T> &, json &jval, const V &value)
{
  jval.push_back(value);
}

template< class T >
void try_push_back(std::vector<T> &vec, json&, const std::string &value, typename std::enable_if<std::is_convertible<T, std::string>::value>::type*)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, json&, long long value, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, json&, double value, typename std::enable_if<std::is_floating_point<T>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, json&, bool value, typename std::enable_if<std::is_same<T, bool>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &, json&, std::nullptr_t)
{
}

template < class T, class C >
void basic_json_mapper<T, C>::on_string(const std::string &value)
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, value);
    } else /*if (is_array_ && is_object_)*/ {
      mapper_runtime_.value = value;
      access::serialize(serializer_, object_);
    }
  } else {
    mapper_runtime_.value = value;
    access::serialize(serializer_, object_);
  }
}

template < class T, class C >
void basic_json_mapper<T, C>::on_integer(long long value)
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, value);
    } else /*if (is_array_ && is_object_)*/ {
      mapper_runtime_.value = value;
      access::serialize(serializer_, object_);
    }
  } else {
    mapper_runtime_.value = value;
    access::serialize(serializer_, object_);
  }
}

template < class T, class C >
void basic_json_mapper<T, C>::on_real(double value)
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, value);
    } else /*if (is_array_ && is_object_)*/ {
      mapper_runtime_.value = value;
      access::serialize(serializer_, object_);
    }
  } else {
    mapper_runtime_.value = value;
    access::serialize(serializer_, object_);
  }
}

template < class T, class C >
void basic_json_mapper<T, C>::on_bool(bool value)
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, value);
    } else /*if (is_array_ && is_object_)*/ {
      mapper_runtime_.value = value;
      access::serialize(serializer_, object_);
    }
  } else {
    mapper_runtime_.value = value;
    access::serialize(serializer_, object_);
  }
}

template < class T, class C >
void basic_json_mapper<T, C>::on_null()
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, nullptr);
    } else {
      mapper_runtime_.value = nullptr;
      access::serialize(serializer_, object_);
    }
  } else {
    mapper_runtime_.value = nullptr;
    access::serialize(serializer_, object_);
  }
}

}

#endif //MATADOR_BASIC_JSON_MAPPER_HPP
