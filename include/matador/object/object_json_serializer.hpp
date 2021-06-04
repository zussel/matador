#ifndef MATADOR_OBJECT_JSON_SERIALIZER_HPP
#define MATADOR_OBJECT_JSON_SERIALIZER_HPP

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

#include "matador/utils/json.hpp"
#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/has_many.hpp"
#include "matador/object/has_one.hpp"
#include "matador/object/belongs_to.hpp"

#include <string>

namespace matador {

class MATADOR_OBJECT_API object_json_serializer
{
public:
  template< typename T, object_holder_type OPT >
  json to_json(const matador::object_pointer<T, OPT> &obj)
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

  template< class V >
  void serialize(const char *id, identifier<V> &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    if (pk.is_valid()) {
      result_[id] = pk.value();
    }
  }

  template < class V >
  void serialize(const char *id, V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    result_[id] = json::object();
  }

  template < class V >
  void serialize(const char *id, V &val, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    result_[id] = val;
  }

  void serialize(const char *id, identifier<std::string> &pk);
  void serialize(const char *id, bool &to);
  void serialize(const char *id, std::string &to);
  void serialize(const char *id, std::string &to, size_t);
  void serialize(const char *id, date &to);
  void serialize(const char *id, time &to);

  template<class V>
  void serialize(const char *id, belongs_to<V> &x, cascade_type)
  {
    if (!x.empty()) {
      result_[id] = json::object();
    }
  }
  template<class V>
  void serialize(const char *id, has_one<V> &x, cascade_type)
  {
    if (!x.empty()) {
      object_json_serializer ojs;
      result_[id] = ojs.to_json(x);
    }
  }

  template < class V, template <class ...> class Container >
  void serialize(const char *id, basic_has_many<V, Container> &x, const char *, const char *, cascade_type)
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
#endif //MATADOR_OBJECT_JSON_SERIALIZER_HPP
