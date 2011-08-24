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
  first->next = last.get();
  last->prev = first.get();
}

prototype_node::~prototype_node()
{
}

void
prototype_node::clear()
{
  if (empty(true)) {
    return;
  }
  std::cout << "clearing all objects for prototype [" << type << "]\n";
  std::cout.flush();
  // remove object proxies until first and marker are left
  // adjust marker first
  object_proxy *old_first = op_first->next;
  adjust_left_marker(op_marker);
  adjust_right_marker(old_first, op_first);

  std::cout << "old first: " << *old_first << "\nbegin loop\n";
  while (op_first->next != op_marker) {
    object_proxy *op = op_first->next;
    std::cout << "op before: " << *op << "\n";
    // remove object proxy from list
    op->remove();
    // delete object proxy and object
    op->clear();
    std::cout << "op after: " << *op << "\n";
    if (op != old_first) {
      delete op;
    }
  }
  std::cout << "end loop\n";
  delete old_first;
}

bool
prototype_node::empty(bool self) const
{
  return op_first->next == (self ? op_marker : op_last);
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
  child->next = last.get();
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

void prototype_node::remove()
{
  // delete all object proxies
  clear();
  // delete all cild nodes
  while (first->next != last.get()) {
    prototype_node *node = first->next;
    node->remove();
    delete node;
  }
  // unlink this node
  unlink();
}

void prototype_node::unlink()
{
  // unlink node
  prev->next = next;
  next->prev = prev;
  next = NULL;
  prev = NULL;
}

prototype_node* prototype_node::next_node() const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first->next != last.get())
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last.get()) {
      node = node->parent;
    }
    return node->next;
  }
}

prototype_node* prototype_node::next_node(prototype_node *root) const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first->next != last.get())
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last.get()) {
      node = node->parent;
    }
    if (node != root)
      return node->next;
    else
      return NULL;
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
    while (node->last.get() && node->first->next != node->last.get()) {
      node = node->last->prev;
    }
    return const_cast<prototype_node*>(node);
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    return parent;
  }
}

bool prototype_node::is_child_of(const prototype_node *parent) const
{
  const prototype_node *node = this;
  while (parent->depth < node->depth) {
    node = node->parent;
  }
  return node == parent;
}

void prototype_node::adjust_left_marker(object_proxy *old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *node = this;
  // get previous node
  node = node->previous_node();
  while (node) {
    bool do_break = node->op_first->next != oproxy;
    /*
    if (node->op_first->next == oproxy) {
      std::cout << "before adjusting left node: " << *node << "\n";
      node->op_last = oproxy;
      node->op_marker = oproxy;
      std::cout << "after adjusting left node: " << *node << "\n";
    } else {
    */
      std::cout << "break: before adjusting left node: " << *node << "\n";
      std::cout.flush();
      node->op_marker = oproxy;
      //if (start->parent != node || node->empty()) {
      if (!is_child_of(node) || node->empty(false)) {
        node->op_last = oproxy;
      }
      std::cout << "break: after adjusting left node: " << *node << "\n";
      std::cout.flush();
      /*break;
    }*/
      if (do_break) {
        break;
      }
    node = node->previous_node();
  }
}

void prototype_node::adjust_right_marker(object_proxy *old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *node = this;
  // get previous node
  node = node->next_node();
  bool first = true;
  while (node) {
    if (node->op_first == old_proxy) {
      std::cout << "before adjusting right node: " << *node << "\n";
      std::cout.flush();
      node->op_first = new_proxy;
      std::cout << "after adjusting right node: " << *node << "\n";
      std::cout.flush();
    } else {
      std::cout << "break: before adjusting right node: " << *node << "\n";
      std::cout.flush();
      node->op_first = new_proxy;
      std::cout << "break: after adjusting right node: " << *node << "\n";
      std::cout.flush();
      break;
    }
    /*
    // check watermark
    if (first) {
      if (depth == node->depth && !node->empty(false)) {
        break;
      }
      first = false;
    } else if (depth <= node->depth && !node->empty(false)) {
      break;
    }
    */
    node = node->next_node();
  }
}

std::ostream& operator <<(std::ostream &os, const prototype_node &pn)
{
  if (pn.parent) {
    os << "\t" << pn.parent->type << " -> " << pn.type << "\n";
  }
  os << "\t" << pn.type << " [label=\"{" << pn.type;
  os << "|{op_first|" << pn.op_first << "}";
  os << "|{op_first:obj|" << pn.op_first->obj << "}";
  os << "|{op_marker|" << pn.op_marker << "}";
  os << "|{op_marker:obj|" << pn.op_marker->obj << "}";
  os << "|{op_last|" << pn.op_last << "}";
  os << "|{op_last:obj|" << pn.op_last->obj << "}";
  // determine size
  int i = 0;
  object_proxy *iop = pn.op_first;
  while (iop && iop->next != pn.op_marker) {
    ++i;
    iop = iop->next;
  }
  os << "|{size|" << i << "}";
  os << "}\"]\n";
  /*
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
  */
  return os;
}

}
