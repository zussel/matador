#ifndef MATADOR_OBJECT_TO_JSON_TABLE_CONVERTER_HPP
#define MATADOR_OBJECT_TO_JSON_TABLE_CONVERTER_HPP

#include "matador/object/object_ptr.hpp"
#include "matador/object/object_view.hpp"
#include "matador/object/object_json_attribute_extractor.hpp"

#include <matador/json/json.hpp>

class object_to_json_table_converter
{
public:
  template<typename Type>
  matador::json to_json_table(const matador::object_ptr<Type> &obj)
  {
    result_ = matador::json::object();
    matador::access::process(*this, *obj);
    return result_;
  }

  template< typename T >
  matador::json to_json_table(const matador::object_view<T> &objects)
  {
    auto result = matador::json::array();

    for (const auto &obj : objects) {
      result_ = matador::json::array();
      matador::access::process(*this, *obj.get());
      result.push_back(result_);
    }

    return result;
  }

  template<typename Type>
  [[nodiscard]] matador::json create_value_information(const Type &value, const std::string &type_name) const {
    return {
      { "type", type_name },
      { "value", value }
    };
  }

  template< class V >
  void on_primary_key(const char * /*id*/, V &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    result_.push_back(create_value_information(pk, "PRIMARY_KEY"));
  }
  void on_primary_key(const char *id, std::string &pk, size_t /*size*/);

  void on_revision(const char * /*id*/, unsigned long long &rev)
  {
    result_.push_back(create_value_information(rev, "INTEGER"));
  }

  template < class V >
  void on_attribute(const char * /*id*/, V &val, const matador::field_attributes &/*attr*/ = matador::null_attributes, typename std::enable_if<std::is_integral<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    result_.push_back(create_value_information(val, "INTEGER"));
  }

  template < class V >
  void on_attribute(const char * /*id*/, V &val, const matador::field_attributes &/*attr*/ = matador::null_attributes, typename std::enable_if<std::is_floating_point<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    result_.push_back(create_value_information(val, "REAL"));
  }

  void on_attribute(const char *id, bool &to, const matador::field_attributes &/*attr*/ = matador::null_attributes);
  void on_attribute(const char *id, std::string &to, const matador::field_attributes &/*attr*/ = matador::null_attributes);
  void on_attribute(const char *id, matador::date &to, const matador::field_attributes &/*attr*/ = matador::null_attributes);
  void on_attribute(const char *id, matador::time &to, const matador::field_attributes &/*attr*/ = matador::null_attributes);

  template<class V>
  void on_belongs_to(const char * /*id*/, matador::object_ptr<V> &x, const matador::foreign_attributes &/*attr*/ = matador::default_foreign_attributes)
  {
    if (!x.empty()) {
      result_.push_back(create_value_information(extractor_.extract(x, { "id", "name"}), "ENTITY"));
    }
  }
  template<class V>
  void on_has_one(const char * /*id*/, matador::object_ptr<V> &x, const matador::foreign_attributes &/*attr*/ = matador::default_foreign_attributes)
  {
    if (!x.empty()) {
      result_.push_back(create_value_information(extractor_.extract(x, { "id", "name"}), "ENTITY"));
    }
  }

  template < class V, template <class ...> class Container >
  void on_has_many(const char *id, matador::container<V, Container> &x, const char * /*join_column*/, const matador::foreign_attributes &/*attr*/ = matador::default_foreign_attributes)
  {
    handle_has_many(id, x);
  }

  template < class V, template <class ...> class Container >
  void on_has_many(const char *id, matador::container<V, Container> &x, const matador::foreign_attributes &/*attr*/ = matador::default_foreign_attributes)
  {
    handle_has_many(id, x);
  }

  template < class V, template <class ...> class Container >
  void on_has_many_to_many(const char *id, matador::container<V, Container> &x, const char *, const char *, const matador::foreign_attributes &/*attr*/ = matador::default_foreign_attributes)
  {
    handle_has_many(id, x);
  }

  template < class V, template <class ...> class Container >
  void on_has_many_to_many(const char *id, matador::container<V, Container> &x, const matador::foreign_attributes &/*attr*/ = matador::default_foreign_attributes)
  {
    handle_has_many(id, x);
  }

private:
  template < class V, template <class ...> class Container >
  void handle_has_many(const char *id, matador::container<V, Container> &x)
  {
    auto array = matador::json::array();
    for (const auto &obj : x) {
      array.push_back(create_value_information(extractor_.extract(obj, { "id", "name"}), "ENTITY"));
    }
    result_.push_back(create_value_information(array, "COLLECTION"));
  }

private:
  matador::object_json_attribute_extractor extractor_;
  matador::json result_;
};


#endif //MATADOR_OBJECT_TO_JSON_TABLE_CONVERTER_HPP
