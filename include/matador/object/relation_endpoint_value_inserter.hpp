//
// Created by sascha on 6/2/17.
//

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

template < class T, object_holder_type OHT >
class object_pointer;

namespace detail {

template < class Value >
class relation_endpoint_value_inserter : object_proxy_accessor
{
public:
  template < class Owner >
  void insert(const object_pointer<Owner, object_holder_type::OBJECT_PTR> &owner, const std::string &field, has_many_item_holder<Value> holder);

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class T >
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) {}
  void serialize(const char *, std::string &, size_t) {}
  template < class T >
  void serialize(const char *, T &, cascade_type) {}
  void serialize(const char *id, object_pointer<Value, object_holder_type::BELONGS_TO> &x, cascade_type);
  void serialize(const char *id, object_pointer<Value, object_holder_type::HAS_ONE> &x, cascade_type);
  template < template < class ... > class Container >
  void serialize(const char *id, has_many<Value, Container> &x, cascade_type cascade);

  template < template < class ... > class Container >
  void serialize(const char *id, has_many<Value, Container> &x, const char*, const char*, cascade_type cascade)
  {
    serialize(id, x, cascade);
  }
  template < class T, template < class ... > class Container >
  void serialize(const char *, has_many<T, Container> &, cascade_type) {}
  template < class T, template < class ... > class Container >
  void serialize(const char *, has_many<T, Container> &, const char*, const char*, cascade_type) {}

private:
  std::string field_;
  has_many_item_holder<Value> holder_;
};

/// @endcond

}
}
#endif //MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP
