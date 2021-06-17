#ifndef MATADOR_JSON_OBJECT_SERIALIZER_HPP
#define MATADOR_JSON_OBJECT_SERIALIZER_HPP

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

#include "matador/utils/json_serializer.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/basic_has_many.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/has_one.hpp"

namespace matador {

class MATADOR_OBJECT_API json_object_serializer
{
public:
  json_object_serializer() = default;

  template< typename T, object_holder_type OPT >
  std::string to_json(const object_pointer<T, OPT> &obj, json_format format = json_format::compact)
  {
    format_ = format;
    json_.clear();
    append(*obj);
    newline();
    return json_;
  }

  template< typename T >
  std::string to_json(const object_view<T> &objects, json_format format = json_format::compact)
  {
    format_ = format;
    json result = json::array();

    json_.append("[");
    if (objects.size() < 2) {
      for (const auto &i : objects) {
        append(*i);
      }
    } else {
      auto it = objects.begin();
      append(*it.optr());
      ++it;
      for (;it != objects.end(); ++it) {
        json_.append(",");
        append(*it.optr());
      }
    }
    json_.append("]");

    return json_;
  }

  template < class V >
  void serialize(V &obj)
  {
    matador::access::serialize(*this, obj);
  }

  template< class V >
  void serialize(const char *id, identifier<V> &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    write_id(id);
    if (pk.is_valid()) {
      append(pk.value()).append(",");
    } else {
      append("null,");
    }
    newline();
  }

  template < class V >
  void serialize(const char *id, V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    write_id(id);
    append(obj);
    newline();
  }

  template < class V >
  void serialize(const char *id, V &val, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    write_id(id);
    append(val).append(",");
    newline();
  }

  void serialize(const char *id, identifier<std::string> &pk);
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
  std::string& write_id(const char *id);

  void begin_object();

  void end_object();

  void indent();

  void newline();

  std::string& append(const char *str);

  std::string& append(const bool &value);

  std::string& append(const std::string &str);

  std::string& append(std::string &val, size_t);

  template<class V>
  std::string& append(const V &value, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    json_.append(std::to_string(value));
    return json_;
  }

  template < class V >
  std::string& append(V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    begin_object();
    matador::access::serialize(*this, obj);
    auto idx = json_.find_last_of(',');
    json_.erase(idx, 1);
    end_object();
    newline();
    return json_;
  }

private:
  std::string json_;
  json_format format_ {};
  unsigned depth_ = 0;
};

template<class Value>
void json_object_serializer::serialize(const char *id, belongs_to<Value> &x, cascade_type)
{
  write_id(id);
  append(*x);
}

template<class Value>
void json_object_serializer::serialize(const char *id, has_one<Value> &x, cascade_type)
{
  if (x.empty()) {
    return;
  }
  write_id(id);
  append(*x);
}

template<class Value, template <class ...> class Container>
void json_object_serializer::serialize(const char *id, basic_has_many<Value, Container> &, const char *,
                                       const char *, cascade_type)
{
  write_id(id);
  json_.append("[],");
  newline();
}

}

#endif //MATADOR_JSON_OBJECT_SERIALIZER_HPP
