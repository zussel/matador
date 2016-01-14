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

#include "object/object_store.hpp"
#include "object/object_container.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace std::placeholders;

namespace oos {

namespace detail {

object_inserter::object_inserter(object_store &ostore)
  : ostore_(ostore) { }

object_inserter::~object_inserter() { }

}

object_store::object_store()
  : first_(new prototype_node)
  , last_(new prototype_node)
  , object_inserter_(*this)
{
  // empty tree where first points to last and
  // last points to first sentinel
  first_->next = last_;
  last_->prev = first_;
}

object_store::~object_store()
{
  clear(true);
  delete last_;
  delete first_;
}

void object_store::detach(const char *type)
{
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    throw object_exception("unknown prototype type");
  }
  remove_prototype_node(node, node->depth == 0);
}

object_store::iterator object_store::detach(const prototype_iterator &i)
{
  if (i == end() || i.get() == nullptr) {
    throw object_exception("invalid prototype iterator");
  }
  return remove_prototype_node(i.get(), i->depth == 0);
}

object_store::iterator object_store::find(const char *type)
{
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    return end();
  }
  return iterator(node);
}

object_store::const_iterator object_store::find(const char *type) const
{
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    return end();
  }
  return const_iterator(node);
}

object_store::const_iterator object_store::begin() const
{
  return const_iterator(first_->next);
}

prototype_iterator object_store::begin() {
  return prototype_iterator(first_->next);
}

const_prototype_iterator object_store::end() const
{
  return const_iterator(last_);
}

prototype_iterator object_store::end()
{
  return iterator(last_);
}

void object_store::clear(bool full)
{
  if (full) {
    while (first_->next != last_) {
      remove_prototype_node(first_->next, true);
    }
  } else {
    // only delete objects
    prototype_iterator first = begin();
    prototype_iterator last = end();
    while (first != last) {
        (first++)->clear(false);
    }
//    prototype_tree_.begin()->clear(true);
  }
  object_map_.clear();
}

void object_store::clear(const char *type)
{
  clear(find(type));
}

void object_store::clear(const prototype_iterator &node)
{
  clear(node.get());
}

prototype_node* object_store::clear(prototype_node *node)
{
  prototype_node *current = node->first->next;
  while (current != node->last.get()) {
    current = clear(current);
  }
  // finally link first to last and vice versa
  return remove_prototype_node(node, false);
}

size_t object_store::size() const
{
  return (size_t) (std::distance(begin(), end()));
}

bool object_store::empty() const
{
  bool is_empty = true;
  for_each_root_node([&](const_prototype_iterator i) {
    is_empty &= i->empty(false);
  });
  return is_empty;
}

void object_store::dump_objects(std::ostream &out) const
{
  const_prototype_iterator root = begin();
  out << "dumping all objects\n";

  object_proxy *op = root->op_first;
  while (op) {
    out << "[" << op << "] (";
    if (op->obj()) {
      out << op->obj() << " prev [" << op->prev()->obj() << "] next [" << op->next()->obj() << "])\n";
    } else {
      out << "serializable 0)\n";
    }
    op = op->next_;
  }
}

void object_store::mark_modified(object_proxy *oproxy)
{
  std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_update, _1, oproxy));
}

void object_store::register_observer(object_observer *observer)
{
  if (std::find(observer_list_.begin(), observer_list_.end(), observer) == observer_list_.end()) {
    observer_list_.push_back(observer);
  }
}

void object_store::unregister_observer(object_observer *observer)
{
  t_observer_list::iterator i = std::find(observer_list_.begin(), observer_list_.end(), observer);
  if (i != observer_list_.end()) {
    observer_list_.erase(i);
  }
}

/*
object_proxy *object_store::initialze_proxy(object_proxy *oproxy, prototype_iterator &node, bool notify)
{// insert new element node
  node->insert(oproxy);

  // initialize object
  object_inserter_.insert(oproxy);
  // set this into persistent serializable
  // notify observer
  if (notify) {
    for_each(observer_list_.begin(), observer_list_.end(), bind(&object_observer::on_insert, _1, oproxy));
  }
  // insert element into hash map for fast lookup
  object_map_[oproxy->id()] = oproxy;

  return oproxy;
}
*/

void
object_store::remove_object(object_proxy *proxy, bool notify)
{
  // find prototype node
  if (!proxy) {
    throw object_exception("couldn't remove serializable, no proxy");
  }
  if (!proxy->node()) {
    throw object_exception("couldn't remove serializable, no prototype");
  }
  
  prototype_iterator node = find(proxy->node()->type());
  if (node == end()) {
    throw object_exception("couldn't find node for serializable");
  }
  
  if (object_map_.erase(proxy->id()) != 1) {
    // couldn't remove serializable
    // throw exception
    throw object_exception("couldn't remove serializable");
  }

  node->remove(proxy);

  if (notify) {
    // notify observer
    std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_delete, _1, proxy));
  }
  // set serializable in object_proxy to null
  object_proxy *op = proxy;
  // delete node
  delete op;
}

//void
//object_store::remove(object_container &oc)
//{
//  /**************
//   *
//   * remove all objects from container
//   * and first and last sentinel
//   *
//   **************/
//  // check if serializable tree is deletable
//  if (!object_deleter_.is_deletable(oc)) {
//    throw object_exception("couldn't remove container serializable");
//  }
//
//  object_deleter::iterator first = object_deleter_.begin();
//  object_deleter::iterator last = object_deleter_.end();
//
//  while (first != last) {
//    if (!first->second.ignore) {
//      remove_object((first++)->second.proxy, true);
//    } else {
//      ++first;
//    }
//  }
//  oc.uninstall();
//}

object_proxy* object_store::find_proxy(unsigned long id) const
{
  t_object_proxy_map::const_iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return nullptr;
  } else {
    return i->second;
  }
}

/*
object_proxy* object_store::create_proxy(unsigned long id)
{
  if (id == 0) {
    return nullptr;
  }

  t_object_proxy_map::iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    std::unique_ptr<object_proxy> proxy(new object_proxy(nullptr, id, this));

    return object_map_.insert(std::make_pair(id, proxy.release())).first->second;
  } else {
    return nullptr;
  }
}
*/
bool object_store::delete_proxy(unsigned long id)
{
  t_object_proxy_map::iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return false;
  } else if (i->second->linked()) {
    return false;
  } else {
    object_map_.erase(i);
    return true;
  }
}

object_proxy* object_store::register_proxy(object_proxy *oproxy)
{
  if (oproxy->id() != 0) {
    throw_object_exception("object proxy already registerd");
  }

  if (oproxy->node() == nullptr) {
    throw_object_exception("object proxy hasn't git a prototype node");
  }

  oproxy->id(seq_.next());

  return object_map_.insert(std::make_pair(oproxy->id(), oproxy)).first->second;
}

sequencer_impl_ptr object_store::exchange_sequencer(const sequencer_impl_ptr &seq)
{
  return seq_.exchange_sequencer(seq);
}

/*
 * adjust the marker of all predeccessor nodes
 * self and last marker
 */
void object_store::adjust_left_marker(prototype_node *root, object_proxy *old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *node = root->previous_node();
  // get previous node
  while (node) {
    if (node->op_marker == old_proxy) {
      node->op_marker = new_proxy;
    }
    if (node->depth >= root->depth && node->op_last == old_proxy) {
      node->op_last = new_proxy;
    }
    node = node->previous_node();
  }
}

void object_store::adjust_right_marker(prototype_node *root, object_proxy* old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *node = root->next_node();
  while (node) {
    if (node->op_first == old_proxy) {
      node->op_first = new_proxy;
    }
    node = node->next_node();
  }
}

prototype_node* object_store::find_prototype_node(const char *type) const {
  // check for null
  if (type == 0) {
    throw object_exception("invalid type (null)");
  }
  /*
   * first search in the prototype map
   */
  t_prototype_map::const_iterator i = prototype_map_.find(type);
  if (i == prototype_map_.end()) {
    /*
   * if not found search in the typeid to prototype map
   */
    t_typeid_prototype_map::const_iterator j = typeid_prototype_map_.find(type);
    if (j == typeid_prototype_map_.end()) {
      return nullptr;
    } else {
      const t_prototype_map &val = j->second;
      /*
     * if size is greater one (1) the name
     * is a typeid and has more than one prototype
     * node and therefor it is not unique and an
     * exception is thrown
     */
      if (val.size() > 1) {
        // throw exception
        throw object_exception("type id not unique");
      } else {
        // return the only prototype
        return val.begin()->second;
      }
    }
  } else {
    return i->second;
  }
}

prototype_node* object_store::remove_prototype_node(prototype_node *node, bool is_root) {
  // remove (and delete) from tree (deletes subsequently all child nodes
  // for each child call remove_prototype(child);
  prototype_node *next = node->next_node(node);

  while (node->has_children()) {
    remove_prototype_node(node->first->next, false);
  }
  // and objects they're containing
  node->clear(false);
  // delete prototype node as well
  // unlink node
  node->unlink();
  // get iterator
  t_prototype_map::iterator j = prototype_map_.find(node->type_.c_str());
  if (j != prototype_map_.end()) {
    prototype_map_.erase(j);
  }
  // find item in typeid map
  t_typeid_prototype_map::iterator k = typeid_prototype_map_.find(node->type_id());
  if (k != typeid_prototype_map_.end()) {
    k->second.erase(node->type_);
    if (k->second.empty()) {
      typeid_prototype_map_.erase(k);
    }
  } else {
    throw object_exception("couldn't find node by id");
  }
  if (is_root) {
    delete node->op_first;
    delete node->op_last;
  }
  delete node;
  return next;
}

prototype_node *object_store::find_parent(const char *name) const
{
  prototype_node *parent_node(nullptr);
  if (name != nullptr) {
    parent_node = find_prototype_node(name);
    if (!parent_node) {
      throw object_exception("unknown prototype type");
    }
  }
  return parent_node;
}

namespace detail {

object_deleter::t_object_count_struct::t_object_count_struct(object_proxy *oproxy, bool ignr)
  : proxy(oproxy), reference_counter(oproxy->reference_count())
  , ignore(ignr)
{ }

//bool object_deleter::is_deletable(object_container &oc)
//{
//  object_count_map.clear();
//  oc.for_each(std::bind(&object_deleter::check_object_list_node, this, _1));
//  return check_object_count_map();
//}

object_deleter::iterator object_deleter::begin()
{
  return object_count_map.begin();
}

object_deleter::iterator object_deleter::end()
{
  return object_count_map.end();
}

void object_deleter::check_object_list_node(object_proxy *proxy)
{
  std::pair<t_object_count_map::iterator, bool> ret = object_count_map.insert(
    std::make_pair(proxy->id(), t_object_count(proxy, false))
  );

  /**********
   *
   * serializable is already in list and will
   * be ignored on deletion so set
   * ignore flag to false because this
   * node must be deleted
   *
   **********/
  if (!ret.second && ret.first->second.ignore) {
    ret.first->second.ignore = false;
  }

  // start collecting information
//  proxy->obj()->deserialize(*this);
}

bool object_deleter::check_object_count_map() const
{
  // check the reference and pointer counter of collected objects
  const_iterator first = object_count_map.begin();
  const_iterator last = object_count_map.end();
  while (first != last) {
    if (first->second.ignore) {
      ++first;
    } else if (first->second.reference_counter == 0) {
//    } else if (first->second.ref_count == 0 && first->second.ptr_count == 0) {
      ++first;
    } else {
      return false;
    }
  }
  return true;
}

}

}
