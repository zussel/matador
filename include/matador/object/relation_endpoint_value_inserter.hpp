#ifndef MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP
#define MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP

#include "matador/object/container_item_holder.hpp"
#include "matador/object/object_proxy_accessor.hpp"

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/access.hpp"

#include <iostream>

namespace matador {

/// @cond MATADOR_DEV

template < class V, template < class ... > class C >
class container;

class object_holder;

namespace detail {

template < class Value >
class relation_endpoint_value_inserter : object_proxy_accessor
{
public:
  template < class Owner >
  void insert(const object_ptr<Owner> &owner, const std::string &field, container_item_holder<Value> holder);

  template < class T >
  void serialize(T &x)
  {
    matador::access::process(*this, x);
  }

  template < class T >
  void on_primary_key(const char *, T &, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type* = 0) {}
  void on_primary_key(const char *, std::string &, size_t /*size*/) {}
  void on_revision(const char *, unsigned long long &/*rev*/) {}
  template < class T >
  void on_attribute(const char *, T &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, char *, const field_attributes &/*attr*/ = null_attributes) {}
  void on_attribute(const char *, std::string &, const field_attributes &/*attr*/ = null_attributes) {}
  void on_belongs_to(const char *id, object_ptr<Value> &x, cascade_type);
  template < class T >
  void on_belongs_to(const char *, T &, cascade_type) {}
  void on_has_one(const char *id, object_ptr<Value> &x, cascade_type);
  template < class T >
  void on_has_one(const char *, T &, cascade_type) {}
  template < template < class ... > class Container >
  void on_has_many(const char *id, container<Value, Container> &x, cascade_type)
  {
    if (field_ != id) {
      return;
    }
    x.insert_holder(holder_);
  }


  template < template < class ... > class Container >
  void on_has_many(const char *id, container<Value, Container> &x, const char*, const char*, cascade_type cascade)
  {
    on_has_many(id, x, cascade);
  }
  template < class T, template < class ... > class Container >
  void on_has_many(const char *, container<T, Container> &, cascade_type) {}
  template < class T, template < class ... > class Container >
  void on_has_many(const char *, container<T, Container> &, const char*, const char*, cascade_type) {}

private:
  std::string field_;
  container_item_holder<Value> holder_;
};

/// @endcond

template<class Value>
template<class Owner>
void relation_endpoint_value_inserter<Value>::insert(const object_ptr <Owner> &owner, const std::string &field,
                                                     container_item_holder <Value> holder)
{
  field_ = field;
  holder_ = std::move(holder);

  matador::access::process(*this, *owner);

  field_.clear();
}

template<class Value>
void relation_endpoint_value_inserter<Value>::on_belongs_to(const char *id,
                                                            object_ptr <Value> &x,
                                                            cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(proxy(holder_.value()), cascade, false);
}

template<class Value>
void relation_endpoint_value_inserter<Value>::on_has_one(const char *id,
                                                         object_ptr <Value> &x,
                                                         cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(proxy(holder_.value()), cascade, false);
}

}
}
#endif //MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP
