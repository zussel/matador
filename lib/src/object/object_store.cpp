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

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/object_observer.hpp"
#include "object/object_list.hpp"
#include "object/object_creator.hpp"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace std::placeholders;

namespace oos {

object_store::object_store()
{}

object_store::~object_store()
{
  clear(true);
}


prototype_tree &object_store::prototypes() {
  return prototype_tree_;
}


const prototype_tree &object_store::prototypes() const {
  return prototype_tree_;
}

prototype_iterator
object_store::insert_prototype(object_base_producer *producer, const char *type, bool abstract, const char *parent)
{
  return prototype_tree_.insert(producer, type, abstract, parent);
}

void object_store::remove_prototype(const char *type)
{
  prototype_tree_.remove(type);
}

prototype_iterator object_store::find_prototype(const char *type)
{
  return prototype_tree_.find(type);
}

const_prototype_iterator object_store::find_prototype(const char *type) const
{
  return prototype_tree_.find(type);
}

const_prototype_iterator object_store::begin() const
{
  return prototype_tree_.begin();
}

prototype_iterator object_store::begin() {
  return prototype_tree_.begin();
}

const_prototype_iterator object_store::end() const
{
  return prototype_tree_.end();
}

prototype_iterator object_store::end()
{
  return prototype_tree_.end();
}

void object_store::clear(bool full)
{
  if (full) {
    prototype_tree_.clear();
  } else {
    // only delete objects
    prototype_tree_.begin()->clear(prototype_tree_, true);
  }
  object_map_.clear();
}

bool object_store::empty() const
{
  const_prototype_iterator root = prototype_tree_.begin();
  return root->op_first->next == root->op_last;
}

void object_store::dump_objects(std::ostream &out) const
{
  const_prototype_iterator root = prototype_tree_.begin();
  out << "dumping all objects\n";

  object_proxy *op = root->op_first;
  while (op) {
    out << "[" << op << "] (";
    if (op->obj) {
      out << *op->obj << " prev [" << op->prev->obj << "] next [" << op->next->obj << "])\n";
    } else {
      out << "object 0)\n";
    }
    op = op->next;
  }
}

object* object_store::create(const char *type) const
{
  const_prototype_iterator node = prototype_tree_.find(type);
  if (node != prototype_tree_.end()) {
    return node->producer->create();
  } else {
    throw object_exception("unknown prototype type");
  }
}

void object_store::mark_modified(object_proxy *oproxy)
{
  std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_update, _1, oproxy->obj));
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

void object_store::insert(object_container &oc)
{
  oc.install(this);
}

object*
object_store::insert_object(object *o, bool notify)
{
  // find type in tree
  if (!o) {
    throw object_exception("object is null");
  }

  // find prototype node
  prototype_iterator node = prototype_tree_.find(typeid(*o).name());
  if (node == prototype_tree_.end()) {
    // raise exception
    std::string msg("couldn't insert element of type [" + std::string(typeid(*o).name()) + "]");
    throw object_exception(msg.c_str());
  }
  // retrieve and set new unique number into object
  object_proxy *oproxy = find_proxy(o->id());
  if (oproxy) {
    if (oproxy->linked()) {
      // an object exists in map.
      // replace it with new object
      // unlink it and
      // link it into new place in list
      remove_proxy(oproxy->node, oproxy);
    }
    oproxy->reset(o);
  } else {
    /* object doesn't exist in map
     * if object has a valid id, update
     * the sequencer else assign new
     * nique id
     */
    if (o->id() == 0) {
      o->id(seq_.next());
    } else {
      seq_.update(o->id());
    }
    oproxy = create_proxy(o->id());
    if (!oproxy) {
      throw object_exception("couldn't create object proxy");
    }
    oproxy->obj = o;
  }
  // insert new element node
  insert_proxy(node, oproxy);
  // create object
  object_creator oc(*this, notify);
  o->deserialize(oc);
  // set corresponding prototype node
  oproxy->node = node.get();
  // set this into persistent object
  o->proxy_ = oproxy;
  // notify observer
  if (notify) {
    std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_insert, _1, o));
  }
  // insert element into hash map for fast lookup
  object_map_[o->id()] = oproxy;
  // return new object
  return o;
}

bool object_store::is_removable(const object_base_ptr &o)
{
  return object_deleter_.is_deletable(o.ptr());
}

void
object_store::remove(object_base_ptr &o)
{
  remove(o.ptr());
}

void
object_store::remove(object *o)
{
  // check if object tree is deletable
  if (!object_deleter_.is_deletable(o)) {
    throw object_exception("object is not removable");
  }
  
  object_deleter::iterator first = object_deleter_.begin();
  object_deleter::iterator last = object_deleter_.end();
  
  while (first != last) {
    if (!first->second.ignore) {
      remove_object((first++)->second.obj, true);
    } else {
      ++first;
    }
  }
}
void
object_store::remove_object(object *o, bool notify)
{
  // find prototype node
  if (!o->proxy_->node) {
    throw object_exception("couldn't remove object, no proxy");
  }
  
  prototype_iterator node = prototype_tree_.find(o->proxy_->node->type.c_str());
  if (node == prototype_tree_.end()) {
    throw object_exception("couldn't find node for object");
  }
  
  if (object_map_.erase(o->id()) != 1) {
    // couldn't remove object
    // throw exception
    throw object_exception("couldn't remove object");
  }

  remove_proxy(node.get(), o->proxy_);

  if (notify) {
    // notify observer
    std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_delete, _1, o));
  }
  // set object in object_proxy to null
  object_proxy *op = o->proxy_;
  // delete node
  delete op;
}

void
object_store::remove(object_container &oc)
{
  /**************
   * 
   * remove all objects from container
   * and first and last sentinel
   * 
   **************/
  // check if object tree is deletable
  if (!object_deleter_.is_deletable(oc)) {
    throw object_exception("couldn't remove container object");
  }

  object_deleter::iterator first = object_deleter_.begin();
  object_deleter::iterator last = object_deleter_.end();
  
  while (first != last) {
    if (!first->second.ignore) {
      remove_object((first++)->second.obj, true);
    } else {
      ++first;
    }
  }
  oc.uninstall();
}

void
object_store::link_proxy(object_proxy *base, object_proxy *prev_proxy)
{
  // link oproxy before this node
  prev_proxy->prev = base->prev;
  prev_proxy->next = base;
  if (base->prev) {
    base->prev->next = prev_proxy;
  }
  base->prev = prev_proxy;
}

void
object_store::unlink_proxy(object_proxy *proxy)
{
  if (proxy->prev) {
    proxy->prev->next = proxy->next;
  }
  if (proxy->next) {
    proxy->next->prev = proxy->prev;
  }
  proxy->prev = NULL;
  proxy->next = NULL;
}

object_proxy* object_store::find_proxy(long id) const
{
  t_object_proxy_map::const_iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return NULL;
  } else {
    return i->second;
  }
}

object_proxy* object_store::create_proxy(long id)
{
  if (id == 0) {
    return 0;
  }
  
  t_object_proxy_map::iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return object_map_.insert(std::make_pair(id, new object_proxy(id, this))).first->second;
  } else {
    return 0;
  }
}

bool object_store::delete_proxy(long id)
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

void object_store::insert_proxy(const prototype_iterator &node, object_proxy *oproxy)
{
  // check count of object in subtree
  if (node->count >= 2) {
    /*************
     *
     * there are more than two objects (normal case)
     * insert before last last
     *
     *************/
    oproxy->link(node->op_marker->prev);
  } else if (node->count == 1) {
    /*************
     *
     * there is one object in subtree
     * insert as first; adjust "left" marker
     *
     *************/
    oproxy->link(node->op_marker->prev);
    prototype_tree_.adjust_left_marker(node.get(), oproxy->next, oproxy);
  } else /* if (node->count == 0) */ {
    /*************
     *
     * there is no object in subtree
     * insert as last; adjust "right" marker
     *
     *************/
    oproxy->link(node->op_marker);
    prototype_tree_.adjust_left_marker(node.get(), oproxy->next, oproxy);
    prototype_tree_.adjust_right_marker(node.get(), oproxy->prev, oproxy);
  }
  // set prototype node
  oproxy->node = node.get();
  // adjust size
  ++node->count;
}

void object_store::remove_proxy(prototype_node *node, object_proxy *oproxy)
{
  if (oproxy == node->op_first->next) {
    // adjust left marker
    prototype_tree_.adjust_left_marker(node, node->op_first->next, node->op_first->next->next);
  }
  if (oproxy == node->op_marker->prev) {
    // adjust right marker
    prototype_tree_.adjust_right_marker(node, oproxy, node->op_marker->prev->prev);
  }
  // unlink object_proxy
  unlink_proxy(oproxy);
  // adjust object count for node
  --node->count;
}

sequencer_impl_ptr object_store::exchange_sequencer(const sequencer_impl_ptr &seq)
{
  return seq_.exchange_sequencer(seq);
}

}
