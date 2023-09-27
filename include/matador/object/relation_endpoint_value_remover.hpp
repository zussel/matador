#ifndef MATADOR_RELATION_ENDPOINT_VALUE_REMOVER_HPP
#define MATADOR_RELATION_ENDPOINT_VALUE_REMOVER_HPP

#include "matador/object/container_item_holder.hpp"
#include "matador/object/object_proxy_accessor.hpp"

#include "matador/utils/field_attributes.hpp"

#include <iostream>

namespace matador {

/// @cond MATADOR_DEV

template < class Type, template < class ... > class ContainerType>
class container;

class object_holder;

template < class T >
class object_ptr;

namespace detail {

template < class Value >
class relation_endpoint_value_remover : object_proxy_accessor
{
public:
  template < class Owner >
  void remove(const object_ptr<Owner> &owner, const std::string &field, container_item_holder<Value> holder);

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
    x.remove_holder(holder_);
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

template < class Value >
template<class Owner>
void relation_endpoint_value_remover<Value>::remove(const object_ptr <Owner> &owner, const std::string &field, container_item_holder<Value> holder)
{
  field_ = field;
  holder_ = holder;

  matador::access::process(*this, *owner);

  field_.clear();
}

template < class Value >
void relation_endpoint_value_remover<Value>::on_belongs_to(const char *id,
                                                           object_ptr<Value> &x,
                                                           cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(nullptr, cascade, false);
}

template < class Value >
void relation_endpoint_value_remover<Value>::on_has_one(const char *id,
                                                        object_ptr<Value> &x,
                                                        cascade_type cascade)
{
  if (field_ != id) {
    return;
  }
  x.reset(nullptr, cascade, false);
}

}
}
#endif //MATADOR_RELATION_ENDPOINT_VALUE_REMOVER_HPP
