//
// Created by sascha on 6/2/17.
//

#ifndef MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP
#define MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP

#include <iostream>

namespace matador {

template < class V, template < class ... > class C >
class has_many;

namespace detail {

template < class Value >
class relation_endpoint_value_inserter
{
public:
  template < class Owner >
  void insert(const object_ptr<Owner> &owner, const std::string &field, object_proxy *value, object_proxy *item_proxy);

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class T >
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) {}
  void serialize(const char *id, object_holder &x, cascade_type);
  template < template < class ... > class Container >
  void serialize(const char *id, has_many<Value, Container> &, const char*, const char*);

private:
  std::string field_;
  object_proxy *value_;
  object_proxy *item_proxy_ = nullptr; // only set if holder type is HAS_MANY and foreign type is also HAS_MANY
};

}
}
#endif //MATADOR_RELATION_ENDPOINT_VALUE_INSERTER_HPP
