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
#include "object/object_exception.hpp"
#include "object/object_observer.hpp"
#include "object/primary_key_reader.hpp"

#include "tools/sequencer.hpp"

#include <memory>
#include <unordered_map>
#include <algorithm>
#include <stack>

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

class serializable;
class object_proxy;
class prototype_node;
class object_container;

/**
 * @cond OOS_DEV
 * @class object_inserter
 * @brief Creates objects and object_lists
 *
 * When an serializable is inserted into the serializable store
 * subsequently other serializable must be created and
 * inserted into the serializable store.
 * This class does these tasks.
 */
class object_inserter
{
public:
  /**
   * @brief Creates an object_inserter instance.
   *
   * An object_inserter instance ist created for a
   * given object_store. The notify flag tells the
   * object_inserter wether the observers should be
   * notified or not.
   *
   * @param ostore The object_store.
   */
  object_inserter(object_store &ostore);
  ~object_inserter();

  template < class T >
  void insert(object_proxy *proxy, T *o);
  template < class T >
  void deserialize(T &x);
  template < class T >
  void deserialize(const char*, T&) {}

  void deserialize(const char*, char*, size_t) {}

//  template < class T >
//  void serialize(const char *, object_ptr<T> &x)
  template < class T, bool TYPE >
  void deserialize(const char*, object_holder<T, TYPE> &x);

  void deserialize(const char*, object_container &x);

private:
  typedef std::set<object_proxy*> t_object_proxy_set;

  t_object_proxy_set object_proxies_;

  std::stack<object_proxy*> object_proxy_stack_;

  object_store &ostore_;
};
/// @endcond

/**
 * @class object_store
 * @brief A class that stores all kind of objects.
 * 
 * This class is the main container class for all
 * objects. To manage the internal list of objects
 * the store must know the serializable class hierarchy.
 *
 * Therefor an serializable prototype tree holds the serializable
 * hierarchy representation including a producer class
 * serializable of all known types.
 */
class OOS_API object_store
{
public:
  /**
   * Create an empty serializable store.
   */
	object_store();

  /**
   * Destroys all prototypes, objects and observers in store.
   */
	~object_store();

  /**
   * Returns the prototype tree
   *
   * @return The prototype tree
   */
  prototype_tree& prototypes();

  /**
   * Returns the prototype tree
   *
   * @return The prototype tree
   */
  const prototype_tree& prototypes() const;

  /**
   * Inserts a new serializable prototype into the prototype tree. The prototype
   * constist of a producer and a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given.
   * 
   * @param producer The producer serializable produces a new serializable of a specific type.
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @param parent   The name of the parent type.
   * @return         Returns new inserted prototype iterator.
   */
  template < class T >
	prototype_iterator attach(const char *type, bool abstract = false, const char *parent = nullptr)
  {
    return prototype_tree_.attach<T>(type, abstract, parent);
  }

  /**
   * Inserts a new serializable prototype into the prototype tree. The prototype
   * constist of a producer and a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given. The producer is automatically created via the template
   * parameter.
   * 
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @return         Returns new inserted prototype iterator.
   */
  template < class T, class S >
  prototype_iterator attach(const char *type, bool abstract = false)
  {
    return prototype_tree_.attach<T, S>(type, abstract);
  }

  /**
   * Removes an serializable prototype from the prototype tree. All children
   * nodes and all objects are also removed.
   * 
   * @param type The name of the type to remove.
   */
  void detach(const char *type);

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

  /**
   * Return the first prototype node.
   *
   * @return The first prototype node iterator.
   */
  prototype_iterator begin();

  /**
   * Return the last prototype node.
   *
   * @return The last prototype node iterator.
   */
  const_prototype_iterator end() const;

  /**
   * Return the last prototype node.
   *
   * @return The last prototype node iterator.
   */
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
   * Dump all serializable to a given stream
   *
   * @param out The stream to the objects dump on.
   */
  void dump_objects(std::ostream &out) const;

  /**
   * Creates an serializable of the given type name.
   * 
   * @param type Typename of the serializable to create.
   * @return The created serializable on success or NULL if the type couldn't be found.
   */
  template < class T >
	T* create() const {
    const_prototype_iterator node = prototype_tree_.find<T>();
    if (node != prototype_tree_.end()) {
      return new T;
    } else {
      throw object_exception("unknown prototype type");
    }
  }

  /**
   * Inserts an serializable of a specfic type. On successfull insertion
   * an object_ptr element with the inserted serializable is returned.
   * 
   * @param o Object to be inserted.
   * @return Inserted serializable contained by an object_ptr on success.
   */
  template < class Y >
	object_ptr<Y> insert(Y *o)
  {
//    return insert(object_ptr<Y>(o));
		return object_ptr<Y>(insert_object(o, true));
	}

  /**
   * Inserts a given object_ptr of specific type.
   * On successfull insertion an object_ptr element
   * with the inserted serializable is returned.
   *
   * @param optr object_ptr to be inserted.
   * @return Inserted serializable contained by an object_ptr on success.
   */
  template < class Y >
  object_ptr<Y> insert(const object_ptr<Y> &optr)
  {
    if (!optr.proxy_) {
      throw object_exception("serializable pointer is null");
    }
    //if (optr.proxy_->id() > 0) {
    //  throw object_exception("serializable id is greater zero");
    //}

    insert_proxy(optr.proxy_, optr.get());
    return optr;
  }

  /**
   * Inserts an object_container into the serializable store. Subsequently the
   * object_container is initialized.
   * 
   * @param oc The object_container to insert.
   */
  void insert(object_container &oc);
  
  /**
   * Returns true if the underlaying
   * serializable is removable.
   * 
   * @param o The serializable to check.
   * @return True if serializable is removable.
   */
  template < class T, bool TYPE >
  bool is_removable(const object_holder<T, TYPE> &o)
  {
    return object_deleter_.is_deletable(o.proxy_, o.get());
  }

  /**
   * Removes an serializable from the serializable store. After successfull
   * removal the serializable is set to zero and isn't valid any more.
   * 
   * Before removal is done a reference and pointer counter check
   * is done. If at least one counter is greater than zero the
   * serializable can't be removed and false is returned.
   * 
   * @throw object_exception
   * @param o Object to remove.
   */
  template < class T, bool TYPE >
	void remove(object_holder<T, TYPE> &o)
  {
    if (o.proxy_ == nullptr) {
      throw object_exception("serializable proxy is nullptr");
    }
    if (o.proxy_->node() == nullptr) {
      throw object_exception("prototype node is nullptr");
    }
    // check if serializable tree is deletable
    if (!object_deleter_.is_deletable<T>(o.proxy_, o.get())) {
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
  
  /**
   * Removes an object_container from serializable store. All elements of the
   * container are removed from the store after a successfull reference and
   * pointer counter check.
   * 
   * @throw object_exception
   * @param oc The serializable vector to remove.
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
   * @brief Register an observer with the serializable store
   *
   * @param observer The serializable observer to register.
   */
  void register_observer(object_observer *observer);

  /**
   * @brief Unregisters an observer from the serializable store
   *
   * @param observer The serializable observer to unregister.
   */
  void unregister_observer(object_observer *observer);

  /**
   * @brief Creates and inserts an serializable proxy serializable.
   * 
   * An serializable proxy serializable is created and inserted
   * into the internal proxy hash map. The proxy won't
   * be linked into the main serializable proxy list until
   * it gets a valid serializable.
   *
   * If id is zero a new id is generated.
   *
   * If the serializable proxy couldn't be created the method
   * returns nullptr
   *
   * @param o The object set into the new object proxy.
   * @param node Objects corresponding prototype_node.
   * @param id Unique id of the serializable proxy.
   * @return An serializable proxy serializable or null.
   */
  template < class T >
  object_proxy *create_proxy(T *o)
  {
    std::unique_ptr<object_proxy> proxy(new object_proxy(o, seq_.next(), this));
    return object_map_.insert(std::make_pair(seq_.current(), proxy.release())).first->second;
  }

//  object_proxy *create_proxy(unsigned long id);

  /**
   * @brief Delete proxy from map
   *
   * Deletes the proxy with the given id
   * from map.
   *
   * @param id Id of proxy to delete
   * @return Returns true if deletion was successfully
   */
  bool delete_proxy(unsigned long id);

  /**
   * @brief Finds serializable proxy with id
   *
   * Try to find the serializable proxy with given id in
   * serializable stores proxy map. If serializable can't be found
   * NULL is returned.
   *
   * @param id ID of serializable proxy to find
   * @return On success it returns an serializable proxy on failure null
   *
   */
  object_proxy *find_proxy(unsigned long id) const;

  /**
   * @brief Inserts a new proxy into the object store
   *
   * @param oproxy Object proxy to insert
   * @param notify Indicates wether all observers should be notified.
   * @param is_new Proxy is a new not inserted proxy, skip object store check
   */
  template < class T >
  object_proxy* insert_proxy(object_proxy *oproxy, T *o, bool notify = true, bool is_new = true);

  /**
   * @brief Registers a new proxy
   *
   * Proxy will be registered in object store. It
   * gets an id and is insert into object map.
   * If proxy has already an id an exception is thrown.
   * If proxy doesn't have a prototype_node an exception is thrown.
   *
   * @param oproxy The object_proxy to register
   * @return The registered object_proxy
   * @throws object_exception
   */
  object_proxy* register_proxy(object_proxy *oproxy);

  /**
   * @brief Exchange the sequencer strategy.
   * 
   * Exchange the sequencer strategy of this object_store.
   * The current sequencer is replaced by the new one and
   * the current sequence is synced with the new sequencer
   * sequence.
   * The old strategy is returned.
   * 
   * @param seq The new sequencer strategy serializable.
   * @return The old sequencer startegy implementation.
   */
  sequencer_impl_ptr exchange_sequencer(const sequencer_impl_ptr &seq);

private:
  friend class object_inserter;
  friend class object_deleter;
  friend class object_serializer;
  friend class restore_visitor;
  friend class object_container;
  friend class object_base_ptr;

private:
  void mark_modified(object_proxy *oproxy);

  template < class T >
	object_proxy* insert_object(T *o, bool notify);
	void remove_object(object_proxy *proxy, bool notify);

//  object_proxy *initialze_proxy(object_proxy *oproxy, prototype_iterator &node, bool notify);

private:
  prototype_tree prototype_tree_;

//  typedef std::unordered_map<serializable*, object_proxy*> t_serializable_proxy_map;
//  t_serializable_proxy_map serializable_map_;

  typedef std::unordered_map<long, object_proxy*> t_object_proxy_map;
  t_object_proxy_map object_map_;

  sequencer seq_;

  typedef std::list<object_observer*> t_observer_list;
  t_observer_list observer_list_;

  object_deleter object_deleter_;
  object_inserter object_inserter_;
};

template < class T >
object_proxy* object_store::insert_object(T *o, bool notify)
{
  // find type in tree
  if (!o) {
    throw object_exception("serializable is null");
  }

  // find prototype node
  prototype_iterator node = prototype_tree_.find(typeid(*o).name());
  if (node == prototype_tree_.end()) {
    // raise exception
    throw object_exception("unknown object type");
  }
  // retrieve and set new unique number into serializable
  /* object doesn't exist in map
   * if object has a valid id, update
   * the sequencer else assign new
   * unique id
   */
  object_proxy *oproxy = create_proxy(o);
  if (!oproxy) {
    throw object_exception("couldn't create serializable proxy");
  }

  if (oproxy->has_identifier()) {
    // if object has primary key of type short, int or long
    // set the id of proxy as value
//    identifier_assigner<unsigned long>::assign(oproxy->id(), o);
    primary_key_reader<unsigned long> reader(oproxy->id());
    oos::access::deserialize(reader, *o);
  }

  node->insert(oproxy);

  // initialize object
  object_inserter_.insert(oproxy, o);
  // set this into persistent serializable
  // notify observer
  if (notify) {
    std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_insert, std::placeholders::_1, oproxy));
  }
  // insert element into hash map for fast lookup
  object_map_[oproxy->id()] = oproxy;

  return oproxy;
}

template < class T >
object_proxy* object_store::insert_proxy(object_proxy *oproxy, T *o, bool notify, bool is_new)
{
  if (!oproxy->obj()) {
    throw object_exception("serializable of proxy is null pointer");
  }

  if (is_new && oproxy->ostore()) {
    throw object_exception("serializable proxy already in serializable store");
  }

  // find prototype node
  prototype_iterator node = prototype_tree_.find(oproxy->classname());
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

  // initialize object
  object_inserter_.insert(oproxy, o);
  // set this into persistent serializable
  // notify observer
  if (notify) {
    std::for_each(observer_list_.begin(), observer_list_.end(), std::bind(&object_observer::on_insert, std::placeholders::_1, oproxy));
  }
  // insert element into hash map for fast lookup
  object_map_[oproxy->id()] = oproxy;

  return oproxy;
}

template < class T >
void object_inserter::insert(object_proxy *proxy, T *o)
{
  object_proxies_.clear();
  while (!object_proxy_stack_.empty()) {
    object_proxy_stack_.pop();
  }

  object_proxies_.insert(proxy);
  object_proxy_stack_.push(proxy);
  if (proxy->obj()) {
    oos::access::deserialize(*this, *o);
  }
}

template < class T >
void object_inserter::deserialize(T &x)
{
  oos::access::deserialize(*this, x);
}

template < class T, bool TYPE >
void object_inserter::deserialize(const char*, object_holder<T, TYPE> &x)
{
  // mark serializable pointer as internal
  x.is_internal_ = true;

  if (!x.proxy_) {
    return;
  }

  if (x.is_reference()) {
    x.proxy_->link_ref();
  } else if (x.ptr() && x.id()){
    x.proxy_->link_ptr();
  }
  if (x.ptr()) {
    bool new_object = object_proxies_.insert(x.proxy_).second;
    if (x.id()) {
      // do the pointer count
      if (new_object) {
        object_proxy_stack_.push(x.proxy_);
        oos::access::deserialize(*this, *x.get());
        object_proxy_stack_.pop();
      }
    } else if (new_object){
      // new object
      ostore_.insert_proxy(x.proxy_, x.get());
    }
  }
}

}

#endif /* OBJECT_STORE_HPP */
