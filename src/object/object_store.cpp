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
#include "object/object_proxy.hpp"
#include "object/object_store.hpp"
#include "object/object_observer.hpp"
#include "object/object_list.hpp"
#include "object/object_vector.hpp"
#include "object/object_container.hpp"
#include "object/object_creator.hpp"
#include "object/object_deleter.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_node.hpp"

#ifdef WIN32
#include <functional>
#include <memory>
#else
#include <tr1/functional>
#include <tr1/memory>
#endif

#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <algorithm>
#include <stack>

using namespace std;
using namespace std::tr1::placeholders;

namespace oos {

class relation_handler : public generic_object_writer<relation_handler>
{
public:
  typedef std::list<std::string> string_list_t;
  typedef string_list_t::const_iterator const_iterator;

public:
  relation_handler(object_store &ostore, prototype_node *node)
    : generic_object_writer<relation_handler>(this)
    , ostore_(ostore)
    , node_(node)
  {}
  virtual ~relation_handler() {}

  template < class T >
  void write_value(const char*, const T&) {}
  
  void write_value(const char*, const char*, int) {}
  
  void write_value(const char *id, const object_container &x)
  {
    /*
     * container knows if it needs
     * a relation table
     */
    x.handle_container_item(ostore_, id, node_);
  }
  
private:
  object_store &ostore_;
  prototype_node *node_;
};

/*
class equal_type : public std::unary_function<const prototype_node*, bool> {
public:
  explicit equal_type(const std::string &type) : type_(type) {}

  bool operator() (const prototype_node *x) const {
    return x->type == type_;
  }
private:
  const std::string &type_;
};
*/

prototype_iterator::prototype_iterator()
  : node_(NULL)
{}

prototype_iterator::prototype_iterator(prototype_node *node)
  : node_(node)
{}

prototype_iterator::prototype_iterator(const prototype_iterator &x)
  : node_(x.node_)
{}

prototype_iterator& prototype_iterator::operator=(const prototype_iterator &x)
{
  node_ = x.node_;
  return *this;
}

prototype_iterator::~prototype_iterator()
{}

bool prototype_iterator::operator==(const prototype_iterator &i) const
{
  return (node_ == i.node_);
}

bool prototype_iterator::operator!=(const prototype_iterator &i) const
{
//  return (node_ != i.node_);
  return !operator==(i);
}

prototype_iterator::self& prototype_iterator::operator++()
{
  increment();
  return *this;
}

prototype_iterator::self prototype_iterator::operator++(int)
{
  prototype_node *tmp = node_;
  increment();
  return prototype_iterator(tmp);
}

prototype_iterator::self& prototype_iterator::operator--()
{
  decrement();
  return *this;
}

prototype_iterator::self prototype_iterator::operator--(int)
{
  prototype_node *tmp = node_;
  decrement();
  return prototype_iterator(tmp);
}

prototype_iterator::pointer prototype_iterator::operator->() const
{
  return node_;
}

prototype_iterator::reference prototype_iterator::operator*() const
{
  return *node_;
}

void prototype_iterator::increment()
{
  if (node_) {
    node_ = node_->next_node();
  }
}
void prototype_iterator::decrement()
{
  if (node_) {
    node_ = node_->previous_node();
  }
}

object_store::object_store()
  : root_(new prototype_node(new object_producer<object>, "object", true))
  , first_(new object_proxy(this))
  , last_(new object_proxy(this))
  , object_deleter_(new object_deleter)
{
  prototype_node_map_.insert(std::make_pair("object", root_));
  typeid_prototype_map_[root_->producer->classname()]["object"] = root_;
  // set marker for root element
  root_->op_first = first_;
  root_->op_marker = last_;
  root_->op_last = last_;
  root_->op_first->next = root_->op_last;
  root_->op_last->prev = root_->op_first;
}

object_store::~object_store()
{
  clear(true);
  delete last_;
  delete first_;
  delete root_;
  delete object_deleter_;
}

bool
object_store::insert_prototype(object_base_producer *producer, const char *type, bool abstract, const char *parent)
{
  // set node to root node
  prototype_node *parent_node = get_prototype(parent);
  if (!parent_node) {
    //throw new object_exception("couldn't find parent prototype");
    return false;
  }

  /* try to insert new prototype node
   */
  prototype_node *node = 0;
//  cout << "DEBUG: try to insert into prototype map: [" << type << "]\n";
  t_prototype_node_map::iterator i = prototype_node_map_.find(type);
  if (i == prototype_node_map_.end()) {
    /* unknown type name try for typeid
     * (unfinished prototype)
     */
    i = prototype_node_map_.find(producer->classname());
    if (i == prototype_node_map_.end()) {
      /*
       * no typeid found, seems to be
       * a new type
       * to be sure check in typeid map
       */
      t_typeid_prototype_map::iterator j = typeid_prototype_map_.find(producer->classname());
      if (j != typeid_prototype_map_.end() && j->second.find(type) != j->second.end()) {
        /* unexpected found the
         * typeid check for type
         */
        /* type found in typeid map
         * throw exception
         */
        return false;
      } else {
        /* insert new prototype and add to
         * typeid map
         */
        // create new one
        node = new prototype_node(producer, type, abstract);
      }
    } else {
      /* prototype is unfinished,
       * finish it, insert by type name,
       * remove typeid entry and add to
       * typeid map
       */
//      cout << "DEBUG: finishing existing type: [" << type << "]\n";
      node = i->second;
      node->initialize(producer, type, abstract);
      prototype_node_map_.erase(i);
    }
  } else {
    // already inserted return iterator
    return false;
  }

  // append as child to parent prototype node
  parent_node->insert(node);
  // store prototype in map
//  cout << "DEBUG: inserting into prototype map: [" << type << "]\n";
  i = prototype_node_map_.insert(std::make_pair(type, node)).first;
  typeid_prototype_map_[producer->classname()][type] = node;

  // Check if nodes object has to many relations
  object *o = producer->create();
  relation_handler rh(*this, node);
  o->serialize(rh);
  delete o;

  return true;
}

bool object_store::clear_prototype(const char *type, bool recursive)
{
  prototype_node *node = get_prototype(type);
  if (!node) {
    //throw new object_exception("couldn't find prototype");
    return false;
  }
//  cout << "DEBUG: clearing prototype map: [" << type << "]\n";
  if (recursive) {
    // clear all objects from child nodes
    // for each child call clear_prototype(child, recursive);
//    prototype_node *child = node->next_node(node->parent);
    prototype_node *child = node->next_node();
    while (child && (child != node || child != node->parent)) {
//      cout << "DEBUG: clearing prototype map: found child [" << child->type << "]\n";
      child->clear();
//      clear_prototype(child->type.c_str(), recursive);
      child = child->next_node();
    }      
  }

  node->clear();

  return true;
}

bool object_store::remove_prototype(const char *type)
{
  prototype_node *node = get_prototype(type);
  if (!node) {
    //throw new object_exception("couldn't find prototype");
    return false;
  }

//  cout << "DEBUG: removing prototype map: [" << node->type << "]\n";

  // remove (and delete) from tree (deletes subsequently all child nodes
  // for each child call remove_prototype(child);
  while (node->first->next != node->last) {
    remove_prototype(node->first->next->type.c_str());
  }
  // and objects they're containing 
  node->clear();
  // delete prototype node as well
  // unlink node
  node->unlink();
  // get iterator
  t_prototype_node_map::iterator i = prototype_node_map_.find(node->type.c_str());
  if (i != prototype_node_map_.end()) {
    prototype_node_map_.erase(i);
  }
  // find item in typeid map
  t_typeid_prototype_map::iterator j = typeid_prototype_map_.find(node->producer->classname());
  if (j != typeid_prototype_map_.end()) {
    j->second.erase(type);
    if (j->second.empty()) {
      typeid_prototype_map_.erase(j);
    }
  } else {
//    cout << "DEBUG: Error: this could not happen!!!\n";
  }
  // delete node
  delete node;

  return true;
}

prototype_iterator object_store::find_prototype(const char *type) const
{
  return prototype_iterator(get_prototype(type));
}

prototype_node* object_store::get_prototype(const char *type) const
{
  // check for null
  if (type == 0) {
    return 0;
  }
  /*
   * first search in the prototype map
   */
  t_prototype_node_map::const_iterator i = prototype_node_map_.find(type);
  if (i == prototype_node_map_.end()) {
    /*
     * if not found search in the typeid to prototype map
     */
     t_typeid_prototype_map::const_iterator j = typeid_prototype_map_.find(type);
     if (j == typeid_prototype_map_.end()) {
       return 0;
     } else {
       const t_prototype_node_map &val = j->second;
       /*
        * if size is greater one (1) the name
        * is a typeid and has more than one prototype
        * node and therefor it is not unique and an
        * exception is thrown
        */
       if (val.size() > 1) {
         // throw exception
         return 0;
       } else {
         // return the only prototype
         return val.begin()->second;
       }
     }
  } else {
    return i->second;
  }
}

prototype_iterator object_store::begin() const
{
  return prototype_iterator(root_);
}

prototype_iterator object_store::end() const
{
  return prototype_iterator(0);
}

void object_store::clear(bool full)
{
  if (full) {
    // clear objects and prototypes
    while (root_->first->next != root_->last) {
      remove_prototype(root_->first->next->type.c_str());
    }
  } else {
    // only delete objects
    clear_prototype(root_->type.c_str(), true);
  }
  object_map_.clear();
}

bool object_store::empty() const
{
  return first_->next == last_;
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
//  out << "dumping prototype tree:\n";
  out << "digraph G {\n";
  out << "\tgraph [fontsize=10]\n";
	out << "\tnode [color=\"#0c0c0c\", fillcolor=\"#dd5555\", shape=record, style=\"rounded,filled\", fontname=\"Verdana-Bold\"]\n";
	out << "\tedge [color=\"#0c0c0c\"]\n";
  do {
    int d = depth(node);
    for (int i = 0; i < d; ++i) out << " ";
    out << *node;
    node = node->next_node();
  } while (node);
  out << "}" << std::endl;
}

void object_store::dump_objects(std::ostream &out) const
{
  out << "dumping all objects\n";

  object_proxy *op = first_;
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
  prototype_node *node = get_prototype(type);
  if (node) {
    return node->producer->create();
  } else {
    return 0;
  }
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
//    delete *i;
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
    // throw exception
    return NULL;
  }
  // find prototype node
//  cout << "DEBUG: inserting object of type [" << typeid(*o).name() << "]\n";
//  t_prototype_node_map::iterator i = prototype_node_map_.find(typeid(*o).name());
  prototype_node *node = get_prototype(typeid(*o).name());
  if (!node) {
//  if (i == prototype_node_map_.end()) {
    // raise exception
    std::string msg("couldn't insert element of type [" + std::string(typeid(*o).name()) + "]");
    throw new object_exception(msg.c_str());
  }
//  prototype_node *node = i->second;
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
      // throw exception
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
  oproxy->node = node;
  // set this into persistent object
  o->proxy_ = oproxy;
  // notify observer
  if (notify) {
    std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_insert, _1, o));
  }
  // insert element into hash map for fast lookup
  object_map_[o->id()] = oproxy;
  // return new object
  //std::cout << "created object (" << std::right << std::setfill(' ') << std::setw(4) << o->id() << ") of type [" << o->classname() << "] proxy " << *oproxy << "\n";
  return o;
}

object*
object_store::import_object(object *o)
{
  /*
   * find prototype node
   */
  t_prototype_node_map::iterator i = prototype_node_map_.find(typeid(*o).name());
  if (i == prototype_node_map_.end()) {
    // raise exception
    std::string msg("couldn't insert element of type [" + std::string(typeid(*o).name()) + "]");
    throw new object_exception(msg.c_str());
  }
  
  //prototype_node *node = i->second;
  
  //relation::const_iterator first = node->relation_.begin();
  //relation::const_iterator last = node->relation_.end();
  //while (first != last) {
    //relation rel(*first++);

    ///*
     //* get back link of parent object
     //*/
    //o->get(rel.name, parent);
    
    ///*
     //* get parents list
     //*/
    //parent->get(rel.list_name, container);
    
    ///*
     //* append object to container
     //*/
    //container->append_proxy(o->proxy_);
  //}
  return 0;
}

bool
object_store::remove(object *o)
{
  // check if object tree is deletable
  if (!object_deleter_->is_deletable(o)) {
    return false;
  }
  
  object_deleter::iterator first = object_deleter_->begin();
  object_deleter::iterator last = object_deleter_->end();
  
  while (first != last) {
    if (!first->second.ignore) {
      remove_object((first++)->second.obj, true);
    } else {
      ++first;
    }
  }
	return true;
}
bool
object_store::remove_object(object *o, bool notify)
{
  // find prototype node
  if (!o->proxy_->node) {
    throw new object_exception("couldn't remove object, no proxy");
  }
  
  prototype_node *node = get_prototype(o->proxy_->node->type.c_str());
  if (!node) {
    throw new object_exception("couldn't find node for object");
  }
  
  /*
  t_prototype_node_map::iterator i = prototype_node_map_.find(typeid(*o).name());
  if (i == prototype_node_map_.end()) {
    // raise exception
    //throw new object_exception("couldn't insert element of type [" + o->type() + "]\n");
    return false;
  }
  */

  if (object_map_.erase(o->id()) != 1) {
    // couldn't remove object
    // throw exception
    return false;
  }

  remove_proxy(node, o->proxy_);

  if (notify) {
    // notify observer
    std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_delete, _1, o));
  }
  // set object in object_proxy to null
  object_proxy *op = o->proxy_;
  // delete node
  delete op;
  // set node of proxy to NULL
  return true;
}

bool
object_store::remove(object_container &oc)
{
  /**************
   * 
   * remove all objects from container
   * and first and last sentinel
   * 
   **************/
  // check if object tree is deletable
  if (!object_deleter_->is_deletable(oc)) {
    return false;
  }
  object_deleter::iterator first = object_deleter_->begin();
  object_deleter::iterator last = object_deleter_->end();
  
  while (first != last) {
    if (!first->second.ignore) {
      remove_object((first++)->second.obj, true);
    } else {
      ++first;
    }
  }
  oc.uninstall();
  return true;
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
    return NULL;
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

void object_store::insert_proxy(prototype_node *node, object_proxy *oproxy)
{
  // check count of object in subtree
  if (node->count >= 2) {
    /*************
     *
     * there are more than two objects (normal case)
     * insert before last last
     *
     *************/
    //cout << "more than two elements: inserting " << *o << " before second last (" << *node->op_marker->prev->obj << ")\n";
    oproxy->link(node->op_marker->prev);
  } else if (node->count == 1) {
    /*************
     *
     * there is one object in subtree
     * insert as first; adjust "left" marker
     *
     *************/
    /*if (node->op_marker->prev->obj) {
      cout << "one element in list: inserting " << *o << " as first (before: " << *node->op_marker->prev->obj << ")\n";
    } else {
      cout << "one element in list: inserting " << *o << " as first (before: [0])\n";
    }*/
    oproxy->link(node->op_marker->prev);
    node->adjust_left_marker(oproxy->next, oproxy);
  } else /* if (node->count == 0) */ {
    /*************
     *
     * there is no object in subtree
     * insert as last; adjust "right" marker
     *
     *************/
    /*if (node->op_marker->obj) {
      cout << "list is empty: inserting " << *o << " as last before " << *node->op_marker->obj << "\n";
    } else {
      cout << "list is empty: inserting " << *o << " as last before [0]\n";
    }*/
    oproxy->link(node->op_marker);
    node->adjust_left_marker(oproxy->next, oproxy);
    node->adjust_right_marker(oproxy->prev, oproxy);
  }
  // set prototype node
  oproxy->node = node;
  // adjust size
  ++node->count;
}

void object_store::remove_proxy(prototype_node *node, object_proxy *oproxy)
{
  if (oproxy == node->op_first->next) {
    // adjust left marker
    //cout << "remove: object proxy is left marker " << *o << " before second last (" << *node->op_marker->prev->obj << ")\n";
    node->adjust_left_marker(node->op_first->next, node->op_first->next->next);
    //adjust_left_marker(node, node->op_first->next->next);
  }
  if (oproxy == node->op_marker->prev) {
    // adjust right marker
    node->adjust_right_marker(oproxy, node->op_marker->prev->prev);
    //adjust_right_marker(node, o->proxy_, node->op_marker->prev->prev);
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
