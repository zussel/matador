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
#include "matador/utils/json_identifier_serializer.hpp"

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

//  using t_id_set = std::unordered_set<std::shared_ptr<basic_identifier>, detail::identifier_hash<std::shared_ptr<basic_identifier>>, detail::identifier_equal_shared>;
  using t_id_set = std::unordered_set<basic_identifier*, detail::identifier_hash<basic_identifier>, detail::identifier_equal>;
  using t_name_id_set_pair = std::pair<std::string, t_id_set>;
  using t_type_id_map = std::unordered_map<std::type_index, t_name_id_set_pair>;

  t_type_id_map type_id_map_;

  template< typename T, object_holder_type OPT >
  std::string to_json(const object_pointer<T, OPT> &obj, json_format format = json_format::compact)
  {
    type_id_map_.clear();
    format_ = format;
    json_.clear();
    append(const_cast<object_pointer<T, OPT>&>(obj));
    newline();
    return json_;
  }

  template< typename T >
  std::string to_json(const object_view<T> &objects, json_format format = json_format::compact)
  {
    type_id_map_.clear();
    format_ = format;
    json result = json::array();

    json_.append("[");
    if (objects.size() < 2) {
      for (const auto &i : objects) {
        append(*i);
      }
    } else {
      auto it = objects.begin();
      append(it.optr());
      ++it;
      for (;it != objects.end(); ++it) {
        json_.append(",");
        append(it.optr());
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

      auto it = type_id_map_.find(*current_type_index_);
      if (it != type_id_map_.end() && it->second.first.empty()) {
        it->second.first.assign(id);
      }
      append(pk.value());
      json_.append(",");
    } else {
      json_.append("null,");
    }
    newline();
  }

  template < class V >
  void serialize(const char *id, V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = nullptr)
  {
    write_id(id);
    append(obj);
    newline();
  }

  template < class V >
  void serialize(const char *id, V &val, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    write_id(id);
    append(val);
    json_.append(",");
    newline();
  }

  void serialize(const char *id, identifier<std::string> &pk);
  void serialize(const char *id, bool &val);
  void serialize(const char *id, std::string &val);
  void serialize(const char *id, std::string &val, size_t len);
  void serialize(const char *id, const char *val, size_t len);
  void serialize(const char *id, date &val);
  void serialize(const char *id, time &val);

  template<class Value>
  void serialize(const char *id, belongs_to<Value> &x, cascade_type);
  template<class Value>
  void serialize(const char *id, has_one<Value> &x, cascade_type);
  template < class Value, template <class ...> class Container >
  void serialize(const char *id, basic_has_many<Value, Container> &x, const char *, const char *, cascade_type);

private:
  void write_id(const char *id);
  void begin_object();
  void end_object();
  void indent();
  void newline();

  void append(const char *str);
  void append(const bool &value);
  void append(const std::string &str);
  void append(std::string &val, size_t);
  void append(const char *val, size_t);

  template<class V>
  void append(const V &value, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = nullptr)
  {
    json_.append(std::to_string(value));
  }

  template< typename T, object_holder_type OPT >
  void append(object_pointer<T, OPT> &x)
  {
    auto tindex = std::type_index(typeid(T));
    auto it = type_id_map_.find(tindex);
    if (it != type_id_map_.end()) {
      auto id_it = it->second.second.find(x.primary_key());
      if (id_it == it->second.second.end()) {
        it->second.second.insert(x.primary_key());
        current_type_index_ = &it->first;
        append(*x);
        current_type_index_ = nullptr;
      } else {
        // only serialize id
        begin_object();
        write_id(it->second.first.c_str());
        json_.append(identifier_serializer_.serialize(*x.primary_key()));
        end_object();
        newline();
        //x.primary_key();
      }
    } else {
      auto ret = type_id_map_.insert(std::make_pair(tindex, t_name_id_set_pair("", t_id_set())));
      current_type_index_ = &ret.first->first;
      ret.first->second.second.insert(x.primary_key());
      append(*x);
      current_type_index_ = nullptr;
    }
  }

  template < class V >
  void append(V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    begin_object();
    matador::access::serialize(*this, obj);
    auto idx = json_.find_last_of(',');
    json_.erase(idx, 1);
    end_object();
    newline();
  }

private:
  std::string json_;
  json_format format_ {};
  unsigned depth_ = 0;

  const std::type_index *current_type_index_ = nullptr;

  json_identifier_serializer identifier_serializer_;
};

template<class Value>
void json_object_serializer::serialize(const char *id, belongs_to<Value> &x, cascade_type)
{
  if (x.empty()) {
    return;
  }
  write_id(id);
  append(x);
}

template<class Value>
void json_object_serializer::serialize(const char *id, has_one<Value> &x, cascade_type)
{
  if (x.empty()) {
    return;
  }
  write_id(id);
  append(x);
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
