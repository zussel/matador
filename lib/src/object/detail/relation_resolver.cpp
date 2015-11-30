//
// Created by sascha on 11/25/15.
//

#include "object/detail/relation_resolver.hpp"

#include "object/prototype_tree.hpp"

namespace oos {
namespace detail {

//void relation_resolver::serialize(const char *id, const object_container &x)
//{
//   //get item type of the container
//   //try to insert it as prototype
//  prototype_iterator pi;
//  object_base_producer *p = x.create_item_producer();
//  if (p) {
//    pi = node_.tree->insert(p, id);
//    if (pi == node_.tree->end()) {
//      throw object_exception("unknown prototype type");
//    }
//  } else {
//    // insert new prototype
//    // get prototype node of container item (child)
//    pi = node_.tree->find(x.classname());
//    if (pi == node_.tree->end()) {
//      // if there is no such prototype node
//      // insert a new one (it is automatically marked
//      // as uninitialized)
//      pi = prototype_iterator(node_.tree->prepare_insert(x.classname()));
//    }
//  }
//  // add container node to item node
//  // insert the relation
//  pi->relations.insert(std::make_pair(node_.type, std::make_pair(&node_, id)));
//}

}
}
