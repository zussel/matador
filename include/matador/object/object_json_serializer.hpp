#ifndef MATADOR_OBJECT_JSON_SERIALIZER_HPP
#define MATADOR_OBJECT_JSON_SERIALIZER_HPP

#include "matador/object/export.hpp"

#include "matador/json/json.hpp"

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/has_many.hpp"

#include <string>

/// @cond MATADOR_DEV

namespace matador {

class MATADOR_OBJECT_API object_json_serializer
{
public:
  template< typename T >
  json to_json(const matador::object_ptr<T> &obj)
  {
    result_ = json::object();
    matador::access::serialize(*this, *obj);
    return result_;
  }

  template< typename T >
  json to_json(const matador::object_view<T> &objects)
  {
    json result = json::array();

    for (const auto &obj : objects) {
      result_ = json::object();
      matador::access::serialize(*this, *obj.get());
      result.push_back(result_);
    }

    return result;
  }

  void on_primary_key(const char *id, std::string &pk, const field_attributes &/*attr*/ = null_attributes);
  template< class V >
  void on_primary_key(const char *id, V &pk, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    result_[id] = pk;
  }

  template < class V >
  void on_attribute(const char *id, V &, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    result_[id] = json::object();
  }

  template < class V >
  void on_attribute(const char *id, V &val, const field_attributes &/*attr*/ = null_attributes, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    result_[id] = val;
  }

  void on_attribute(const char *id, bool &to, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, std::string &to, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, date &to, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, time &to, const field_attributes &/*attr*/ = null_attributes);

  template<class V>
  void on_belongs_to(const char *id, object_ptr<V> &x, cascade_type)
  {
    if (!x.empty()) {
      result_[id] = json::object();
    }
  }
  template<class V>
  void on_has_one(const char *id, object_ptr<V> &x, cascade_type)
  {
    if (!x.empty()) {
      object_json_serializer ojs;
      result_[id] = ojs.to_json(x);
    }
  }

  template < class V, template <class ...> class Container >
  void on_has_many(const char *id, basic_has_many<V, Container> &x, const char *, const char *, cascade_type)
  {
    json array = json::array();
    object_json_serializer ojs;
    for (const auto &obj : x) {
      array.push_back(ojs.to_json(obj));
    }
    result_[id] = array;
  }

private:
  json result_;
};

}

/// @endcond

#endif //MATADOR_OBJECT_JSON_SERIALIZER_HPP
