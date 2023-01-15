#ifndef MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP
#define MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP

#include "matador/object/has_many_item_holder.hpp"
#include "matador/object/object_proxy_accessor.hpp"

#include "matador/utils/cascade_type.hpp"

#include "matador/utils/access.hpp"

#include <iostream>

namespace matador {

/// @cond MATADOR_DEV

template < class V, template < class ... > class C >
class has_many;

class object_holder;

namespace detail {

template < class Value >
class relation_endpoint_value_inserter : object_proxy_accessor
{
public:
  template < class Owner >
  void insert(const object_ptr<Owner> &owner, const std::string &field, has_many_item_holder<Value> holder);

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  void on_primary_key(const char *, basic_identifier &) {}
  template < class T >
  void on_attribute(const char *, T &) {}
  void on_attribute(const char *, char *, size_t) {}
  void on_attribute(const char *, std::string &, size_t) {}
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
    x.insert_holder(holder_);
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

template<class Value>
template<class Owner>
void relation_endpoint_value_inserter<Value>::insert(const object_ptr <Owner> &owner, const std::string &field,
                                                     has_many_item_holder <Value> holder)
{
  field_ = field;
  holder_ = std::move(holder);

  matador::access::serialize(*this, *owner);

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
