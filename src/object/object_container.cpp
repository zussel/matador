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

void object_container::handle_container_item(object_store &ostore, const char *id, prototype_node *node) const
{
  prototype_node *item_node = 0;
  object_base_producer *p = create_item_producer();
  if (p) {
    ostore.insert_prototype(p, id);
    item_node = ostore.get_prototype(id);
  } else {
    
    // insert new prototype
    //ostore.insert_prototype(p, id);
    // get prototype node of container item (child)
    item_node = ostore.get_prototype(classname());
    if (!item_node) {
      // if there is no such prototype node
      // insert a new one (it is automatically marked
      // as uninitialzed)
      item_node = new prototype_node();
      ostore.typeid_prototype_map_.insert(std::make_pair(classname(), object_store::t_prototype_node_map()));
//      std::cout << "DEBUG: inserting unfinished node [" << classname() << "] in advance\n";
      ostore.prototype_node_map_[classname()] = item_node;
    }
  }
  // add container node to item node
//  std::cout << "DEBUG: storing relation info [" << classname() << "(item_node: " << item_node << ")] storing relation: [" << node->type << "][prototype: " << node << "(" << node->type << ")]\n";
  
  // insert the relation
  item_node->relations.insert(std::make_pair(node->type, std::make_pair(node, id)));
  
  // insert an id proxy list for this field (id)
  node->relation_data.insert(std::make_pair(id, prototype_node::object_map_t()));
}

}
