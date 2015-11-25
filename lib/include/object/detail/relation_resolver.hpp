//
// Created by sascha on 11/25/15.
//

#ifndef OOS_RELATION_RESOLVER_HPP
#define OOS_RELATION_RESOLVER_HPP

#include <string>

#include "object/prototype_tree.hpp"

namespace oos {
namespace detail {

template < class T >
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

  static void build(prototype_node &node)
  {
    relation_resolver<T> resolver(node);
    resolver.build();
  }
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

  void serialize(const char *id, const object_container &x)
  {
    /*
     * get item type of the container
     * try to insert it as prototype
     */
    prototype_iterator pi;
    object_base_producer *p = x.create_item_producer();
    if (p) {
      pi = node_.tree->insert(p, id);
      if (pi == node_.tree->end()) {
        throw object_exception("unknown prototype type");
      }
    } else {
      // insert new prototype
      // get prototype node of container item (child)
      pi = node_.tree->find(x.classname());
      if (pi == node_.tree->end()) {
        // if there is no such prototype node
        // insert a new one (it is automatically marked
        // as uninitialized)
        pi = prototype_iterator(node_.tree->prepare_insert(x.classname()));
      }
    }
    // add container node to item node
    // insert the relation
    pi->relations.insert(std::make_pair(node_.type, std::make_pair(&node_, id)));
  }

  void serialize(const char *, const object_base_ptr &x)
  {
    prototype_iterator pi = node_.tree->find(x.type());
    if (pi == node_.tree->end()) {
      // if there is no such prototype node
      // insert a new one (it is automatically marked
      // as uninitialized)
      node_.tree->prepare_insert(x.type());
    }
  }

private:
  prototype_node &node_;
};

}
}

#endif //OOS_RELATION_RESOLVER_HPP
