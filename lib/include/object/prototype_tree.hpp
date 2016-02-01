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

#ifndef PROTOTYPE_TREE_HPP
#define PROTOTYPE_TREE_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#define EXPIMP_TEMPLATE
#endif

#include "object/prototype_node.hpp"
#include "object/prototype_iterator.hpp"
#include "object/object_exception.hpp"
#include "tools/identifier_resolver.hpp"
#include "object/has_one.hpp"
#include "object/has_many.hpp"

#include <string>
#include <unordered_map>

namespace oos {

class object_container;

namespace detail {

class foreign_key_analyzer
{
public:
  explicit foreign_key_analyzer(prototype_node &node) : node_(node){}
  ~foreign_key_analyzer() {}

  template < class T >
  static void analyze(prototype_node &node)
  {
    foreign_key_analyzer analyzer(node);
    T obj;
    oos::access::serialize(analyzer, obj);
  }

  template < class V >
  void serialize(V &x)
  {
    oos::access::serialize(*this, x);
  }

  template < class V >
  void serialize(const char*, V&) {}
  void serialize(const char*, char*, size_t) {}
  template < class T >
  void serialize(const char *id, has_one<T> &x, cascade_type);
  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

private:
  prototype_node &node_;
};

class relation_resolver
{
public:
  typedef std::list<std::string> string_list_t;
  typedef string_list_t::const_iterator const_iterator;

public:
  relation_resolver(prototype_node &node)
    : node_(node)
  {}
  virtual ~relation_resolver() {}

  template < class T >
  static void build(prototype_node &node)
  {
    relation_resolver resolver(node);
    resolver.build<T>();
  }

  template < class T >
  void build()
  {
    T obj;
    oos::access::serialize(*this, obj);
  }

  template < class V >
  void serialize(V &x)
  {
    oos::access::serialize(*this, x);
  }

  template < class V >
  void serialize(const char*, V&) {}
  void serialize(const char*, char*, int) {}
  template < class T >
  void serialize(const char *, has_one<T> &x, cascade_type cascade);
  template < class T, template <class ...> class C >
  void serialize(const char *, has_many<T, C> &, const char *, const char *);

private:
  prototype_node &node_;
};

}

/**
 * @brief This class holds all prototypes nodes in a tree
 *
 * All prototype_node objects are stored within this tree. The user
 * decides the hierarchy when inserting a prototype_node
 *
 * The following example inserts a new prototype_node 'student' under
 * the base class 'person' (which must be already inserted).
 *
 * @code{.cpp}
 * tree.insert<student, person>("student");
 * @endcode
 *
 * One can find a prototype_node by its name or type
 *
 * @code{.cpp}
 * tree.find("student");
 * tree.find<student>();
 * @endcode
 *
 * One can also erase a prototype_node by its name or type
 *
 * @code{.cpp}
 * tree.remove("student");const
 * tree.erase<student>();
 * @endcode
 *
 */
class OOS_API prototype_tree {
public:
  typedef prototype_iterator iterator;               /**< Shortcut for the list iterator. */
  typedef const_prototype_iterator const_iterator;   /**< Shortcut for the list const iterator. */

public:
  prototype_tree();
  ~prototype_tree();

  /**
  * Inserts a new serializable prototype into the prototype tree. The prototype
  * consists of a producer and a unique type name. To know where the new
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
  iterator attach(const char *type, bool abstract = false, const char *parent = nullptr)
  {
    // set node to root node
    prototype_node *parent_node = nullptr;
    if (parent != nullptr) {
      parent_node = find_prototype_node(parent);
      if (!parent_node) {
        throw object_exception("unknown prototype type");
      }
    }
    /*
     * try to insert new prototype node
     */
    prototype_node *node = acquire<T>(type, abstract);

    if (parent != nullptr) {
      parent_node->insert(node);
    } else {
      last_->prev->append(node);
    }

    return initialize<T>(node);
  }

  /**
  * Inserts a new serializable prototype into the prototype tree. The prototype
  * consists of a producer and a unique type name. To know where the new
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
  iterator attach(const char *type, bool abstract = false)
  {
    return attach<T>(type, abstract, typeid(S).name());
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
  * Finds and returns prototype node const iterator identified
  * by the given name or classname (typeid). If the
  * prototype couldn't be found const_prototype_iterator end
  * is returned.
  *
  * @param type Name or class name of the prototype
  * @return Returns a prototype const iterator.
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
  template < class T >
  iterator find()
  {
    return find(typeid(T).name());
  }

  /**
  * @brief Finds prototype node by template type.
  * @tparam Template type.
  *
  * Finds and returns prototype node const iterator identified
  * by the given template typeid. If the prototype couldn't
  * be found const_prototype_iterator end is returned.
  *
  * @return Returns a prototype const iterator.
  */
  template < class T >
  const_iterator find() const
  {
    return find(typeid(T).name());
  }

  /**
   * Returns true if prototype tree
   * is empty
   *
   * @return True on empty prototype tree
   */
  bool empty() const;

  /**
   * Return count of prototype nodes
   *
   * @return Count of prototype nodes
   */
  size_t size() const;

  /**
   * Return the number of prototypes
   * in the tree
   *
   * @return The number of prototypes
   */
  size_t prototype_count() const;

  /**
   * Clears the whole tree. All nodes and object_proxy objects
   * are deleted.
   */
  void clear();

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
   * Clears a prototype_node and its
   * cildren nodes. All object_proxy objects will be
   * deleted.
   *
   * @param node The prototype_node to remove.
   * @return The next valid prototype node.
   * @throws oos::object_exception on error
   */
  prototype_node* clear(prototype_node *node);

  /**
   * Determines the depth of a prototype_node.
   *
   * @param node The prototype_node requesting its depth
   * @return The depth of the given node
   */
  int depth(const prototype_node *node) const;

  /**
  * Dump all prototypes to a given stream
  *
  * @param out The stream to the prototypes dump on.
  */
//  void dump(std::ostream &out) const;

  /**
   * Erase a prototype node identified
   * by its iterator. The successor iterator
   * is returned.
   *
   * @param i The prototype iterator to be erased
   * @return The successor of the erased iterator
   */
  iterator erase(const prototype_iterator &i);

  /**
  * Removes an serializable prototype from the prototype tree. All children
  * nodes and all objects are also removed.
  *
  * @param type The name of the type to remove.
  * @return Returns true if the type was found and successfully removed
  * @throws oos::object_exception on error
  */
  void remove(const char *type);

  /**
  * Removes an serializable prototype from the prototype tree. All children
  * nodes and all objects are also removed.
  *
  * @param node The prototype iterator node to remove.
  * @return Returns true if the type was found and successfully removed
  * @throws oos::object_exception on error
  */
  void remove(const prototype_iterator &node);

  /**
  * Return the first prototype node.
  *
  * @return The first prototype node iterator.
  */
  iterator begin();

  /**
  * Return the first prototype node.
  *
  * @return The first prototype node iterator.
  */
  const_iterator begin() const;

  /**
  * Return the last prototype node.
  *
  * @return The last prototype node iterator.
  */
  iterator end();

  /**
   * Return the last prototype node.
   *
   * @return The last prototype node iterator.
   */
  const_iterator end() const;

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
private:
  typedef std::unordered_map<std::string, prototype_node*> t_prototype_map;
  // typeid -> [name -> prototype]
  typedef std::unordered_map<std::string, t_prototype_map> t_typeid_prototype_map;

private:
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
  prototype_node* find_prototype_node(const char *type) const;

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
  prototype_node* remove_prototype_node(prototype_node *node, bool is_root);

  /**
  * @internal
  *
  * Adjust first marker of all successor nodes with given serializable proxy.
  *
  * @param old_proxy The old first marker proxy.
  * @param new_proxy The new first marker proxy.
  */
  void adjust_right_marker(prototype_node *root, object_proxy *old_proxy, object_proxy *new_proxy);

  /**
  * @internal
  *
  * Adjusts self and last marker of all predeccessor nodes with given
  * serializable proxy.
  *
  * @param old_proxy The old last marker proxy.
  * @param new_proxy The new last marker proxy.
  */
  void adjust_left_marker(prototype_node *root, object_proxy *old_proxy, object_proxy *new_proxy);

  /**
   * Get or create a prototype node
   *
   * @param producer The producer of the concrete object
   * @param type The type name of the node
   * @param abstract indicates wether the representing object is abstract
   * @return The prototype node
   */
  template < class T >
  prototype_node *acquire(const char *type, bool abstract)
  {
    prototype_node *node = nullptr;
    t_prototype_map::iterator i = prototype_map_.find(type);
    if (i != prototype_map_.end()) {
      throw_object_exception("prototype already inserted: " << type);
    }

    /* unknown type name try for typeid
     * (unfinished prototype)
     */
    const char *name = typeid(T).name();
    i = prototype_map_.find(name);
    if (i == prototype_map_.end()) {
      /*
       * no typeid found, seems to be
       * a new type
       * to be sure check in typeid map
       */
      t_typeid_prototype_map::iterator j = typeid_prototype_map_.find(name);
      if (j != typeid_prototype_map_.end() && j->second.find(type) != j->second.end()) {
        /* unexpected found the
         * typeid check for type
         */
        /* type found in typeid map
         * throw exception
         */
        throw object_exception("unexpectly found prototype");
      } else {
        /* insert new prototype and add to
         * typeid map
         */
        // create new one
        node = new prototype_node(this, type, typeid(T).name(), abstract);
      }
    } else {
      /* prototype is unfinished,
       * finish it, insert by type name,
       * remove typeid entry and add to
       * typeid map
       */
      node = i->second;
      node->initialize(this, type, abstract);
      prototype_map_.erase(i);
    }

    return node;
  }

  /**
   * Initializes a prototype node
   *
   * @param node The node to initialize
   * @return iterator representing the prototype node
   */
  template < class T >
  iterator initialize(prototype_node *node)
  {
    // store prototype in map
    // Todo: check return value
    prototype_map_.insert(std::make_pair(node->type_, node))/*.first*/;
    typeid_prototype_map_[typeid(T).name()].insert(std::make_pair(node->type_, node));

    // Analyze primary and foreign keys of node
    basic_identifier *id = identifier_resolver<T>::resolve();
    if (id) {
      id->isolate();
      node->id_.reset(id);
    }

    // Check if nodes serializable has 'to-many' relations
    T obj;
    std::unique_ptr<T> o(new T);
    detail::relation_resolver::build<T>(*node);

    // Analyze primary and foreign keys of node
    detail::foreign_key_analyzer::analyze<T>(*node);

    while (!node->foreign_key_ids.empty()) {
      auto i = node->foreign_key_ids.front();
      node->foreign_key_ids.pop_front();
      prototype_node *foreign_node = i.first;
      std::shared_ptr<basic_identifier> fk(node->id_->clone());
      foreign_node->foreign_keys.insert(std::make_pair(i.second, fk));
    }

    return prototype_iterator(node);
  }

private:
  friend class object_container;
  friend class object_store;
  friend class prototype_node;
  friend class relation_resolver;

  prototype_node *first_;
  prototype_node *last_;

  // name to prototype node map
  t_prototype_map prototype_map_;
  // typeid to prototype node map
  t_typeid_prototype_map typeid_prototype_map_;
};

namespace detail {

template < class T >
void foreign_key_analyzer::serialize(const char *id, has_one<T> &x, cascade_type)
{
  prototype_iterator node = node_.tree()->find(x.type());
  if (node == node_.tree()->end()) {
    throw_object_exception("couldn't find prototype node of type '" << x.type() << "'");
  } else if (!node->has_primary_key()) {
    throw_object_exception("object of type '" << x.type() << "' has no primary key");
  } else {
    // node is inserted/attached; store it in nodes foreign key map
    std::shared_ptr<basic_identifier> fk(node->id()->clone());
    node_.register_foreign_key(id, fk);
  }
}

template<class T >
void relation_resolver::serialize(const char *, has_one<T> &x, cascade_type) {
  prototype_iterator pi = node_.tree()->find(x.type());
  if (pi == node_.tree()->end()) {
    // if there is no such prototype node
    // insert a new one
    node_.tree()->attach<T>(x.type());
  }
}

template < class T, template <class ...> class C >
void relation_resolver::serialize(const char *id, has_many<T, C> &, const char *, const char *)
{
  // attach releation table for has many relation
  node_.tree()->attach<typename has_many<T, C>::value_type>(id);
}

}

}

#endif /* PROTOTYPE_TREE_HPP */
