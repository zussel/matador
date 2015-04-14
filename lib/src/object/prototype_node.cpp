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
#include "object/prototype_tree.hpp"
#include "object/object_store.hpp"

#include <iostream>

using namespace std;

namespace oos {

prototype_node::prototype_node()
{
}

prototype_node::prototype_node(object_base_producer *p, const char *t, bool a)
  : first(new prototype_node)
  , last(new prototype_node)
  , producer(p)
  , type(t)
  , abstract(a)
{
  first->next = last.get();
  last->prev = first.get();
}

void prototype_node::initialize(object_base_producer *p, const char *t, bool a)
{
  first.reset(new prototype_node);
  last.reset(new prototype_node);
  producer.reset(p);
  type.assign(t);
  abstract = a;
  initialized = true;
  has_primary_key = false;
  first->next = last.get();
  last->prev = first.get();
}

prototype_node::~prototype_node()
{}

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

void prototype_node::insert(object_proxy *proxy)
{
  // check count of object in subtree
  if (count >= 2) {
    /*************
     *
     * there are more than two objects (normal case)
     * insert before last last
     *
     *************/
    proxy->link(op_marker->prev);
  } else if (count == 1) {
    /*************
     *
     * there is one object in subtree
     * insert as first; adjust "left" marker
     *
     *************/
    proxy->link(op_marker->prev);
    tree->adjust_left_marker(this, proxy->next, proxy);
  } else /* if (node->count == 0) */ {
    /*************
     *
     * there is no object in subtree
     * insert as last; adjust "right" marker
     *
     *************/
    proxy->link(op_marker);
    tree->adjust_left_marker(this, proxy->next, proxy);
    tree->adjust_right_marker(this, proxy->prev, proxy);
  }
  // set prototype node
  proxy->node = this;
  // adjust size
  ++count;
  // find and insert primary key
  std::shared_ptr<primary_key_base> pk(pk_serializer.serialize(proxy->obj));
  if (pk) {
    primary_key_map.insert(std::make_pair(pk, proxy));
  }
}

void prototype_node::remove(object_proxy *proxy)
{
  if (proxy == op_first->next) {
    // adjust left marker
    tree->adjust_left_marker(this, op_first->next, op_first->next->next);
  }
  if (proxy == op_marker->prev) {
    // adjust right marker
    tree->adjust_right_marker(this, proxy, op_marker->prev->prev);
  }
  // unlink object_proxy
  if (proxy->prev) {
    proxy->prev->next = proxy->next;
  }
  if (proxy->next) {
    proxy->next->prev = proxy->prev;
  }
  proxy->prev = 0;
  proxy->next = 0;

  // adjust object count for node
  --count;
}

void prototype_node::clear(prototype_tree &tree, bool recursive)
{
  if (!empty(true)) {
    tree.adjust_left_marker(this, op_first->next, op_marker);
    tree.adjust_right_marker(this, op_marker->prev, op_first);

    while (op_first->next != op_marker) {
      object_proxy *op = op_first->next;
      // remove object proxy from list
      op->unlink();
      // delete object proxy and object
      delete op;
    }
    count = 0;
  }

  if (recursive) {
    prototype_node *current = first->next;

    while (current != last.get()) {
      current->clear(tree, recursive);
      current = current->next;
    }
  }
}

void prototype_node::unlink()
{
  // unlink node
  prev->next = next;
  next->prev = prev;
  next = 0;
  prev = 0;
}

prototype_node* prototype_node::next_node() const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first && first->next != last.get())
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

prototype_node* prototype_node::next_node(const prototype_node *root) const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first && first->next != last.get())
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last.get() && node->parent != root) {
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
    while (node->last && node->first->next != node->last.get()) {
      node = node->last->prev;
    }
    return const_cast<prototype_node*>(node);
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    return parent;
  }
}

prototype_node* prototype_node::previous_node(const prototype_node *root) const
{
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (prev && prev->prev) {
    const prototype_node *node = prev;
    while (node->last && node->first->next != node->last.get() && node->parent != root) {
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


bool prototype_node::has_children() const
{
  return first->next != last.get();
}

std::ostream& operator <<(std::ostream &os, const prototype_node &pn)
{
  if (pn.parent) {
    os << "\t" << pn.parent->type << " -> " << pn.type << "\n";
  }
  os << "\t" << pn.type << " [label=\"{" << pn.type << " (" << &pn << ")";
  os << "|{op_first|" << pn.op_first << "}";
  os << "|{op_marker|" << pn.op_marker << "}";
  os << "|{op_last|" << pn.op_last << "}";
  os << "|{parent|" << pn.parent << "}";
  os << "|{prev|" << pn.prev << "}";
  os << "|{next|" << pn.next << "}";
  os << "|{first|" << pn.first.get() << "}";
  os << "|{last|" << pn.last.get() << "}";
  // determine size
  int i = 0;
  object_proxy *iop = pn.op_first;
  while (iop && iop->next != pn.op_marker) {
    ++i;
    iop = iop->next;
  }
  os << "|{size|" << i << "}";
  /*
  os << "|{relations}";
  // list relations
  prototype_node::type_map_t::const_iterator first = pn.relations.begin();
  prototype_node::type_map_t::const_iterator last = pn.relations.end();
  while (first != last) {
    prototype_node::field_prototype_map_t::const_iterator ffirst = first->second.begin();
    prototype_node::field_prototype_map_t::const_iterator llast = first->second.end();
    while (ffirst != llast) {
      os << "|{" << (ffirst->second ? ffirst->second->type : "null") << "|" << ffirst->first << "}";
      ++ffirst;
    }
    ++first;
  }
  */
  os << "}\"]\n";
  return os;
}

}
