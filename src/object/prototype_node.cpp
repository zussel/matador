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

#include "object/prototype_node.hpp"
#include "object/object_store.hpp"

namespace oos {

prototype_node::prototype_node()
  : parent(NULL)
  , prev(NULL)
  , next(NULL)
  , first(NULL)
  , last(NULL)
  , producer(NULL)
  , depth(0)
  , count(0)
{
}

prototype_node::prototype_node(object_base_producer *p, const char *t)
  : parent(NULL)
  , prev(NULL)
  , next(NULL)
  , first(new prototype_node)
  , last(new prototype_node)
  , producer(p)
  , depth(0)
  , count(0)
  , type(t)
{
  first->next = last;
  last->prev = first;
}

prototype_node::~prototype_node()
{
  while (first && first->next != last) {
    prototype_node *node = first->next;
    first->next = node->next;
    node->next->prev = first;
    delete node;
  }
  delete first;
  delete last;
  delete producer;
}

void
prototype_node::clear()
{
  while (first && first->next != last) {
    prototype_node *node = first->next;
    remove(node);
  }
  // remove objects
}

bool
prototype_node::empty() const
{
  return op_first->next == op_last;
}

unsigned long
prototype_node::size() const
{
  return count;
}

void
prototype_node::insert(prototype_node *child)
{
  child->parent = this;
  child->prev = last->prev;
  child->next = last;
  last->prev->next = child;
  last->prev = child;
  // set depth
  child->depth = depth + 1;
  // set object proxy pointer
  // 1. first
  if (op_first->next == op_last) {
    // node hasn't any object (proxy)
    child->op_first = op_first;
  } else {
    // node has some objects (proxy)
    child->op_first = op_last->prev;
  }
  // 2. marker
  child->op_marker = op_last;
  // 3. last
  child->op_last = op_last;
}    

void
prototype_node::remove(prototype_node *child)
{
  // unlink node
  child->prev->next = child->next;
  child->next->prev = child->prev;
  // clear objects
  child->clear();
  // delete node
  delete child;
}

prototype_node* prototype_node::next_node() const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first->next != last)
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last) {
      node = node->parent;
    }
    return node->next;
  }
}

prototype_node* prototype_node::previous_node() const
{
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (prev && prev->prev) {
    const prototype_node *node = prev;
    while (node->last && node->first->next != node->last) {
      node = node->last->prev;
    }
    return const_cast<prototype_node*>(node);
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    return parent;
  }
}

std::ostream& operator <<(std::ostream &os, const prototype_node &pn)
{
  os << "node [" << &pn << "] depth [" << pn.depth << "] type [" << pn.type << "] class [" << pn.producer->classname() << "]";
  if (pn.op_first->obj) {
    os << " first [" << *pn.op_first->obj << "]";
  } else {
    os << " first [op: " << pn.op_first << "]";
  }
  if (pn.op_marker->obj) {
    os << " marker [" << *pn.op_marker->obj << "]";
  } else {
    os << " marker [op: " << pn.op_marker << "]";
  }
  if (pn.op_last->obj) {
    os << " last [" << *pn.op_last->obj << "]";
  } else {
    os << " last [op: " << pn.op_last << "]";
  }
  int i = 0;
  object_proxy *iop = pn.op_first;
  while (iop->next != pn.op_marker) {
    ++i;
    iop = iop->next;
  }
  os << " (# " << i << ")";
  return os;
}

}
