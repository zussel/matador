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

#include "object/serializable.hpp"

#include "object/object_store.hpp"
#include "object/object_observer.hpp"
#include "object/object_list.hpp"
#include "object/object_creator.hpp"
#include "object/primary_key_reader.hpp"

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
    prototype_tree_.begin()->clear(true);
  }
  object_map_.clear();
}

bool object_store::empty() const
{
  const_prototype_iterator root = prototype_tree_.begin();
  return root->op_first->next_ == root->op_last;
}

void object_store::dump_objects(std::ostream &out) const
{
  const_prototype_iterator root = prototype_tree_.begin();
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

serializable * object_store::create(const char *type) const
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

void object_store::insert(object_container &oc)
{
  oc.install(this);
}

object_proxy*
object_store::insert_object(serializable *o, bool notify)
{
  // find type in tree
  if (!o) {
    throw object_exception("serializable is null");
  }

  // find prototype node
  prototype_iterator node = prototype_tree_.find(typeid(*o).name());
  if (node == prototype_tree_.end()) {
    // raise exception
    throw object_exception("couldn't insert serializable");
  }
  // retrieve and set new unique number into serializable
  object_proxy *oproxy = nullptr;
  t_serializable_proxy_map::iterator i = serializable_map_.find(o);
  if (i != serializable_map_.end()) {
    // serializable exists in serializable store
    oproxy = i->second;
    if (oproxy->linked()) {
      // an serializable exists in map.
      // replace it with new serializable
      // unlink it and
      // link it into new place in list
      oproxy->node()->remove(oproxy);
    }
    oproxy->reset(o);
  } else {
    /* serializable doesn't exist in map
     * if serializable has a valid id, update
     * the sequencer else assign new
     * unique id
     */
    oproxy = create_proxy(o, seq_.next());
    if (!oproxy) {
      throw object_exception("couldn't create serializable proxy");
    }
  }

  if (oproxy->has_primary_key()) {
    // if object has primary key of type short, int or long
    // set the id of proxy as value
    primary_key_reader<unsigned long> reader(oproxy->id());
    o->deserialize(reader);
  }

  // insert new element node
  node->insert(oproxy);

  // initialize serializable
  object_creator oc(oproxy, *this, notify);
  o->deserialize(oc);
  // set corresponding prototype node
  oproxy->node_ = node.get();
  // set this into persistent serializable
  // notify observer
  if (notify) {
    std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_insert, _1, oproxy));
  }
  // insert element into hash map for fast lookup
  object_map_[oproxy->id()] = oproxy;
  // return new serializable
  return oproxy;
}

bool object_store::is_removable(const object_base_ptr &o)
{
  return object_deleter_.is_deletable(o.proxy_);
}

void
object_store::remove(object_base_ptr &o)
{
  remove(o.proxy_);
}

void
object_store::remove(object_proxy *proxy)
{
  if (proxy == nullptr) {
    throw object_exception("serializable proxy is nullptr");
  }
  if (proxy->node() == nullptr) {
    throw object_exception("prototype node is nullptr");
  }
  // check if serializable tree is deletable
  if (!object_deleter_.is_deletable(proxy)) {
    throw object_exception("serializable is not removable");
  }
  
  object_deleter::iterator first = object_deleter_.begin();
  object_deleter::iterator last = object_deleter_.end();
  
  while (first != last) {
    if (!first->second.ignore) {
      remove_object((first++)->second.proxy, true);
    } else {
      ++first;
    }
  }
}
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
  
  prototype_iterator node = prototype_tree_.find(proxy->node()->type.c_str());
  if (node == prototype_tree_.end()) {
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

void
object_store::remove(object_container &oc)
{
  /**************
   * 
   * remove all objects from container
   * and first and last sentinel
   * 
   **************/
  // check if serializable tree is deletable
  if (!object_deleter_.is_deletable(oc)) {
    throw object_exception("couldn't remove container serializable");
  }

  object_deleter::iterator first = object_deleter_.begin();
  object_deleter::iterator last = object_deleter_.end();
  
  while (first != last) {
    if (!first->second.ignore) {
      remove_object((first++)->second.proxy, true);
    } else {
      ++first;
    }
  }
  oc.uninstall();
}

object_proxy* object_store::find_proxy(unsigned long id) const
{
  t_object_proxy_map::const_iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return nullptr;
  } else {
    return i->second;
  }
}

object_proxy* object_store::create_proxy(serializable *o, unsigned long id)
{
  if (id == 0) {
    return nullptr;
  }
  
  t_object_proxy_map::iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    std::unique_ptr<object_proxy> proxy(new object_proxy(o, id, this));

    return object_map_.insert(std::make_pair(id, proxy.release())).first->second;
  } else {
    return nullptr;
  }
}

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


void object_store::insert_proxy(object_proxy *oproxy)
{
  if (!oproxy->obj()) {
    throw object_exception("serializable of proxy is null pointer");
  }

  if (oproxy->ostore()) {
    throw object_exception("serializable proxy already in serializable store");
  }

  // find prototype node
  prototype_iterator node = prototype_tree_.find(typeid(*oproxy->obj()).name());
  if (node == prototype_tree_.end()) {
    // raise exception
    throw object_exception("couldn't insert serializable");
  }

  if (oproxy->id() == 0) {
    oproxy->id(seq_.next());
  } else {
    seq_.update(oproxy->id());
  }
  oproxy->ostore_ = this;

  node->insert(oproxy);

  // initialize serializable
  object_creator oc(oproxy, *this, true);
  oproxy->obj()->deserialize(oc);
  // notify observer
//  if (true) {
    std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_insert, _1, oproxy));
//  }
  // insert element into hash map for fast lookup
  object_map_[oproxy->id()] = oproxy;
}

sequencer_impl_ptr object_store::exchange_sequencer(const sequencer_impl_ptr &seq)
{
  return seq_.exchange_sequencer(seq);
}

}
