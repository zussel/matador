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


object_proxy* object_container::owner() const
{
  return owner_;
}

void object_container::owner(object_proxy *ownr)
{
  owner_ = ownr;
}

void object_container::handle_container_item(prototype_tree &ptree, const char *id, prototype_node *node) const
{
  prototype_node *item_node = 0;
  object_base_producer *p = create_item_producer();
  if (p) {
    ptree.insert(p, id);
    item_node = ptree.find_prototype_node(id);
    if (!item_node) {
      throw object_exception("unknown prototype type");
    }
  } else {
    // insert new prototype
    // get prototype node of container item (child)
    item_node = ptree.find_prototype_node(classname());
    if (!item_node) {
      // if there is no such prototype node
      // insert a new one (it is automatically marked
      // as uninitialized)
      item_node = new prototype_node();
      ptree.typeid_prototype_map_.insert(std::make_pair(classname(), prototype_tree::t_prototype_map()));
      ptree.prototype_map_[classname()] = item_node;
    }
  }
  // add container node to item node
  // insert the relation
  item_node->relations.insert(std::make_pair(node->type, std::make_pair(node, id)));
}

}
