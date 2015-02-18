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

#ifndef OBJECT_STORE_HPP
#define OBJECT_STORE_HPP

#include "object/object_ptr.hpp"
#include "object/prototype_tree.hpp"
#include "object/object_producer.hpp"
#include "object/object_deleter.hpp"

#include "tools/sequencer.hpp"
#include "object_exception.hpp"

#include <memory>
#include <unordered_map>

#include <string>
#include <ostream>
#include <list>

#ifdef _MSC_VER
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

class object;
struct object_proxy;
struct prototype_node;
class object_observer;
class object_container;

/**
 * @class object_store
 * @brief A class that stores all kind of objects.
 * 
 * This class is the main container class for all
 * objects. To manage the internal list of objects
 * the store must know the object class hierarchy.
 *
 * Therefor an object prototype tree holds the object
 * hierarchy representation including a producer class
 * object of all known types.
 *
 * 
 */
class OOS_API object_store
{
private:
  typedef std::unordered_map<long, object_proxy*> t_object_proxy_map;
  typedef std::unordered_map<std::string, prototype_node*> t_prototype_map;

public:
  /**
   * Create an empty object store.
   */
	object_store();

  /**
   * Destroys all prototypes, objects and observers in store.
   */
	~object_store();

  /*
   * Returns the prototype tree
   *
   * @return The prototype tree
   */
  prototype_tree& prototypes();

  /*
   * Returns the prototype tree
   *
   * @return The prototype tree
   */
  const prototype_tree& prototypes() const;

  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * constist of a producer and a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given.
   * 
   * @param producer The producer object produces a new object of a specific type.
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers object is treated as an abstract node.
   * @param parent   The name of the parent type.
   * @return         Returns new inserted prototype iterator.
   */
	prototype_iterator insert_prototype(object_base_producer *producer, const char *type, bool abstract = false, const char *parent = "object");

  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * constist of a producer and a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given. The producer is automatically created via the template
   * parameter.
   * 
   * @tparam T       The type of the prototype node
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers object is treated as an abstract node.
   * @return         Returns new inserted prototype iterator.
   */
  template < class T >
  prototype_iterator insert_prototype(const char *type, bool abstract = false)
  {
    return insert_prototype(new object_producer<T>, type, abstract, "object");
  }

  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * constist of a producer and a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given. The producer is automatically created via the template
   * parameter.
   * 
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers object is treated as an abstract node.
   * @return         Returns new inserted prototype iterator.
   */
  template < class T, class S >
  prototype_iterator insert_prototype(const char *type, bool abstract = false)
  {
    return insert_prototype(new object_producer<T>, type, abstract, typeid(S).name());
  }

  /**
   * Clears a prototype node. All objects will be deleted. If
   * the recursive flag is set all objects from the children nodea
   * will be deleted as well.
   * 
   * @param type The name of the type to remove.
   * @param recursive If set, also the object in children nodes are deleted.
   * @return Returns true if the type was found and successfully cleared.
   */
//  void clear_prototype(const char *type, bool recursive);

  /**
   * Removes an object prototype from the prototype tree. All children
   * nodes and all objects are also removed.
   * 
   * @param type The name of the type to remove.
   */
  void remove_prototype(const char *type);

  /**
   * @brief Finds prototype node.
   *
   * Finds and returns prototype node iterator identified
   * by the given name or classname (typeid). If the
   * prototype couldn't be found prototype_iterator end
   * is returned.
   *
   * @param type Name or class name of the prototype
   * @return Returns a prototype iterator.
   */
  prototype_iterator find_prototype(const char *type);
  const_prototype_iterator find_prototype(const char *type) const;

  /**
   * @brief Finds prototype node by template type.
   * @tparam Template type.
   *
   * Finds and returns prototype node iterator identified
   * by the given template typeid. If the prototype couldn't
   * be found prototype_iterator end is returned.
   *
   * @return Returns a prototype iterator.
   */
  template < class T >
  prototype_iterator find_prototype()
  {
    return find_prototype(typeid(T).name());
  }
  template < class T >
  const_prototype_iterator find_prototype() const
  {
    return find_prototype(typeid(T).name());
  }

  /**
   * Return the first prototype node.
   *
   * @return The first prototype node iterator.
   */
  const_prototype_iterator begin() const;
  prototype_iterator begin();

  /**
   * Return the last prototype node.
   *
   * @return The last prototype node iterator.
   */
  const_prototype_iterator end() const;
  prototype_iterator end();

  /**
   * Removes all inserted prototypes and all inserted objects.
   */
  void clear(bool full = false);

  /**
   * Returns true if the object_store
   * conatins no elements (objects)
   * 
   * @return True on empty object_store.
   */
  bool empty() const;

  /**
   * Dump all object to a given stream
   *
   * @param out The stream to the objects dump on.
   */
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
		return object_ptr<Y>(insert_object(o, true));
	}

  /**
   *
   */
  template < class Y >
  object_ptr<Y> insert(const object_ptr<Y> &optr)
  {
    if (!optr.proxy_) {
      throw object_exception("object pointer is null");
    }
    insert_proxy(optr.proxy_);
    return optr;
  }

  /**
   * Inserts an object_container into the object store. Subsequently the
   * object_container is initialized.
   * 
   * @param oc The object_container to insert.
   */
  void insert(object_container &oc);
  
  /**
   * Returns true if the underlaying
   * object is removable.
   * 
   * @param o The object to check.
   * @return True if object is removable.
   */
  bool is_removable(const object_base_ptr &o);

  /**
   * Removes an object from the object store. After successfull
   * removal the object is set to zero and isn't valid any more.
   * 
   * Before removal is done a reference and pointer counter check
   * is done. If at least one counter is greater than zero the
   * object can't be removed and false is returned.
   * 
   * @throw object_exception
   * @param o Object to remove.
   */
	void remove(object_base_ptr &o);
  
  /**
   * Removes an object_container from object store. All elements of the
   * container are removed from the store after a successfull reference and
   * pointer counter check.
   * 
   * @throw object_exception
   * @param oc The object vector to remove.
   */
  void remove(object_container &oc);

  /*
  template < class InputIterator >
  void insert(InputIterator first, InputIterator last)
  {
   // std::iterator_traits<InputIterator>::value_type *o = *first;
  }
  */
  
  /**
   * @brief Register an observer with the object store
   *
   * @param observer The object observer to register.
   */
  void register_observer(object_observer *observer);

  /**
   * @brief Unregisters an observer from the object store
   *
   * @param observer The object observer to unregister.
   */
  void unregister_observer(object_observer *observer);

  /**
   * @brief Creates and inserts an object proxy object.
   * 
   * An object proxy object is created and inserted
   * into the internal proxy hash map. The proxy won't
   * be linked into the main object proxy list until
   * it gets a valid object.
   * 
   * If the object proxy couldn't be created the method
   * returns null
   * 
   * @param id Unique id of the object proxy.
   * @return An object proxy object or null.
   */
  object_proxy *create_proxy(long id);

  /**
   * @brief Delete proxy from map
   *
   * Deletes the proxy with the given id
   * from map.
   *
   * @param id Id of proxy to delete
   * @return Returns true if deletion was successfully
   */
  bool delete_proxy(long id);

  /**
   * @brief Finds object proxy with id
   *
   * Try to find the object proxy with given id in
   * object stores proxy map. If object can't be found
   * NULL is returned.
   *
   * @param id ID of object proxy to find
   * @return On success it returns an object proxy on failure null
   *
   */
  object_proxy *find_proxy(long id) const;

  /**
   * @brief Inserts the proxy into a prototype list
   * 
   * @param node Prototype into which the proxy will be inserted.
   * @param oproxy Object proxy to insert
   */
  void insert_proxy(const prototype_iterator &node, object_proxy *oproxy);

  /**
  * @brief Inserts a new proxy into the object store
  *
  * @param oproxy Object proxy to insert
  */
  void insert_proxy(object_proxy *oproxy);

  /**
   * @brief Removes an object proxy from a prototype list
   *
   * @param node Prototype from which the proxy will be removed.
   * @param oproxy Object proxy to remove
   */
  void remove_proxy(prototype_node *node, object_proxy *oproxy);

  /**
   * @brief Exchange the sequencer strategy.
   * 
   * Exchange the sequencer strategy of this object_store.
   * The current sequencer is replaced by the new one and
   * the current sequence is synced with the new sequencer
   * sequence.
   * The old strategy is returned.
   * 
   * @param seq The new sequencer strategy object.
   * @return The old sequencer startegy implementation.
   */
  sequencer_impl_ptr exchange_sequencer(const sequencer_impl_ptr &seq);

private:
  friend class object_creator;
  friend class object_deleter;
  friend class object_serializer;
  friend class restore_visitor;
  friend class object_container;
  friend class object;

private:
  void mark_modified(object_proxy *oproxy);

  void remove(object_proxy *proxy);
	object_proxy* insert_object(object *o, bool notify);
	void remove_object(object_proxy *proxy, bool notify);
	
  void link_proxy(object_proxy *base, object_proxy *next);
  void unlink_proxy(object_proxy *proxy);

private:
  prototype_tree prototype_tree_;

  t_object_proxy_map object_map_;

  sequencer seq_;

  typedef std::list<object_observer*> t_observer_list;
  t_observer_list observer_list_;

  object_deleter object_deleter_;
};

}

#endif /* OBJECT_STORE_HPP */
