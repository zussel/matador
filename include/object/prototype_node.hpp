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

namespace oos {

class object_base_producer;
class object_proxy;

struct prototype_node {
  prototype_node();
  prototype_node(object_base_producer *p, const char *t);
  ~prototype_node();

  void clear();
  bool empty() const;
  unsigned long size() const;
  void insert(prototype_node *child);
  void remove(prototype_node *child);

  void unlink();

  prototype_node* next_node() const;
  prototype_node* previous_node() const;

  bool is_child_of(const prototype_node *parent) const;

  void adjust_left_marker(object_proxy *oproxy);
  void adjust_right_marker(object_proxy *old_proxy, object_proxy *new_proxy);

  friend std::ostream& operator <<(std::ostream &os, const prototype_node &pn);

  // tree links
  prototype_node *parent;
  prototype_node *prev;
  prototype_node *next;
  prototype_node *first;
  prototype_node *last;

  // data
  object_base_producer *producer;
  object_proxy *op_first;
  object_proxy *op_marker;
  object_proxy *op_last;

  unsigned int depth;
  unsigned long count;

  std::string type;	
};

}

#endif /* PROTOTYPE_NODE_HPP */
