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

#ifndef OBJECTBAG_HPP
#define OBJECTBAG_HPP

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_proxy.hpp"

#ifdef WIN32
#include <memory>
#include <unordered_map>
#else
#include <tr1/memory>
#include <tr1/unordered_map>
#endif

#include <typeinfo>
#include <string>
#include <ostream>
#include <list>

namespace oos {

class object_observer
{
public:
  virtual ~object_observer() {}
  
  virtual void on_insert(object *o) = 0;
  virtual void on_update(object *o) = 0;
  virtual void on_delete(object *o) = 0;
};

template < class T > class object_list;
class object_list_base;

class object_base_producer {
public:
  virtual ~object_base_producer() {}
  virtual object* create() const = 0;
  virtual const char *classname() const = 0;
};

/**
 * @class object_producer
 * @brief produces a new object of type T
 * 
 * These producers a placed in the object type tree and whenever
 * a new object of a certain type is requested, the appropiate
 * producer is used to create the object.
 * It supports also a method to determine the name of the class
 * which is produced.
 */
template < class T >
class object_producer : public object_base_producer {
public:
  virtual ~object_producer() {}
  /**
   * Creates and returns a new object of type T
   * @return new object of type T
   */
  virtual object* create() const {
    return new T;
  }
  /**
   * Returns the name of the class which is created
   * @return the name of the produced class
   */
  virtual const char *classname() const {
    return typeid(T).name();
  }
};

class prototype_node;

class object_store
{
private:
  typedef std::tr1::unordered_map<long, object*> t_object_map;
	typedef std::tr1::unordered_map<std::string, prototype_node*> t_prototype_node_map;

public:
  /*
   * Create an empty object store
   */
	object_store();
  /*
   * Destroys all objects in store
   */
	~object_store();
	
  /*
   * Insert a new object prototype into the prototype tree.
   */
	bool insert_prototype(object_base_producer *producer, const char *type, const char *parent = "OBJECT");
  /*
   * Removes an object prototype from the prototype tree and
   * removes also the created objects from store
   */
  bool remove_prototype(const char *type);
  /**
   * Removes all inserted prototypes
   */
  void clear_prototypes();
	
	void dump_prototypes(std::ostream &out) const;
	void dump_objects(std::ostream &out) const;

	object* create(const char *type) const;
	template < class Y >
	object_ptr<Y> insert(Y *o) {
		return object_ptr<Y>(insert_object(o));
	}
  template < class Y >
  void insert(object_list<Y> &ol) {
    // set object store
    insert_object_list(ol);
  }
	template < class Y >
	bool remove(object_ptr<Y> o) {
		return remove_object(o.get());
	}
  template < class InputIterator >
  void insert(InputIterator first, InputIterator last) {
   // std::iterator_traits<InputIterator>::value_type *o = *first;
  }
  void mark_modified(object_proxy *oproxy);

  void register_observer(object_observer *observer);
  void unregister_observer(object_observer *observer);

  void cleanup();
  
private:
  template < class T > friend class object_view;
  friend class object_creator;
  friend class object_deleter;

private:
	object* insert_object(object *o);
	bool remove_object(object *o);
	
  bool insert_object_list(object_list_base &olb);

  //void adjust_left_marker(prototype_node *node, object_proxy *oproxy);
  //void adjust_right_marker(prototype_node *node, object_proxy *old_proxy, object_proxy *new_proxy);

private:
  prototype_node *root_;
  t_prototype_node_map prototype_node_name_map_;
  t_prototype_node_map prototype_node_type_map_;
  t_object_map object_map_;
  long id_;
  
  typedef std::list<object_observer*> t_observer_list;
  t_observer_list observer_list_;

  object_proxy *first_;
  object_proxy *last_;
  
  typedef std::list<object_proxy*> t_object_proxy_list;
  t_object_proxy_list deleted_object_proxy_list_;
};

}

#endif /* OBJECTBAG_HPP */
