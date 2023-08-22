#ifndef MATADOR_RELATION_ENDPOINT_VALUE_REMOVER_HPP
#define MATADOR_RELATION_ENDPOINT_VALUE_REMOVER_HPP

#include "matador/object/has_many_item_holder.hpp"
#include "matador/object/object_proxy_accessor.hpp"

#include "matador/utils/field_attributes.hpp"

#include <iostream>

namespace matador {

/// @cond MATADOR_DEV

template < class V, template < class ... > class C >
class has_many;

class object_holder;

template < class T >
class object_ptr;

namespace detail {

template < class Value >
class relation_endpoint_value_remover : object_proxy_accessor
{
public:
  template < class Owner >
  void remove(const object_ptr<Owner> &owner, const std::string &field, has_many_item_holder<Value> holder);

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class T >
  void on_primary_key(const char *, T &, const field_attributes &/*attr*/ = null_attributes) {}
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
  void on_has_many(const char *id, has_many<Value, Container> &x, cascade_type)
  {
    if (field_ != id) {
      return;
    }
    x.remove_holder(holder_);
  }

  template < template < class ... > class Container >
  void on_has_many(const char *id, has_many<Value, Container> &x, const char*, const char*, cascade_type cascade)
  {
    on_has_many(id, x, cascade);
  }
  template < class T, template < class ... > class Container >
  void on_has_many(const char *, has_many<T, Container> &, cascade_type) {}
  template < class T, template < class ... > class Container >
  void on_has_many(const char *, has_many<T, Container> &, const char*, const char*, cascade_type) {}

private:
  std::string field_;
  has_many_item_holder<Value> holder_;
};

/// @endcond

template < class Value >
template<class Owner>
void relation_endpoint_value_remover<Value>::remove(const object_ptr <Owner> &owner, const std::string &field, has_many_item_holder<Value> holder)
{
  field_ = field;
  holder_ = holder;

  matador::access::serialize(*this, *owner);

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
