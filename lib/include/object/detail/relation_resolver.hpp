//
// Created by sascha on 11/25/15.
//

#ifndef OOS_RELATION_RESOLVER_HPP
#define OOS_RELATION_RESOLVER_HPP

#include "object/access.hpp"

#include <string>
#include <list>

namespace oos {

class prototype_node;
class object_container;
class object_base_ptr;

namespace detail {

class relation_resolver
{
public:
  typedef std::list<std::string> string_list_t;
  typedef string_list_t::const_iterator const_iterator;

public:
  relation_resolver(prototype_node &node)
    : node_(node)
  {}
  virtual ~relation_resolver() {}

  template < class T >
  static void build(prototype_node &node)
  {
    relation_resolver resolver(node);
    resolver.build<T>();
  }

  template < class T >
  void build()
  {
    T obj;
    oos::access::serialize(*this, obj);
  }

  template < class V >
  void serialize(const V &x)
  {
    oos::access::serialize(*this, x);
  }

  template < class V >
  void serialize(const char*, const V&) {}
  void serialize(const char*, const char*, int) {}
  void serialize(const char *id, const object_container &x);
  void serialize(const char *, const object_base_ptr &x);

private:
  prototype_node &node_;
};

}
}

#endif //OOS_RELATION_RESOLVER_HPP
