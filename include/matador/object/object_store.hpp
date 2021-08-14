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

#include "matador/object/prototype_iterator.hpp"
#include "matador/object/object_exception.hpp"
#include "matador/object/typed_object_store_observer.hpp"
#include "matador/object/object_inserter.hpp"
#include "matador/object/object_deleter.hpp"
#include "matador/object/node_analyzer.hpp"
#include "matador/object/has_one.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/object_serializer.hpp"
#include "matador/object/basic_has_many.hpp"
#include "matador/object/transaction.hpp"

#include "matador/utils/sequencer.hpp"
#include "matador/utils/sequence_synchronizer.hpp"
#include "matador/utils/identifier_setter.hpp"

#include <memory>
#include <unordered_map>
#include <algorithm>
#include <stack>

#include <string>
#include <ostream>
#include <list>
#include <iostream>

#ifdef _MSC_VER
  #ifdef matador_object_EXPORTS
    #define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define MATADOR_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
  #define MATADOR_OBJECT_API
#endif

namespace matador {

class object_proxy;

class prototype_node;

namespace detail {

/// @cond MATADOR_DEV

class MATADOR_OBJECT_API modified_marker
{
public:
  typedef void (*t_marker)(object_store &store, object_proxy &proxy);

public:
  template < class T >
  explicit modified_marker(T*)
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

/// @endcond

}


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
class MATADOR_OBJECT_API object_store
{
public:
  object_store(const object_store&) = delete;
  object_store& operator=(const object_store&) = delete;

public:
  typedef prototype_iterator iterator;              /**< Shortcut for the list iterator. */
  typedef const_prototype_iterator const_iterator;  /**< Shortcut for the list const iterator. */

  /**
   * Describes wether the inserted object type
   * is handle as a concrete or abstract type
   */
  enum abstract_type {
    abstract,           /**< Indicates an abstract object type */
    not_abstract        /**< Indicates a concrete object type */
  };

/// @cond MATADOR_DEV
  template < class T >
  struct null_observer : public typed_object_store_observer<T>
  {
    template < class V >
    explicit null_observer(const null_observer<V> *) {}
    void on_attach(prototype_node &, T &) override {}
    void on_detach(prototype_node &, T &) override {}
    void on_insert(object_proxy &) override {}
    void on_update(object_proxy &) override {}
    void on_delete(object_proxy &) override {}
  };
/// @endcond

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
   * Inserts a new concrete object prototype into the object_store. The prototype
   * consists of a unique type name. The type of the prototype is given via
   * template parameter T.
   * If parent name is given prototype node is inserted below the found parent
   * node.
   *
   * @tparam T Type of the prototype
   * @param type Name of the prototype
   * @param parent Name of the parent node
   * @return Iterator representing the inserted prototype node
   */
  template <class T >
  prototype_iterator attach(const char *type, const char *parent = nullptr)
  {
    return attach<T>(type, abstract_type::not_abstract, parent);
  }

  /**
   * Inserts a new abstract object prototype into the object_store. The prototype
   * consists of a unique type name. The type of the prototype is given via
   * template parameter T.
   * If parent name is given prototype node is inserted below the found parent
   * node.
   *
   * @tparam T Type of the prototype
   * @param type Name of the prototype
   * @param parent Name of the parent node
   * @return Iterator representing the inserted prototype node
   */
  template <class T >
  prototype_iterator attach_abstract(const char *type, const char *parent = nullptr)
  {
    return attach<T>(type, abstract_type::abstract, parent);
  }

  /**
   * Inserts a new concrete object prototype into the prototype tree. The prototype
   * consists of a unique type name.
   *
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @param type     The unique name of the type.
   * @return         Returns new inserted prototype iterator.
   */
  template<class T, class S >
  prototype_iterator attach(const char *type)
  {
    return attach<T>(type, abstract, typeid(S).name());
  }

  /**
   * Inserts a new abstract object prototype into the prototype tree. The prototype
   * consists of a unique type name.
   *
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @param type     The unique name of the type.
   * @return         Returns new inserted prototype iterator.
   */
  template<class T, class S >
  prototype_iterator attach_abstract(const char *type)
  {
    return attach<T>(type, abstract, typeid(S).name());
  }

  /**
   *
   * If the abstract flag is true prototype is treated as abstract. No concrete
   * object can be inserted for this prototype.
   *
   * @tparam T       The type of the prototype node
   * @param type     The unique name of the type.
   * @param abstract Indicates if the producers serializable is treated as an abstract node.
   * @param parent   Name of the parent node
   * @return         Returns new inserted prototype iterator.
   */
  template < class T >
  prototype_iterator attach(const char *type, abstract_type abstract, const char *parent)
  {
    auto node = new prototype_node(this, type, new T, abstract == abstract_type::abstract);
    return attach_internal<T, null_observer>(node, parent, {});
  }

  /**
   * Inserts a new concrete object prototype into the prototype tree. The prototype
   * consists of a unique type name.
   *
   * @tparam T       The type of the prototype node
   * @tparam O       The type of the observer classes
   * @param type     The unique name of the type.
   * @param observer A list of observer to be called an attach
   * @return         Returns new inserted prototype iterator.
   */
  template<class T, template < class U = T > class O >
  prototype_iterator attach(const char *type, std::initializer_list<O<T>*> observer)
  {
    return attach<T, O>(type, abstract_type::not_abstract, nullptr, observer);
  }

  /**
   * Inserts a new concrete object prototype into the prototype tree. The prototype
   * consists of a unique type name.
   *
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @tparam O       The type of the observer classes
   * @param type     The unique name of the type.
   * @param observer A list of observer to be called an attach
   * @return         Returns new inserted prototype iterator.
   */
  template<class T, class S, template < class U = T > class O >
  prototype_iterator attach(const char *type, std::initializer_list<O<T>*> observer)
  {
    return attach<T>(type, abstract_type::not_abstract, typeid(S).name(), observer);
  }

  /**
   * Inserts a new abstract object prototype into the prototype tree. The prototype
   * consists of a unique type name.
   *
   * @tparam T       The type of the prototype node
   * @tparam S       The type of the parent prototype node
   * @tparam O       The type of the observer classes
   * @param type     The unique name of the type.
   * @param observer A list of observer to be called an attach
   * @return         Returns new inserted prototype iterator.
   */
  template<class T, class S, template < class U = T > class O >
  prototype_iterator attach_abstract(const char *type, std::initializer_list<O<T>*> observer)
  {
    return attach<T>(type, abstract_type::abstract, typeid(S).name(), observer);
  }

  /**
   * Inserts a new abstract object prototype into the prototype tree. The prototype
   * consists of a unique type name.
   *
   * @tparam T       The type of the prototype node
   * @tparam O       The type of the observer classes
   * @param type     The unique name of the type.
   * @param abstract Indicates weather type is abstract or not
   * @param parent   The name of the parent node.
   * @param observer A list of observer to be called an attach
   * @return         Returns new inserted prototype iterator.
   */
  template < class T, template < class U = T > class O >
  prototype_iterator attach(const char *type, abstract_type abstract, const char *parent, std::initializer_list<O<T>*> observer)
  {
    auto node = new prototype_node(this, type, new T, abstract == abstract_type::abstract);
    return attach_internal<T>(node, parent, std::vector<O<T>*>(observer));
  }

  /**
   * Inserts a new object prototype into the object_store. The
   * type of the prototype is given via template parameter T. A prepared
   * prototype node is passed to be inserted.
   * If parent name is given prototype node is inserted below the found parent
   * node.
   *
   * @tparam T       The type of the prototype node
   * @tparam O       The type of the observer classes
   * @param node     The prototype node to be inserted
   * @param parent   The name of the parent node.
   * @param observer A list of observer to be called an attach
   * @return         Returns new inserted prototype iterator.
   */
  template < class T, template < class U = T > class O >
  prototype_iterator attach(prototype_node *node, const char *parent, std::initializer_list<O<T>*> observer);

  /**
   * Inserts a new object prototype into the object_store as a root object type.
   * The type of the prototype is given via template parameter T. A prepared
   * prototype node is passed to be inserted.
   *
   * @tparam T       The type of the prototype node
   * @tparam O       The type of the observer classes
   * @param node     The prototype node to be inserted
   * @param observer A list of observer to be called an attach
   * @return         Returns new inserted prototype iterator.
   */
  template < class T, template < class U = T > class O >
  prototype_iterator attach(prototype_node *node, std::initializer_list<O<T>*> observer)
  {
    return attach<T>(node, nullptr, observer);
  }

  /**
   * Inserts a new object prototype into the object_store. The
   * type of the prototype is given via template parameter T. A prepared
   * prototype node is passed to be inserted.
   * If parent name is given prototype node is inserted below the found parent
   * node.
   * 
   * @tparam T       The type of the prototype node
   * @param node     The prototype node to be inserted
   * @param parent   The name of the parent node.
   * @return         Returns new inserted prototype iterator.
   */
  template < class T >
  prototype_iterator attach(prototype_node *node, const char *parent = nullptr)
  {
    return attach<T, null_observer>(node, parent, {});
  }

  /**
   * Inserts a new object prototype into the object_store as a root object type.
   * The type of the prototype is given via template parameter T. A prepared
   * prototype node is passed to be inserted.
   *
   * @tparam T       The type of the prototype node
   * @tparam O       The type of the observer classes
   * @param node     The prototype node to be inserted
   * @param parent   The name of the parent node.
   * @param observer A list of observer to be called an attach
   * @return         Returns new inserted prototype iterator.
   */
  template < class T, template < class U = T > class O >
  prototype_iterator attach_internal(prototype_node *node, const char *parent, std::vector<O<T>*> observer);


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
   * @brief Get object by primary key
   * @tparam Template type.
   */
  template < class T >
  object_ptr<T> get(basic_identifier &pk)
  {
    auto node = find_prototype_node(typeid(T).name());
    if (node == nullptr) {
      return object_ptr<T>();
    }
    return object_ptr<T>(node->find_proxy(&pk));
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
   * @throws matador::object_exception on error
   */
  void clear(const char *type);

  /**
   * Clears a prototype node by an iterator and its
   * children nodes. All object_proxy objects will be
   * deleted.
   *
   * @param node The prototype_iterator to remove.
   * @throws matador::object_exception on error
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

  /**
   * Creates an serializable of the given type name.
   * 
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
   * @param proxy Object proxy to insert
   * @param notify Indicates wether all observers should be notified.
   */
  template < class T >
  object_proxy* insert(object_proxy *proxy, bool notify)
  {
    if (proxy == nullptr) {
      throw_object_exception("proxy is null");
    }
    if (proxy->obj() == nullptr) {
      throw_object_exception("object is null");
    }
    iterator node = find(proxy->classname());
    if (node == end()) {
      throw_object_exception("couldn't find object type");
    }
    // check if proxy/object is already inserted
    if (proxy->ostore() != nullptr && proxy->id() > 0) {
      return proxy;
    }
    // check if proxy/object is already inserted
    if (proxy->ostore() == nullptr && proxy->id() > 0) {
      throw_object_exception("object has id but doesn't belong to a store");
    }

    proxy->id(seq_.next());
    proxy->ostore_ = this;

    // get object
    T *object = static_cast<T*>(proxy->obj());
    if (object && proxy->has_identifier() && !proxy->pk()->is_valid()) {
      // if object has primary key of type short, int or long
      // set the id of proxy as value
      identifier_setter<unsigned long>::assign(proxy->id(), object);
    } else if (object && proxy->has_identifier()) {
      synchronizer_.sync(*proxy->pk());
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
   * Inserts an object of a specific type. On successful insertion
   * an object_ptr element with the inserted object is returned.
   *
   * @param o Object to be inserted.
   * @return Inserted object contained by an object_ptr on success.
   */
  template < class T >
  object_ptr<T> insert(T *o)
  {
    if (o == nullptr) {
      throw_object_exception("object is null");
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
   * @param o object_ptr to be inserted.
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

  /**
   * Remove the given object_proxy. The prototype_node
   * id extracted from the given proxy to remove the proxy
   * from its proxy list.
   *
   * @param proxy The object_proxy to be removed.
   */
  void remove_proxy(object_proxy *proxy);

  /**
   * Remove the given object_proxy. The prototype_node
   * id extracted from the given proxy to remove the proxy
   * from its proxy list.
   *
   * If the notify flag is true all observers are notified on
   * successfull deletion.
   *
   * If check_if_deletable flag is true method checks if the
   * proxy is deletable.
   *
   * @tparam T Type of the object represented by the object_proxy
   * @param proxy              The object_proxy to be removed.
   * @param check_if_deletable If true methods checks if proxy is deletable.
   */
  template < class T >
  void remove(object_proxy *proxy, bool check_if_deletable)
  {
    if (proxy == nullptr) {
      throw_object_exception("object proxy is nullptr");
    }
    if (proxy->node() == nullptr) {
      throw_object_exception("prototype node is nullptr");
    }
    // check if object tree is deletable
    if (check_if_deletable && !object_deleter_.is_deletable<T>(proxy, (T*)proxy->obj())) {
      throw_object_exception("object is not removable");
    }

    if (check_if_deletable) {
      object_deleter_.remove();
    } else {
      // single deletion
      if (object_map_.erase(proxy->id()) != 1) {
        // couldn't remove object
        // throw exception
        throw_object_exception("couldn't remove object");
      }

      proxy->node()->remove(proxy);

      if (!transactions_.empty()) {
        // notify transaction
        transactions_.top().on_delete<T>(proxy);
      } else {
        on_proxy_delete_(proxy);
      }
    }
  }

  /**
   * Removes an object from the object store. After successful
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
    remove<T>(o.proxy_, true);
  }

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
   * Return the current transaction in stack
   *
   * @return The current transaction in stack
   */
  transaction current_transaction();

  /**
   * Returns true if there is at least one transaction on stack.
   *
   * @return True if there at least one transaction.
   */
  bool has_transaction() const;

  /**
   * Marks the given object proxy as modified
   * and notifies all transactions
   * 
   * @tparam T Object type of the proxy
   * @param proxy The modified object proxy
   */
  template < class T >
  void mark_modified(object_proxy *proxy)
  {
    // notify observers
    proxy->node()->on_update_proxy(proxy);

    if (has_transaction()) {
      transactions_.top().on_update<T>(proxy);
    }
  }

  /**
   * Marks the given object_ptr as modified
   * and notifies all transactions
   * 
   * @tparam T Object type of the object_ptr
   * @param optr The modified object_ptr
   */
  template < class T >
  void mark_modified(const object_ptr<T> &optr)
  {
    mark_modified<T>(optr.proxy_);
  }

  /**
   * Registers a callback when an object proxy
   * is deleted from the object store
   *
   * @param callback To be called when an object proxy is deleted
   */
  void on_proxy_delete(std::function<void(object_proxy*)> callback);
private:
  friend class detail::object_inserter;
  friend struct detail::basic_relation_endpoint;
  friend class object_serializer;
  friend class object_holder;
  friend class object_proxy;
  friend class prototype_node;
  friend class transaction;
  template < class T, template <class ...> class C >
  friend class has_many;


private:
  /**
   * Clears a prototype_node and its
   * cildren nodes. All object_proxy objects will be
   * deleted.
   *
   * @param node The prototype_node to remove.
   * @return The next valid prototype node.
   * @throws matador::object_exception on error
   */
  prototype_node* clear(prototype_node *node);

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
   * @throws matador::object_exception if in error occurrs
   */
  prototype_node *find_prototype_node(const char *type) const;

  /**
   * @internal
   *
   * Removes a prototy node and
   * return its successor node
   *
   * @param node The prototype node to remove
   * @param check_for_eos Indicates if given node is root node
   * @return The successor node
   * @throws matador::object_exception if in error occurrs
   */
  prototype_node *remove_prototype_node(prototype_node *node, bool check_for_eos);

  template < class T >
  prototype_node* attach_node(prototype_node *node, const char *parent);

  prototype_node* find_parent(const char *name) const;

  void push_transaction(const transaction &tr);
  void pop_transaction();

  template < class T >
  void validate(prototype_node *node);

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

  typedef std::unordered_map<long, object_proxy *> t_object_proxy_map;
  t_object_proxy_map object_map_;

  sequencer seq_;
  sequence_synchronizer synchronizer_;

  detail::object_deleter object_deleter_;
  detail::object_inserter object_inserter_;

  std::stack<transaction> transactions_;

  std::function<void(object_proxy *)> on_proxy_delete_;
};

template < class T >
void object_store::validate(prototype_node *node)
{
  std::unique_ptr<prototype_node> nptr(node);
  // try to find node in prepared map
  const char *name = typeid(T).name();
  auto i = prototype_map_.find(node->type_);
  if (i != prototype_map_.end()) {
    throw_object_exception("prototype already inserted: " << node->type_.c_str());
  }
  // try to find by typeid name
  i = prototype_map_.find(name);
  if (i != prototype_map_.end()) {
    throw_object_exception("prototype already inserted: " << node->type_.c_str());
  }
  /*
   * no typeid found, seems to be
   * a new type
   * to be sure check in typeid map
   */
  auto j = typeid_prototype_map_.find(name);
  if (j != typeid_prototype_map_.end() && j->second.find(node->type_) != j->second.end()) {
    /* unexpected found the
     * typeid check for type
     * throw exception
     */
    throw object_exception("unexpectly found prototype");
  }
  nptr.release();
}

template < class T, template < class U = T > class O  >
prototype_iterator object_store::attach(prototype_node *node, const char *parent, std::initializer_list<O<T>*> observer)
{
  return attach_internal<T>(node, parent, std::vector<O<T>*>(observer));
}

template < class T, template < class U = T > class O  >
prototype_iterator object_store::attach_internal(prototype_node *node, const char *parent, std::vector<O<T>*> observer)
{

  for(auto obs : observer) {
    node->register_observer(obs);
  }
  // Check if nodes object has 'to-many' relations
  // Analyze primary and foreign keys of node
  detail::node_analyzer<T, O> analyzer(*node, *this, observer);

  T *proto = node->prototype<T>();
  analyzer.analyze(*proto);

  validate<T>(node);

  node = attach_node<T>(node, parent);

  node->on_attach();

  return prototype_iterator(node);
}

template < class T >
prototype_node* object_store::attach_node(prototype_node *node, const char *parent)
{
  std::unique_ptr<prototype_node> nptr(node);
  // set node to root node
  prototype_node *parent_node = find_parent(parent);
  /*
   * try to insert new prototype node
   */
  // insert node
  if (parent_node != nullptr) {
    parent_node->insert(node);
  } else {
    last_->prev->append(node);
  }
  // Analyze primary and foreign keys of node
  basic_identifier *id = identifier_resolver<T>::create();
  if (id) {
    node->id_.reset(id);
  }
  // store prototype in map
  // Todo: check return value
  prototype_map_.insert(std::make_pair(node->type_, node))/*.first*/;
  typeid_prototype_map_[typeid(T).name()].insert(std::make_pair(node->type_, node));

  return nptr.release();
}

namespace detail {

/// @cond MATADOR_DEV

template < class T >
void modified_marker::marker_func(object_store &store, object_proxy &proxy)
{
  store.mark_modified<T>(&proxy);
}

/// @endcond

}

}

#include "matador/object/node_analyzer.tpp"
#include "matador/object/relation_field_endpoint.tpp"
#include "matador/object/relation_endpoint_value_inserter.tpp"
#include "matador/object/relation_endpoint_value_remover.tpp"
#include "matador/object/object_inserter.tpp"
#include "matador/object/object_deleter.tpp"
#include "matador/object/object_ptr.tpp"

#endif /* OBJECT_STORE_HPP */
