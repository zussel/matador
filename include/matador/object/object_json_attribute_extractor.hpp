#ifndef MATADOR_OBJECT_JSON_ATTRIBUTE_EXTRACTOR_HPP
#define MATADOR_OBJECT_JSON_ATTRIBUTE_EXTRACTOR_HPP

#include "matador/object/export.hpp"

#include "matador/json/json.hpp"

#include "matador/utils/field_attributes.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/container.hpp"

namespace matador {

class MATADOR_OBJECT_API object_json_attribute_extractor
{
public:
  template< typename T >
  json extract(const object_ptr<T> &obj, const std::set<std::string> &attributes)
  {
    attributes_ = attributes;
    result_ = json::object();
    matador::access::process(*this, *obj);
    return result_;
  }

  template< class V >
  void on_primary_key(const char *id, V &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    if (attributes_.count(id) > 0) {
      result_[id] = pk;
    }
  }
  void on_primary_key(const char *id, std::string &pk, size_t /*size*/);

  void on_revision(const char *id, unsigned long long &rev)
  {
    if (attributes_.count(id) > 0) {
      result_[id] = rev;
    }
  }

//  template < class V >
//  void on_attribute(const char *id, V &, const utils::field_attributes &/*attr*/ = utils::null_attributes, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
//  {
//    result_[id] = json::object();
//  }

  template < class V >
  void on_attribute(const char *id, V &val, const utils::field_attributes &/*attr*/ = utils::null_attributes, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    if (attributes_.count(id) > 0) {
      result_[id] = val;
    }
  }

  void on_attribute(const char *id, bool &to, const utils::field_attributes &/*attr*/ = utils::null_attributes);
  void on_attribute(const char *id, std::string &to, const utils::field_attributes &/*attr*/ = utils::null_attributes);
  void on_attribute(const char *id, date &to, const utils::field_attributes &/*attr*/ = utils::null_attributes);
  void on_attribute(const char *id, time &to, const utils::field_attributes &/*attr*/ = utils::null_attributes);

  template<class V>
  void on_belongs_to(const char *id, object_ptr<V> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
//    if (!x.empty()) {
//    }
  }
  template<class V>
  void on_has_one(const char *id, object_ptr<V> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
//    if (!x.empty()) {
//    }
  }

  template < class V,
  template <class ...> class Container >
  void on_has_many(const char *id, container<V, Container> &x, const char * /*join_column*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    handle_has_many(id, x);
  }

  template < class V,
  template <class ...> class Container >
  void on_has_many(const char *id, container<V, Container> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    handle_has_many(id, x);
  }

  template < class V,
  template <class ...> class Container >
  void on_has_many_to_many(const char *id, container<V, Container> &x, const char *, const char *, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    handle_has_many(id, x);
  }

  template < class V,
  template <class ...> class Container >
  void on_has_many_to_many(const char *id, container<V, Container> &x, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    handle_has_many(id, x);
  }

private:
  template < class V,
  template <class ...> class Container >
  void handle_has_many(const char *id, container<V, Container> &x)
  {
//    json array = json::array();
//    for (const auto &obj: x) {
//      array.push_back(ojs.to_json(obj));
//    }
//    result_[id] = array;
  }

private:
  std::set<std::string> attributes_;
  json result_;
};

}

#endif //MATADOR_OBJECT_JSON_ATTRIBUTE_EXTRACTOR_HPP
