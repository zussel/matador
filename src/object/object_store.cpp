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
#include "object/object_atomizer.hpp"
#include "object/object_list.hpp"
#include "object/prototype_node.hpp"

#include <tr1/functional>
#include <memory>
#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <stack>

using std::tr1::placeholders::_1;

namespace oos {

class equal_type : public std::unary_function<const prototype_node*, bool> {
public:
  explicit equal_type(const std::string &type) : type_(type) {}

  bool operator() (const prototype_node *x) const {
    return x->type == type_;
  }
private:
  const std::string &type_;
};

class object_actor : public object_atomizer
{
public:
  virtual ~object_actor() {}
  
protected:
  std::stack<object*> object_stack_;
};

class object_creator : public object_actor
{
public:
  object_creator(object_store &ostore) : ostore_(ostore) {}
  virtual ~object_creator() {}

  virtual void read_object(const char*, base_object_ptr &x)
  {
    if (!x.is_reference()) {
      // create object
      x.reset(ostore_.create(x.type()));
      object_stack_.push(x.ptr());
      x.ptr()->read_from(this);
      object_stack_.pop();
    }
  }
  virtual void read_object_list(const char*, object_list_base &x)
  {
    ostore_.insert_object_list(x);
  }
private:
  object_store &ostore_;
};

class object_deleter : public object_actor
{
public:
  object_deleter(object_store &ostore) : ostore_(ostore) {}
  virtual ~object_deleter() {}

  virtual void read_object(const char*, base_object_ptr &x)
  {
    if (!x.is_reference() && x.ptr()) {
      ostore_.remove_object(x.ptr());
    }
  }
  virtual void read_object_list(const char*, object_list_base &)
  {
  }
private:
  object_store &ostore_;
};

object_store::object_store()
  : root_(new prototype_node(new object_producer<object>, "OBJECT"))
  , id_(0)
  , first_(new object_proxy(NULL, this))
  , last_(new object_proxy(NULL, this))
{
  prototype_node_name_map_.insert(std::make_pair("OBJECT", root_));
  prototype_node_type_map_.insert(std::make_pair(root_->producer->classname(), root_));
  // set marker for root element
  root_->op_first = first_;
  root_->op_marker = last_;
  root_->op_last = last_;
  root_->op_first->next = root_->op_last;
  root_->op_last->prev = root_->op_first;
}

void delete_object_proxy(object_proxy *op)
{
  delete op;
}

object_store::~object_store()
{
  delete root_;
  delete first_;
  delete last_;
  // delete all deleted object_proxys
  std::for_each(deleted_object_proxy_list_.begin(), deleted_object_proxy_list_.end(), delete_object_proxy);

  while (first_->next != last_) {
    object_proxy *i = first_->next;
    first_->next = i->next;
    i->next->prev = first_;
    delete i;
  }
}

bool
object_store::insert_prototype(object_base_producer *producer, const char *type, const char *parent)
{
  // set node to root node
  prototype_node *parent_node;
  if (parent) {
    // parent type name is set search parent node
    t_prototype_node_map::iterator i = prototype_node_name_map_.find(parent);
    if (i == prototype_node_name_map_.end()) {
      //throw new object_exception("couldn't find parent prototype");
      return false;
    }
    parent_node = i->second;
  }
  // find prototype node 'type' starting from node
  t_prototype_node_map::iterator i = prototype_node_name_map_.find(type);
  if (i != prototype_node_name_map_.end()) {
    //throw new object_exception("prototype already exists");
    return false;
  }
  prototype_node *node = new prototype_node(producer, type);
  // append as child to parent prototype node
  parent_node->insert(node);
  // store prototype in map
  prototype_node_name_map_.insert(std::make_pair(type, node));
  prototype_node_type_map_.insert(std::make_pair(producer->classname(), node));
  // return success
  return true;
}

bool object_store::remove_prototype(const char *type)
{
  t_prototype_node_map::iterator i = prototype_node_name_map_.find(type);
  if (i == prototype_node_name_map_.end()) {
    //throw new object_exception("couldn't find prototype");
    return false;
  }
  if (!i->second->parent) {
    // now parent
    return false;
  }
  t_prototype_node_map::iterator j = prototype_node_type_map_.find(i->second->producer->classname());
  if (j == prototype_node_type_map_.end()) {
		// couldn't fnd prototype in type map
		// throw exception
		return false;
	}
  // remove (and delete) from tree (deletes subsequently all child nodes
  // for each child call remove_prototype(child);
  while (i->second->first->next != i->second->last) {
    prototype_node *node = i->second->first->next;
    remove_prototype(node->type.c_str());
  }
  // and objects they're containing 
  i->second->parent->remove(i->second);
  // erase node from map
  prototype_node_name_map_.erase(i);
  prototype_node_type_map_.erase(j);
  return true;
}

void object_store::clear_prototypes()
{
  root_->clear();
}

int depth(prototype_node *node)
{
  int d = 0;
  while (node->parent) {
    node = node->parent;
    ++d;
  }
  return d;
}

void object_store::dump_prototypes(std::ostream &out) const
{
  prototype_node *node = root_;
  out << "dumping prototype tree:\n";
  do {
    int d = depth(node);
    for (int i = 0; i < d; ++i) out << " ";
    out << *node << "\n";
    node = node->next_node();
  } while (node);
  out << std::endl;
}

void object_store::dump_objects(std::ostream &out) const
{
  out << "dumping all objects\n";

  object_proxy *i = first_;
  while (i->next != last_) {
    i = i->next;
    out << *i->obj << " (prev [" << i->prev->obj << "] next [" << i->next->obj << "])\n";
  }
}

object* object_store::create(const char *type) const
{
  t_prototype_node_map::const_iterator i = prototype_node_name_map_.find(type);
  if (i == prototype_node_name_map_.end()) {
    //throw new object_exception("couldn't find prototype");
    return NULL;
  }
	return i->second->producer->create();
}

void object_store::mark_modified(object_proxy *oproxy)
{
  std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_update, _1, oproxy->obj));
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
    delete *i;
    observer_list_.erase(i);
  }
}

void object_store::cleanup()
{
  // remove all object_proxy from object_proxy
  // link list
  // set object link to null
  // erase object_proxy from to delete list
}

object* object_store::insert_object(object *o)
{
  // find type in tree
  if (!o) {
    // throw exception
    return NULL;
  }
  // find prototype node
  t_prototype_node_map::iterator i = prototype_node_type_map_.find(typeid(*o).name());
  if (i == prototype_node_type_map_.end()) {
    // raise exception
    //throw new object_exception("couldn't insert element of type [" + o->type() + "]\n");
    return NULL;
  }
  prototype_node *node = i->second;
  // retrieve and set new unique number into object
  //object->id(UniqueProducer::instance().number("default"));
  o->id(++id_);

  // insert new element node
  object_proxy *oproxy = new object_proxy(o, this);
  
  // check amount of object in subtree
  if (node->count >= 2) {
    // there are more than two objects (normal case)
    // insert before last last
    //std::cout << "more than two elements: inserting " << *o << " before second last (" << *node->op_marker->prev->obj << ")\n";
    node->op_marker->prev->insert(oproxy);
  } else if (node->count == 1) {
    // there is one object in subtree
    // insert as first; adjust "left" marker
    /*if (node->op_marker->prev->obj) {
      std::cout << "one element in list: inserting " << *o << " as first (before: " << *node->op_marker->prev->obj << ")\n";
    } else {
      std::cout << "one element in list: inserting " << *o << " as first (before: [0])\n";
    }*/
    node->op_marker->prev->insert(oproxy);
    adjust_left_marker(node, oproxy);
  } else /* if (node->count == 0) */ {
    // there is no object in subtree
    // insert as last; adjust "right" marker
    /*if (node->op_marker->obj) {
      std::cout << "list is empty: inserting " << *o << " as last before " << *node->op_marker->obj << "\n";
    } else {
      std::cout << "list is empty: inserting " << *o << " as last before [0]\n";
    }*/
    node->op_marker->insert(oproxy);
    adjust_left_marker(node, oproxy);
    adjust_right_marker(node, oproxy->prev, oproxy);
  }
  // create object
  object_creator oc(*this);
  o->read_from(&oc);
  // notify observer
  std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_insert, _1, o));
  // dump objects
  //std::cout << std::endl;
/*  dump_objects(std::cout);
  std::cout << std::endl;*/
  // adjust size
  ++node->count;
  // insert element into hash map
  //  objectmap_[object->id()] = object;
  // set this into persistent object
  o->proxy_ = oproxy;
  return o;
}

bool object_store::remove_object(object *o)
{
  // find prototype node
  t_prototype_node_map::iterator i = prototype_node_type_map_.find(typeid(*o).name());
  if (i == prototype_node_type_map_.end()) {
    // raise exception
    //throw new object_exception("couldn't insert element of type [" + o->type() + "]\n");
    return false;
  }
  prototype_node *node = i->second;

  if (o->proxy_ == node->op_first->next) {
    // adjust left marker
    adjust_left_marker(node, node->op_first->next->next);
  }
  if (o->proxy_ == node->op_marker->prev) {
    // adjust right marker
    adjust_right_marker(node, o->proxy_, node->op_marker->prev->prev);
  }
  // unlink object_proxy
  o->proxy_->remove();

  // call object deleter for object
  object_deleter deleter(*this);
  o->read_from(&deleter);
  // notify observer
  std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_delete, _1, o));
  // if object was last object in list of prototype node
  // adjust prototype node last
  // if object was last object (now empty)
  // adjust prototype node first, marker and last

  // mark object proxy as deleted
  // set object in object_proxy to null
  object_proxy *op = o->proxy_;
  delete o;
  op->obj = NULL;
  deleted_object_proxy_list_.push_back(op);
  // return true
  return true;
}

bool object_store::insert_object_list(object_list_base &olb)
{
  olb.ostore_ = this;
  insert_object(olb.first_obj_);
  insert_object(olb.last_obj_);
  return true;
}

bool is_child_of(prototype_node *start, prototype_node *node)
{
  while (start->depth < node->depth) {
    node = node->parent;
  }
  return node == start;
}

void object_store::adjust_left_marker(prototype_node *node, object_proxy *oproxy)
{
  // store start node
  prototype_node *start = node;
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
      //std::cout << "break: before adjusting left node: " << *node << "\n";
      node->op_marker = oproxy;
      //if (start->parent != node || node->empty()) {
      if (!is_child_of(node, start) || node->empty()) {
        node->op_last = oproxy;
      }
      //std::cout << "break: after adjusting left node: " << *node << "\n";
      /*break;
    }*/
      if (do_break) {
        break;
      }
    node = node->previous_node();
  }
}

void object_store::adjust_right_marker(prototype_node *node, object_proxy *old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *start = node;
  // get previous node
  node = node->next_node();
  bool first = true;
  while (node) {
    if (node->op_first == old_proxy) {
      //std::cout << "before adjusting right node: " << *node << "\n";
      node->op_first = new_proxy;
      //std::cout << "after adjusting right node: " << *node << "\n";
    } else {
      //std::cout << "break: before adjusting right node: " << *node << "\n";
      node->op_first = new_proxy;
      //std::cout << "break: after adjusting right node: " << *node << "\n";
      break;
    }
    // check watermark
    if (first) {
      if (start->depth == node->depth && !node->empty()) {
        break;
      }
      first = false;
    } else if (start->depth <= node->depth && !node->empty()) {
      break;
    }
    node = node->next_node();
  }
}

}
