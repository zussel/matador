/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "object/object_container.hpp"

using namespace std;

namespace oos {


object_proxy* object_container::proxy(const object_base_ptr &optr) const
{
  return optr.proxy_;
}

object_proxy* object_container::owner() const
{
  return owner_;
}

void object_container::owner(object_proxy *ownr)
{
  owner_ = ownr;
}

void object_container::handle_container_item(const char *id, prototype_node &node) const
{
  prototype_iterator pi;
  object_base_producer *p = create_item_producer();
  if (p) {
    pi = node.tree->insert(p, id);
    if (pi == node.tree->end()) {
      throw object_exception("unknown prototype type");
    }
  } else {
    // insert new prototype
    // get prototype node of container item (child)
    pi = node.tree->find(classname());
    if (pi == node.tree->end()) {
      // if there is no such prototype node
      // insert a new one (it is automatically marked
      // as uninitialized)
      pi = prototype_iterator(new prototype_node());
      node.tree->typeid_prototype_map_.insert(std::make_pair(classname(), prototype_tree::t_prototype_map()));
      node.tree->prototype_map_[classname()] = pi.get();
    }
  }
  // add container node to item node
  // insert the relation
  pi->relations.insert(std::make_pair(node.type, std::make_pair(&node, id)));
}

}
