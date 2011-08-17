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

#ifndef PROTOTYPE_NODE_HPP
#define PROTOTYPE_NODE_HPP

#include <iostream>
#include <memory>

namespace oos {

class object_base_producer;
class object_proxy;

struct prototype_node {
  prototype_node();
  prototype_node(object_base_producer *p, const char *t);
  ~prototype_node();

  /**
   * clear removes all object proxies really belonging to
   * this node and deletes them and the holded object
   */
  void clear();

  /**
   * Returns true if object proxy list is empty. If self is true, only
   * list of own objects is checked. If self is false, complete list is
   * checked.
   */
  bool empty(bool self) const;
  
  /**
   * Returns the size of the object proxy list.
   */
  unsigned long size() const;
  
  /**
   * Appends the given prototype node to the list of children.
   */

  void insert(prototype_node *child);
    
  /**
   * Removes node and cildren nodes from list, clears all object
   * and deletes all members.
   */
  void remove();

  /**
   * Unlinks node from list.
   */
  void unlink();

  /**
   * Returns nodes successor node or NULL if node is last.
   */
  prototype_node* next_node() const;
  prototype_node* next_node(prototype_node *root) const;
  
  /**
   * Return nodes predeccessor node or NULL if node is first.
   */
  prototype_node* previous_node() const;

  /**
   * Returns true if node is child of given parent node.
   */
  bool is_child_of(const prototype_node *parent) const;

  /**
   * Adjusts self and last marker of all predeccessor nodes with given
   * object proxy.
   */
  void adjust_left_marker(object_proxy *oproxy);
  
  /**
   * Adjust first marker of all successor nodes with given object proxy.
   */
  void adjust_right_marker(object_proxy *old_proxy, object_proxy *new_proxy);

  friend std::ostream& operator <<(std::ostream &os, const prototype_node &pn);

  // tree links
  prototype_node *parent;
  prototype_node *prev;
  prototype_node *next;
  std::auto_ptr<prototype_node> first;
  std::auto_ptr<prototype_node> last;

  // data
  std::auto_ptr<object_base_producer> producer;
  object_proxy *op_first;
  object_proxy *op_marker;
  object_proxy *op_last;

  unsigned int depth;
  unsigned long count;

  std::string type;	
};

}

#endif /* PROTOTYPE_NODE_HPP */
