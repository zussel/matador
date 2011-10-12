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
#include "object/object_atomizer.hpp"
#include "object/prototype_node.hpp"

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
#include <map>

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
  #define OOS_API
#endif

namespace oos {

class OOS_API object_observer
{
public:
  virtual ~object_observer() {}
  
  virtual void on_insert(object *o) = 0;
  virtual void on_update(object *o) = 0;
  virtual void on_delete(object *o) = 0;
};

template < class T > class object_list;
class object_list_base;

class OOS_API object_base_producer {
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

class OOS_API object_deleter : public object_atomizer
{
private:
  typedef struct t_object_count_struct
  {
    t_object_count_struct(object *o, bool ignr = true)
      : obj(o)
      , ref_count(o->proxy_->ref_count)
      , ptr_count(o->proxy_->ptr_count)
      , ignore(ignr)
    {}
    object *obj;
    unsigned long ref_count;
    unsigned long ptr_count;
    bool ignore;
  } t_object_count;

private:
  typedef std::map<unsigned long, t_object_count> t_object_count_map;

public:
  typedef t_object_count_map::iterator iterator;

  object_deleter();
  virtual ~object_deleter();

  bool is_deletable(object *obj);

  virtual void read_object(const char*, base_object_ptr &x);
  virtual void read_object_list(const char*, object_list_base &);

  iterator begin();
  iterator end();

private:
  void check_object(object *o, bool is_ref);

private:
  t_object_count_map object_count_map;
};

//struct prototype_node;

/**
 * @class object_store
 * @brief A class that stores all kind of objects.
 * 
 * 
 */
class OOS_API object_store
{
private:
  typedef std::tr1::unordered_map<long, object*> t_object_map;
	typedef std::tr1::unordered_map<std::string, prototype_node*> t_prototype_node_map;

public:
  /**
   * Create an empty object store.
   */
	object_store();

  /**
   * Destroys all prototypes, objects and observers in store.
   */
	~object_store();
	
  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * constist of a producer and a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given.
   * 
   * @param producer The producer object produces a new object of a specific type.
   * @param type     The unique name of the type.
   * @param parent   The name of the parent type.
   * @return Returns true if the prototype was inserted successfully.
   */
	bool insert_prototype(object_base_producer *producer, const char *type, const char *parent = "OBJECT");

  template < class T >
  bool insert_prototype(const char *type, const char *parent = "OBJECT")
  {
    return insert_prototype(new object_producer<T>, type, parent);
  }
  /**
   * Removes an object prototype from the prototype tree. All children
   * nodes and all objects are also removed.
   * 
   * @param type The name of the type to remove.
   * @return Returns true if the type was found and successfully removed
   */
  bool remove_prototype(const char *type);

  /**
   * Removes all inserted prototypes and all inserted objects.
   */
  void clear();
	
	void dump_prototypes(std::ostream &out) const;
	void dump_objects(std::ostream &out) const;

  /**
   * Creates an object of the given type name.
   * 
   * @param type Typename of the object to create.
   * @return The created object on success or NULL if the type couldn't be found.
   */
	object* create(const char *type) const;

  /**
   * Inserts an object of a specfic type. On successfull insertion
   * an object_ptr element with the inserted object is returned.
   * 
   * @param o Object to be inserted.
   * @return Inserted object contained by an object_ptr on success.
   */
  template < class Y >
	object_ptr<Y> insert(Y *o)
  {
		return object_ptr<Y>(insert_object(o));
	}
  
  template < class Y >
  void insert(object_list<Y> &ol)
  {
    // set object store
    insert_object_list(ol);
  }
  
	template < class Y >
	bool remove(object_ptr<Y> &o)
  {
    // check if object tree is deletable
    if (!object_deleter_.is_deletable(o.get())) {
      return false;
    }
    
    object_deleter::iterator first = object_deleter_.begin();
    object_deleter::iterator last = object_deleter_.end();
    
    while (first != last) {
      if (!first->second.ignore) {
        remove_object((first++)->second.obj);
      } else {
        ++first;
      }
    }
		return true;
	}
  
  template < class Y >
  bool remove(object_list<Y> &ol)
  {
    return false;
  }

  template < class InputIterator >
  void insert(InputIterator first, InputIterator last)
  {
   // std::iterator_traits<InputIterator>::value_type *o = *first;
  }
  
  void mark_modified(const object_proxy_ptr &oproxy);

  void register_observer(object_observer *observer);
  void unregister_observer(object_observer *observer);

private:
  template < class T > friend class object_view;
  friend class object_creator;
  friend class object_deleter;

private:
	object* insert_object(object *o);
	bool remove_object(object *o);
	
  bool insert_object_list(object_list_base &olb);

  void link_proxy(const object_proxy_ptr &base, const object_proxy_ptr &next);
  void unlink_proxy(const object_proxy_ptr &proxy);

private:
  std::auto_ptr<prototype_node> root_;
 
  t_prototype_node_map prototype_node_name_map_;
  t_prototype_node_map prototype_node_type_map_;
  t_object_map object_map_;
  long id_;
  
  typedef std::list<object_observer*> t_observer_list;
  t_observer_list observer_list_;

  object_proxy_ptr first_;
  object_proxy_ptr last_;
  
  typedef std::list<object_proxy_ptr> t_object_proxy_list;
  t_object_proxy_list deleted_object_proxy_list_;

  object_deleter object_deleter_;
};

}

#endif /* OBJECTBAG_HPP */
