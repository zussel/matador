#ifndef MATADOR_RELATION_ENDPOINT_VALIDATOR_HPP
#define MATADOR_RELATION_ENDPOINT_VALIDATOR_HPP

#include "matador/object/relation_endpoint.hpp"

namespace matador::detail {

template < typename Type >
class relation_endpoint_validator
{
public:
  static bool has_belongs_to(const char *column_name) {
    relation_endpoint_validator validator(column_name, detail::basic_relation_endpoint::BELONGS_TO);

    return validator.validate();
  }

  static bool has_has_many(const char *column_name) {
    relation_endpoint_validator validator(column_name, detail::basic_relation_endpoint::HAS_MANY);

    return validator.validate();
  }

  template<class V>
  void on_primary_key(const char *, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template<class V>
  void on_attribute(const char *, V &, const utils::field_attributes &/*attr*/ = utils::null_attributes) { }
  void on_attribute(const char *, char *, const utils::field_attributes &/*attr*/ = utils::null_attributes) { }
  void on_attribute(const char *, std::string &, const utils::field_attributes &/*attr*/ = utils::null_attributes) { }

  template<class Value>
  void on_belongs_to(const char *id, object_ptr<Value> &/*x*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes)
  {
    if (!is_valid_ && relation_type_ == basic_relation_endpoint::BELONGS_TO) {
      is_valid_ = column_name_ == id;
    }
  }
  template<class Value>
  void on_has_one(const char * /*id*/, object_ptr<Value> &/*x*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  template<class Value, template<class ...> class Container>
  void on_has_many(const char * id, container<Value, Container> &, const char *, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes )
  {
    if (!is_valid_ && relation_type_ == basic_relation_endpoint::HAS_MANY) {
      is_valid_ = column_name_ == id;
    }
  }
  template<class Value, template<class ...> class Container>
  void on_has_many(const char * id, container<Value, Container> &, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes )
  {
    if (!is_valid_ && relation_type_ == basic_relation_endpoint::HAS_MANY) {
      is_valid_ = column_name_ == id;
    }
  }
  template<class Value, template<class ...> class Container>
  void on_has_many_to_many(const char * /*id*/, container<Value, Container> &/*x*/, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}
  template<class Value, template<class ...> class Container>
  void on_has_many_to_many(const char *, container<Value, Container> &, const char *, const char *, const utils::foreign_attributes &/*attr*/ = utils::default_foreign_attributes) {}

private:
  explicit relation_endpoint_validator(const char* column_name,
                                       detail::basic_relation_endpoint::relation_type relation_type)
  : column_name_(column_name)
  , relation_type_(relation_type) {}

  bool validate() {
    Type obj;
    access::process(*this, obj);

    return is_valid_;
  }

private:
  const std::string column_name_;
  const detail::basic_relation_endpoint::relation_type relation_type_;

  bool is_valid_{false};
};


}

#endif //MATADOR_RELATION_ENDPOINT_VALIDATOR_HPP
