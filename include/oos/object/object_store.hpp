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

#include "oos/object/prototype_iterator.hpp"
#include "oos/object/object_exception.hpp"
#include "oos/object/object_store_observer.hpp"
#include "oos/object/object_inserter.hpp"
#include "oos/object/object_deleter.hpp"
#include "oos/object/node_analyzer.hpp"
#include "oos/object/has_one.hpp"
#include "oos/object/belongs_to.hpp"
#include "oos/object/object_serializer.hpp"
#include "oos/object/basic_has_many.hpp"
#include "oos/object/transaction.hpp"

#include "oos/utils/sequencer.hpp"
#include "oos/utils/identifier_setter.hpp"

#include <memory>
#include <unordered_map>
#include <algorithm>
#include <stack>

#include <string>
#include <ostream>
#include <list>
#include <iostream>

#ifdef _MSC_VER
  #ifdef oos_object_EXPORTS
    #define OOS_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define OOS_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
  #define OOS_OBJECT_API
#endif

namespace oos {

class object_proxy;

class prototype_node;

namespace detail {

class OOS_OBJECT_API modified_marker
{
public:
  typedef void (*t_marker)(object_store &store, object_proxy &proxy);

public:
  template < class T >
  modified_marker(T*)
    : marker_(&marker_func<T>)
  {}

  void mark(object_store &store, object_proxy &proxy)
  {
    marker_(store, proxy);
  }

private:
  template < class T >
  static void marker_func(object_store &store, object_proxy &proxy);

private:
  t_marker marker_;
};

}

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
class OOS_OBJECT_API object_store
{
private:
  object_store(const object_store&) = delete;
  object_store& operator=(const object_store&) = delete;

public:
  typedef prototype_iterator iterator;              /**< Shortcut for the list iterator. */
  typedef const_prototype_iterator const_iterator;  /**< Shortcut for the list const iterator. */

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
   * Inserts a new object prototype into the prototype tree. The prototype
   * constist of a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given.
   * 
   * @tparam T       The type of the prototype node
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @param parent   The name of the parent type.
   * @return         Returns new inserted prototype iterator.
   */
//  template< class T, template < class V = T > class ... O >
//  prototype_iterator attach(const char *type, bool abstract = false, const char *parent = nullptr);

  template <class T, template < class V = T > class ... O >
  prototype_iterator attach(const char *type, O<T>*... observer);

  template <class T, template < class V = T > class ... O >
  prototype_iterator attach(const char *type, bool abstract = false, const char *parent = nullptr, O<T>*... observer);

  template <class T >
  object_store::iterator object_store::attach(const char *type, bool abstract, const char *parent, const std::initializer_list<object_store_observer<T>*> &observer);

  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * consists of a unique type name. To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given.
   * parameter.
   * 
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @return         Returns new inserted prototype iterator.
   */
//  template<class T, class S, template < class V = T > class ... O >
//  prototype_iterator attach(const char *type, bool abstract = false);

  template<class T, class S, template < class V = T > class ... O >
  prototype_iterator attach(const char *type, O<T>*... observer);

  template<class T, class S, template < class V = T > class ... O >
  prototype_iterator attach(const char *type, bool abstract = false, O<T>*... observer);

  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * consists of a unique type name (generated from typeid). To know where the new
   * prototype is inserted into the hierarchy the type name of the parent
   * node is also given.
   *
   * @tparam T       The type of the prototype node
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @param parent   The name of the parent type.
   * @return         Returns new inserted prototype iterator.
   */
  template<class T>
  prototype_iterator prepare_attach(bool abstract = false, const char *parent = nullptr);

  /**
   * Inserts a new object prototype into the prototype tree. The prototype
   * consists of a unique type name (generated from typeid). To know where the new
   * prototype is inserted into the hierarchy the typeid of the parent
   * node is also given.
   *
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @return         Returns new inserted prototype iterator.
   */
  template<class T, class S>
  prototype_iterator prepare_attach(bool abstract = false);

  /**
   * Removes an object prototype from the prototype tree. All children
   * nodes and all objects are also removed.
   * 
   * @param type The name of the type to remove.
   */
  void detach(const char *type);

  /**
   * Erase a prototype node identified
   * by its iterator. The successor iterator
   * is returned.
   *
   * @param i The prototype iterator to be erased
   * @return The successor of the erased iterator
   */
  iterator detach(const prototype_iterator &i);

  /**
   * Finds the typename to the given class.
   * If no typename is found an empty string
   * is returned.
   *
   * @tparam class type to find
   * @return The corresponding typename
   */
  template < class T >
  std::string type() const {
    const_iterator i = find<T>();
    if (i == end()) {
      return "";
    }
    return i->type();
  }

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
  iterator find(const char *type);

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
  const_iterator find(const char *type) const;

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
  template<class T>
  iterator find()
  {
    return find(typeid(T).name());
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
  template<class T>
  const_iterator find() const
  {
    return find(typeid(T).name());
  }

  /**
   * Return the first prototype node.
   *
   * @return The first prototype node iterator.
   */
  const_iterator begin() const;

  /**
   * Return the first prototype node.
   *
   * @return The first prototype node iterator.
   */
  iterator begin();

  /**
   * Return the last prototype node.
   *
   * @return The last prototype node iterator.
   */
  const_iterator end() const;

  /**
   * Return the last prototype node.
   *
   * @return The last prototype node iterator.
   */
  iterator end();

  /**
   * Removes all inserted prototypes and all inserted objects.
   */
  void clear(bool full = false);

  /**
 * Clears a prototype node and its cildren nodes.
 * All objects will be deleted.
 *
 * @param type The name of the type to remove.
 * @throws oos::object_exception on error
 */
  void clear(const char *type);

  /**
   * Clears a prototype node by an iterator and its
   * cildren nodes. All object_proxy objects will be
   * deleted.
   *
   * @param node The prototype_iterator to remove.
   * @throws oos::object_exception on error
   */
  void clear(const prototype_iterator &node);

  /**
   * Return count of prototype nodes
   *
   * @return Count of prototype nodes
   */
  size_t size() const;

  /**
   * Returns true if the object_store
   * conatins no elements (objects)
   * 
   * @return True on empty object_store.
   */
  bool empty() const;

  size_t depth(const prototype_node *node) const;

  void dump(std::ostream &out) const;

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
  template<class T>
  T *create() const {
    const_prototype_iterator node = find<T>();
    if (node != end()) {
      return new T;
    } else {
      throw object_exception("unknown prototype type");
    }
  }

  /**
   * Executes a predicate for each root node
   *
   * @tparam P The type of the predicate function object
   * @param pred The function object to be executed
   */
  template < typename P >
  void for_each_root_node(P pred) const
  {
    prototype_node *node = first_->next;
    while (node != last_) {
      pred(const_prototype_iterator(node));
      node = node->next;
    }
  }

  /**
   * @brief Inserts a new proxy into the object store
   *
   * @param oproxy Object proxy to insert
   * @param notify Indicates wether all observers should be notified.
   * @param is_new Proxy is a new not inserted proxy, skip object store check
   */
  template < class T >
  object_proxy* insert(object_proxy *proxy, bool notify)
  {
    if (proxy == nullptr) {
      throw object_exception("proxy is null");
    }
    if (proxy->obj() == nullptr) {
      throw object_exception("object is null");
    }
    iterator node = find(proxy->classname());
    if (node == end()) {
      throw object_exception("couldn't find object type");
    }
    // check if proxy/object is already inserted
    if (proxy->ostore() != nullptr && proxy->id() > 0) {
      return proxy;
    }
    // check if proxy/object is already inserted
    if (proxy->ostore() == nullptr && proxy->id() > 0) {
      throw object_exception("object has id but doesn't belong to a store");
    }

    proxy->id(seq_.next());
    proxy->ostore_ = this;

    // get object
    T *object = static_cast<T*>(proxy->obj());
    if (object && proxy->has_identifier() && !proxy->pk()->is_valid()) {
      // if object has primary key of type short, int or long
      // set the id of proxy as value
      identifier_setter<unsigned long>::assign(proxy->id(), object);
    }

    node->insert(proxy);

    // initialize object
    if (object) {
      object_inserter_.insert(proxy, object, notify);
    }
    // set this into persistent serializable
    // notify observer
    if (notify && !transactions_.empty()) {
      transactions_.top().on_insert<T>(proxy);
    }

    // insert element into hash map for fast lookup
    object_map_.insert(std::make_pair(proxy->id(), proxy));

    return proxy;
  }

  /**
   * Inserts an object of a specfic type. On successfull insertion
   * an object_ptr element with the inserted object is returned.
   *
   * @param o Object to be inserted.
   * @return Inserted object contained by an object_ptr on success.
   */
  template < class T >
  object_ptr<T> insert(T *o)
  {
    if (o == nullptr) {
      throw object_exception("object is null");
    }
    object_inserter_.reset();
    std::unique_ptr<object_proxy> proxy(new object_proxy(o));
    try {
      insert<T>(proxy.get(), true);
    } catch (object_exception &ex) {
      proxy->release<T>();
      throw ex;
    }

    return object_ptr<T>(proxy.release());
  }

  /**
   * Inserts a given object_ptr of specific type.
   * On successfull insertion an object_ptr element
   * with the inserted object is returned.
   *
   * @param optr object_ptr to be inserted.
   * @return Inserted object contained by an object_ptr on success.
   */
  template < class T >
  object_ptr<T> insert(const object_ptr<T> &o)
  {
    return insert(o, true);
  }

  /**
   * Returns true if the underlaying
   * serializable is removable.
   * 
   * @param o The serializable to check.
   * @return True if serializable is removable.
   */
  template<class T>
  bool is_removable(const object_ptr<T> &o) {
    return object_deleter_.is_deletable(o.proxy_, o.get());
  }

  void remove_proxy(object_proxy *proxy);

  template < class T >
  void remove(object_proxy *proxy, bool notify, bool check_if_deletable)
  {
    if (proxy == nullptr) {
      throw object_exception("object proxy is nullptr");
    }
    if (proxy->node() == nullptr) {
      throw object_exception("prototype node is nullptr");
    }
    // check if object tree is deletable
    if (check_if_deletable && !object_deleter_.is_deletable<T>(proxy, (T*)proxy->obj())) {
      throw object_exception("object is not removable");
    }

    if (check_if_deletable) {
      detail::object_deleter::iterator first = object_deleter_.begin();
      detail::object_deleter::iterator last = object_deleter_.end();

      while (first != last) {
        if (!first->second.ignore) {
          (first++)->second.remove(notify);
        } else {
          ++first;
        }
      }
    } else {
      // single deletion
      if (object_map_.erase(proxy->id()) != 1) {
        // couldn't remove object
        // throw exception
        throw object_exception("couldn't remove object");
      }

      proxy->node()->remove(proxy);

      if (notify && !transactions_.empty()) {
        // notify transaction
        transactions_.top().on_delete<T>(proxy);
      } else {
        delete proxy;
      }
    }
  }
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
  template<class T>
  void remove(object_ptr<T> &o)
  {
    remove<T>(o.proxy_, true, true);
  }

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
  template<class T>
  object_proxy *create_proxy(T *o)
  {
    std::unique_ptr<object_proxy> proxy(new object_proxy(o, seq_.next(), this));
    return object_map_.insert(std::make_pair(seq_.current(), proxy.release())).first->second;
  }

  template<class T>
  object_proxy *create_proxy(T *o, unsigned long oid)
  {
    std::unique_ptr<object_proxy> proxy(new object_proxy(o, oid, this));
    return object_map_.insert(std::make_pair(oid, proxy.release())).first->second;
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
   * Insert object proxy into object store
   * Object id must be set
   */
  object_proxy* insert_proxy(object_proxy *proxy);

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
  object_proxy *register_proxy(object_proxy *oproxy);

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

  void notify_relation_insert(prototype_node::relation_info &info, void *owner, object_proxy *value);
  void notify_relation_insert(prototype_node &node, void *owner, object_proxy *value);
  void notify_relation_remove(prototype_node::relation_info &info, void *owner, object_proxy *value);
  void notify_relation_remove(prototype_node &node, void *owner, object_proxy *value);

  transaction current_transaction();
  bool has_transaction() const;

private:
  friend class detail::modified_marker;
  friend class detail::object_inserter;
  friend class object_deleter;
  friend class object_serializer;
  friend class restore_visitor;
  friend class object_container;
  friend class object_holder;
  friend class object_proxy;
  friend class prototype_node;
  template < class T, template < class ... > class ON_ATTACH >
  friend class detail::node_analyzer;
  friend class transaction;
  template < class T, template <class ...> class C >
  friend class has_many;


private:
//  template < class T, template < class V = T > class ... O, typename = typename std::enable_if< std::is_same<T, has_many_item<typename T::value_type>>::value >::type >
//  prototype_iterator attach(const char *id, abstract_has_many *container)
//  {
//    temp_container_ = container;
//    prototype_iterator i = attach<T, O...>(id);
//    temp_container_ = nullptr;
//    return i;
//  }

  /**
   * Clears a prototype_node and its
   * cildren nodes. All object_proxy objects will be
   * deleted.
   *
   * @param node The prototype_node to remove.
   * @return The next valid prototype node.
   * @throws oos::object_exception on error
   */
  prototype_node* clear(prototype_node *node);

  template < class T >
  void mark_modified(object_proxy *proxy)
  {
    if (!transactions_.empty()) {
      transactions_.top().on_update<T>(proxy);
    }
  }

  template < class T >
  object_ptr<T> insert(const object_ptr<T> &o, bool reset)
  {
    if (reset) {
      object_inserter_.reset();
    }
    insert<T>(o.proxy_, true);
    return o;
  }

  /**
   * @internal
   *
   * Searches a prototype by type
   * which can either be a type name
   * or a class name.
   * Returns a valid prototype node or
   * nullptr on unknown type
   * or throws an exception on error
   *
   * @param type Type name of the prototype node to search
   * @return The requested prototype node or nullptr
   * @throws oos::object_exception if in error occurrs
   */
  prototype_node *find_prototype_node(const char *type) const;

  /**
   * @internal
   *
   * Removes a prototy node and
   * return its successor node
   *
   * @param node The prototype node to remove
   * @param is_root Indicates if given node is root node
   * @return The successor node
   * @throws oos::object_exception if in error occurrs
   */
  prototype_node *remove_prototype_node(prototype_node *node, bool is_root);

  /**
   * Get or create a prototype node
   *
   * @param producer The producer of the concrete object
   * @param type The type name of the node
   * @param abstract indicates wether the representing object is abstract
   * @tparam T Type of the node
   * @return The prototype node
   */
  template<class T>
  prototype_node *acquire(const char *type, bool abstract);

  prototype_node* find_parent(const char *name) const;

  void push_transaction(const transaction &tr);
  void pop_transaction();

  bool is_relation_notification_enabled();
  void enable_relation_notification();
  void disable_relation_notification();

  template< class T >
  void create_observer(prototype_node *) {}

  template< class T, template < class V = T > class HEAD >
  void create_observer(prototype_node *node, HEAD<T>* observer)
  {
    node->register_observer(observer);
  }

  template< class T, template < class V = T > class HEAD, template < class V = T > class ... TAIL >
  void create_observer(prototype_node *node, HEAD<T>* observer, TAIL<T>*... observer_list)
  {
    node->register_observer(observer);
    create_observer<T>(node, observer_list...);
  }

private:
  typedef std::unordered_map<std::string, prototype_node *> t_prototype_map;
  // typeid -> [name -> prototype]
  typedef std::unordered_map<std::string, t_prototype_map> t_typeid_prototype_map;

private:
  prototype_node *first_;
  prototype_node *last_;

  // name to prototype node map
  t_prototype_map prototype_map_;
  // typeid to prototype node map
  t_typeid_prototype_map typeid_prototype_map_;

  // prepared prototype nodes
  t_prototype_map prepared_prototype_map_;

  typedef std::unordered_map<long, object_proxy *> t_object_proxy_map;
  t_object_proxy_map object_map_;

  sequencer seq_;

  detail::object_deleter object_deleter_;
  detail::object_inserter object_inserter_;

  // only used when a has_many object is inserted
  abstract_has_many *temp_container_ = nullptr;

  std::stack<transaction> transactions_;

  // relation notification related
  bool relation_notification_ = true;
};

template <class T, template < class V = T > class ... O >
object_store::iterator object_store::attach(const char *type, O<T>*... observer)
{
  return attach<T>(type, false, nullptr, observer...);
}

template <class T, template < class V = T > class ... O >
object_store::iterator object_store::attach(const char *type, bool abstract, const char *parent, O<T>*... observer)
{
  return attach<T>(type, abstract, parent, { observer... });
  // set node to root node
  prototype_node *parent_node = find_parent(parent);
  /*
   * try to insert new prototype node
   */
  const char *name = typeid(T).name();
  prototype_node *node = nullptr;
  t_prototype_map::iterator i = prepared_prototype_map_.find(name);
  if (i != prepared_prototype_map_.end()) {
    // found a prepared node
    node = i->second;
    node->type_.assign(type);
    prepared_prototype_map_.erase(i);
  } else {
    node = acquire<T>(type, abstract);
    // insert node
    if (parent_node != nullptr) {
      parent_node->insert(node);
    } else {
      last_->prev->append(node);
    }
    // Analyze primary and foreign keys of node
    basic_identifier *id = identifier_resolver<T>::resolve();
    if (id) {
      id->isolate();
      node->id_.reset(id);
    }
  }

  create_observer<T>(node, observer...);

  // store prototype in map
  // Todo: check return value
  prototype_map_.insert(std::make_pair(node->type_, node))/*.first*/;
  typeid_prototype_map_[typeid(T).name()].insert(std::make_pair(node->type_, node));

  // Check if nodes serializable has 'to-many' relations
  // Analyze primary and foreign keys of node
  detail::node_analyzer<T, O...> analyzer(*node, observer...);
  analyzer.analyze();

  node->on_attach();

  return prototype_iterator(node);
}

template <class T >
object_store::iterator object_store::attach(const char *type, bool abstract, const char *parent, const std::initializer_list<object_store_observer<T>*> &observer)
{
  return iterator();
};

//template<class T, class S, template < class V = T > class ... O >
//object_store::iterator object_store::attach(const char *type, bool abstract, O<T>*... observer)
//{
//  return iterator();
//}

template<class T, class S, template < class V = T > class ... O >
object_store::iterator object_store::attach(const char *type, O<T>*... observer)
{
  return attach<T, S>(type, false, observer...);
}

template<class T, class S, template < class V = T > class ... O >
object_store::iterator object_store::attach(const char *type, bool abstract, O<T>*... observer)
{
  return attach<T>(type, abstract, typeid(S).name(), observer...);
}

template<class T>
prototype_iterator object_store::prepare_attach(bool abstract, const char *parent)
{
  prototype_node *parent_node = find_parent(parent);

  if (typeid_prototype_map_.find(typeid(T).name()) != typeid_prototype_map_.end()) {
    throw_object_exception("attach: object type " << typeid(T).name() << " already in attached");
  }

  t_prototype_map::iterator i = prepared_prototype_map_.find(typeid(T).name());
  if (i != prepared_prototype_map_.end()) {
    // there is already a prepared node for this type
    return prototype_iterator(i->second);
  }

  std::unique_ptr<prototype_node> node(new prototype_node(this, "", new T, typeid(T), abstract));

  node->initialize(this, "", abstract);

  if (parent_node != nullptr) {
    parent_node->insert(node.get());
  } else {
    last_->prev->append(node.get());
  }

  // store only in prepared prototype map
  // Todo: check return value
  prepared_prototype_map_.insert(std::make_pair(typeid(T).name(), node.get()));
  // Analyze primary and foreign keys of node
  std::unique_ptr<basic_identifier> id(identifier_resolver<T>::resolve());
  if (id) {
    id->isolate();
    node->id_.reset(id.release());
  }

  return prototype_iterator(node.release());
}

template<class T, class S>
prototype_iterator object_store::prepare_attach(bool abstract)
{
  return prepare_attach<T>(abstract, typeid(T).name());
}

template<class T>
prototype_node *object_store::acquire(const char *type, bool abstract)
{
  // try to find node in prepared map
  const char *name = typeid(T).name();
  prototype_node *node = nullptr;
  t_prototype_map::iterator i = prototype_map_.find(type);
  if (i != prototype_map_.end()) {
    throw_object_exception("prototype already inserted: " << type);
  }
  // try to find by typeid name
  i = prototype_map_.find(name);
  if (i != prototype_map_.end()) {
    throw_object_exception("prototype already inserted: " << type);
  }
  /*
   * no typeid found, seems to be
   * a new type
   * to be sure check in typeid map
   */
  t_typeid_prototype_map::iterator j = typeid_prototype_map_.find(name);
  if (j != typeid_prototype_map_.end() && j->second.find(type) != j->second.end()) {
    /* unexpected found the
     * typeid check for type
     * throw exception
     */
    throw object_exception("unexpectly found prototype");
  } else {
    /* insert new prototype and add to
     * typeid map
     */
    node = new prototype_node(this, type, new T, typeid(T), abstract);
  }
  return node;
}

namespace detail {

template < class T >
void modified_marker::marker_func(object_store &store, object_proxy &proxy)
{
  store.mark_modified<T>(&proxy);
}

}

}

#include "oos/object/node_analyzer.tpp"
#include "oos/object/object_inserter.tpp"
#include "oos/object/object_deleter.tpp"

#endif /* OBJECT_STORE_HPP */
