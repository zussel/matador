#ifndef MATADOR_JSON_OBJECT_SERIALIZER_HPP
#define MATADOR_JSON_OBJECT_SERIALIZER_HPP

#include "matador/object/export.hpp"

#include "matador/json/json_serializer.hpp"
#include "matador/json/json_identifier_serializer.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/container.hpp"

/// @cond MATADOR_DEV

namespace matador {

class MATADOR_OBJECT_API json_object_serializer
{
public:
  json_object_serializer() = default;

  using id_pk_ref = std::reference_wrapper<const identifier>;
  using t_id_set = std::unordered_set<id_pk_ref , id_pk_hash, std::equal_to<const identifier>>;
  using t_name_id_set_pair = std::pair<std::string, t_id_set>;
  using t_type_id_map = std::unordered_map<std::type_index, t_name_id_set_pair>;

  t_type_id_map type_id_map_;

  template< typename T >
  std::string to_json_string(const object_ptr<T> &obj, json_format format = json_format::compact)
  {
    type_id_map_.clear();
    format_ = format;
    json_.clear();
    append(const_cast<object_ptr<T>&>(obj));
    newline();
    return json_;
  }

  template< typename T >
  std::string to_json_string(const object_view<T> &objects, json_format format = json_format::compact)
  {
    type_id_map_.clear();
    format_ = format;
    json result = json::array();

    json_.append("[");
    if (objects.size() < 2) {
      for (const auto &i : objects) {
        append(i);
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
    matador::access::process(*this, obj);
  }

  template< class V >
  void on_primary_key(const char *id, V &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    write_id(id);

    auto it = type_id_map_.find(*current_type_index_);
    if (it != type_id_map_.end() && it->second.first.empty()) {
      it->second.first.assign(id);
    }
    append(pk);
    json_.append(",");

    newline();
  }
  void on_primary_key(const char *id, std::string &pk, size_t /*size*/);
  void on_revision(const char *id, unsigned long long &rev);

  template < class V >
  void on_attribute(const char *id, V &obj, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<!matador::is_builtin<V>::value>::type* = nullptr)
  {
    write_id(id);
    append(obj);
    newline();
  }

  template < class V >
  void on_attribute(const char *id, V &val, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    write_id(id);
    append(val);
    json_.append(",");
    newline();
  }

  void on_attribute(const char *id, bool &val, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, std::string &val, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, const char *val, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, char val[], const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, date &val, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, time &val, const field_attributes &/*attr*/ = null_attributes);

  template<class Value>
  void on_belongs_to(const char *id, object_ptr<Value> &x, cascade_type);
  template<class Value>
  void on_has_one(const char *id, object_ptr<Value> &x, cascade_type);
  template < class Value, template <class ...> class Container >
  void on_has_many(const char *id, container<Value, Container> &c, const char *, const char *, cascade_type);

private:
  void write_id(const char *id);
  void begin_object();
  void end_object();
  void begin_array();
  void end_array();
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

  template< typename T >
  void append(const object_ptr<T> &x)
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
        json_.append(identifier_serializer_.serialize(x.primary_key()));
        end_object();
        newline();
      }
    } else {
      auto ret = type_id_map_.insert(std::make_pair(tindex, t_name_id_set_pair("", t_id_set())));
      current_type_index_ = &ret.first->first;
      ret.first->second.second.insert(x.primary_key());
      append(*x);
      current_type_index_ = nullptr;
    }
  }

  template< typename T >
  void append(object_ptr<T> &x)
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
        json_.append(identifier_serializer_.serialize(x.primary_key()));
        end_object();
        newline();
        //x.primary_key();
      }
    } else {
      auto ret = type_id_map_.insert(std::make_pair(tindex, t_name_id_set_pair("", t_id_set())));
      current_type_index_ = &ret.first->first;
      auto pk = x.primary_key();
      ret.first->second.second.insert(pk);
      append(*x);
      current_type_index_ = nullptr;
    }
  }

  template < class V >
  void append(V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    begin_object();
    matador::access::process(*this, obj);
    auto idx = json_.find_last_of(',');
    json_.erase(idx, 1);
    end_object();
    newline();
  }

private:
  std::string json_;
  json_format format_ {};
  unsigned depth_ = 0;

  bool is_array = false;

  const std::type_index *current_type_index_ = nullptr;

  json_identifier_serializer identifier_serializer_;
};

template<class Value>
void json_object_serializer::on_belongs_to(const char *id, object_ptr<Value> &x, cascade_type)
{
  if (x.empty()) {
    return;
  }
  write_id(id);
  append(x);
}

template<class Value>
void json_object_serializer::on_has_one(const char *id, object_ptr<Value> &x, cascade_type)
{
  if (x.empty()) {
    return;
  }
  write_id(id);
  append(x);
}

template<class Value, template <class ...> class Container>
void json_object_serializer::on_has_many(const char *id, container<Value, Container> &c, const char *,
                                         const char *, cascade_type)
{
  write_id(id);
  begin_array();
  if (c.size() < 2) {
    for (const auto &obj : c) {
      append(obj);
    }
  } else {
    auto it = c.begin();
    append(*it++);
    for (;it != c.end(); ++it) {
      json_.append(",");
      append(*it);
    }
  }
  end_array();
  newline();
}

}

/// @endcond

#endif //MATADOR_JSON_OBJECT_SERIALIZER_HPP
