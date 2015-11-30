//
// Created by sascha on 11/25/15.
//

#ifndef OOS_RELATION_RESOLVER_HPP
#define OOS_RELATION_RESOLVER_HPP

#include "object/access.hpp"
#include <object/prototype_iterator.hpp>
#include <object/prototype_tree.hpp>
#include <object/object_ptr.hpp>

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

  template < class T, bool TYPE >
  void serialize(const char *, const object_holder<T, TYPE> &x)
  {
    prototype_iterator pi = node_.tree()->find(x.type());
    if (pi == node_.tree()->end()) {
      // if there is no such prototype node
      // insert a new one (it is automatically marked
      // as uninitialized)
      node_.tree()->attach<T>(x.type());
    }
  }

private:
  prototype_node &node_;
};

}
}

#endif //OOS_RELATION_RESOLVER_HPP
