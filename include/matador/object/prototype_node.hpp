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

#ifndef PROTOTYPE_NODE_HPP
#define PROTOTYPE_NODE_HPP

#ifdef _MSC_VER
  #ifdef matador_object_EXPORTS
    #define MATADOR_OBJECT_API __declspec(dllexport)
    #define EXPIMP_OBJECT_TEMPLATE
  #else
    #define MATADOR_OBJECT_API __declspec(dllimport)
    #define EXPIMP_OBJECT_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define MATADOR_OBJECT_API
  #define EXPIMP_OBJECT_TEMPLATE
#endif

#include "matador/utils/identifier.hpp"

#include "matador/object/identifier_proxy_map.hpp"
#include "matador/object/object_store_observer.hpp"

#include <map>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

namespace matador {

class object_store;
class object_proxy;

/// @cond MATADOR_DEV
namespace detail {
template < class T, template <class ...> class C, class Enabled >
class has_many_inserter;
template < class T, template <class ...> class C, class Enabled >
class has_many_deleter;
class basic_node_analyzer;
template < class T, template < class V = T > class O >
class node_analyzer;
}
/// @endcond

/**
 * @class prototype_node
 * @brief Holds the prototype of a concrete serializable.
 *
 * The prototype_node class holds the prototype of
 * a concrete serializable inside a producer serializable. Whenever
 * requested the class produces a new serializable.
 * 
 * It also holds a partial list containing all objects of
 * this and all children nodes.
 * This list is defined by three marker: the beginning
 * of the list, the end of the own objects and the end of
 * the last child objects.
 */ 
class MATADOR_OBJECT_API prototype_node
{
private:
  // copying not permitted
  prototype_node(const prototype_node&) = delete;
  prototype_node& operator=(const prototype_node&) = delete;

public:
  /// @cond MATADOR_DEV
  struct relation_info
  {
    enum relation_type {
      BELONGS_TO, HAS_ONE, HAS_MANY
    };

    typedef std::function<void(object_proxy*, const std::string&, object_proxy*)> modify_value_func;
    relation_info(const std::string &n,
                  relation_type t,
                  const modify_value_func &insert_func,
                  const modify_value_func &remove_func,
                  prototype_node *pn)
      : name(n), type(t), insert_value(insert_func), remove_value(remove_func), node(pn)
    {}
    std::string name;
    relation_type type;
    std::function<void(object_proxy*, const std::string&, object_proxy*)> insert_value;
    std::function<void(object_proxy*, const std::string&, object_proxy*)> remove_value;
    prototype_node *node;
  };

  typedef std::unordered_map<std::type_index, relation_info> relation_map;

  struct relation_node_info
  {
    std::string owner_type_;
    std::string relation_id_;
    std::string owner_id_column_;
    std::string item_id_column_;
  };

  /// @endcond

public:

  /**
   * Creates a regular prototype node.
   *
   * @tparam T Type of the node
   * @param store Corresponding object_store
   * @param type Type name
   * @param abstract Flag indicating if node is abstract.
   * @return The created node.
   */
  template < class T >
  static prototype_node* make_node(object_store *store, const char *type, bool abstract = false);

  /**
   * Creates a relation prototype node.
   *
   * @tparam T Type of the node
   * @param store Corresponding object_store
   * @param type Type name
   * @param abstract Flag indicating if node is abstract.
   * @param owner_type Type name of the owner node
   * @param relation_id Name of the relation in the prototype object
   * @param owner_column Owner column name
   * @param item_column Item (foreign) column name
   * @return The created node.
   */
  template < class T >
  static prototype_node* make_relation_node(object_store *store, const char *type, bool abstract,
                                     const char *owner_type, const char *relation_id,
                                     const char *owner_column, const char *item_column);

  prototype_node();

  /**
   * @brief Creates a new prototype_node.
   * 
   * Creates a new prototype_node which creates an
   * serializable from given object_base_producer. The node
   * gets the given type name t.
   * 
   * @param tree The node containing tree.
   * @param type The type name of the node.
   * @param proto Prototype object of the node
   * @param abstract Tells the node if its prototype is abstract.
   */
  template < class T >
  prototype_node(object_store *tree, const char *type, T *proto, bool abstract = false)
    : tree_(tree)
    , first(new prototype_node)
    , last(new prototype_node)
    , type_(type)
    , abstract_(abstract)
    , type_index_(typeid(T))
    , deleter_(&destroy<T>)
    , notifier_(&notify_observer<T>)
    , prototype(proto)
  {
    first->next = last.get();
    last->prev = first.get();
//    std::cout << "creating node " << type << "/" << type_index_.name() << "\n";
  }


  ~prototype_node();

  /**
   * @brief Initializes a prototype_node.
   * 
   * Initializes a prototype_node. The node
   * gets the given type name alias t.
   *
   * @param tree The node containing tree.
   * @param type The type name of this node.
   * @param abstract Tells the node if its prototype is abstract.
   */
  void initialize(object_store *tree, const char *type, bool abstract);

  /**
   * Returns true if serializable proxy list is empty. If self is true, only
   * list of own objects is checked. If self is false, complete list is
   * checked.
   * 
   * @param self If true only elements inside this node are considered.
   * @return True if the node is empty.
   */
  bool empty(bool self) const;
  
  /**
   * Returns the size of the serializable proxy list.
   * 
   * @return The number of objects.
   */
  unsigned long size() const;

  /**
   * Return the type name of this node.
   *
   * @return The type name
   */
  const char* type() const;

  /**
   * Return the type id of this node.
   *
   * @return The type id
   */
  const char* type_id() const;

  /**
   * Appends the given prototype node as a sibling
   * on the same level.
   *
   * @param sibling The new sibling node.
   */
  void append(prototype_node *sibling);
  /**
   * Appends the given prototype node to the list of children.
   * 
   * @param child The child node to add.
   */
  void insert(prototype_node *child);

  /**
   * Inserts a serializable proxy into the
   */
  void insert(object_proxy *proxy);

  /**
   * @brief Removes an serializable proxy from prototype node
   *
   * @param proxy Object proxy to remove
   */
  void remove(object_proxy *proxy);

  /**
   * Delete all objects inside this node
   * if recursive flag is set, delete all
   * objects below this node as well.
   * To adjust the serializable proxy marker for the
   * remaining objects the corresponding
   * prototype tree must be passed
   *
   * @param recursive Indicates wether all or only nodes objects are deleted
   */
  void clear(bool recursive);

  /**
   * Unlinks node from list.
   */
  void unlink();

  /**
   * Returns nodes successor node or NULL if node is last.
   * 
   * @return The next node.
   */
  prototype_node* next_node() const;

  /**
   * Returns nodes successor node or nullptr if node is last.
   * Where given root node is the sentinel not to pass while
   * traversing
   *
   * @param root The sentinel root node not to pass.
   * @return The next node.
   */
  prototype_node* next_node(const prototype_node *root) const;

  /**
   * Return nodes predeccessor node or NULL if node is first.
   * 
   * @return The previous node.
   */
  prototype_node* previous_node() const;

  /**
   * Returns nodes predeccessor node or nullptr if node is last.
   * Where given root node is the sentinel not to pass while
   * traversing
   *
   * @param root The sentinel root node not to pass.
   * @return The previous node.
   */
  prototype_node* previous_node(const prototype_node *root) const;

  /**
   * Returns the corresponding tree/object_store
   *
   * @return The corresponding object_store
   */
  object_store* tree() const;

  /**
   * Returns true if node is child of given parent node.
   * 
   * @param parent The parent node.
   * @return True if node is child of given parent node.
   */
  bool is_child_of(const prototype_node *parent) const;

  /**
   * Returns true if node has children.
   *
   * @return True if node has children.
   */
  bool has_children() const;

  /**
   * Returns true if the serializable represented by this node
   * owns a primary key
   *
   * @return True if serializable owns a primary key
   */
  bool has_primary_key() const;

  /**
   * @brief Returns a pointer to the identifier prototype
   * @return A pointer to the identifier prototype
   */
  basic_identifier* id() const;

  /**
   * @brief Returns true if the node represents an abstract object
   * @return True if the node represents an abstract object
   */
  bool is_abstract() const;

  /**
   * @brief Return the type index of the represented object type
   *
   * @return Type index of the represented object type
   */
  std::type_index type_index() const;

  /**
   * Returns true if this node represents a relation
   * node (has_many_item<T>).
   *
   * @return True if this node represents a relation node.
   */
  bool is_relation_node() const;

  /**
   * Returns the relation node info. This struct contains
   * only valid data if this node represents a n relation
   * node (has_many_item<T>).
   * @return
   */
  const relation_node_info& node_info() const;

  /**
   * Prints the node in graphviz layout to the stream.
   *
   * @param os The ostream to write to.
   * @param pn The prototype_node to be written.
   * @return The modified ostream.
   */
  friend MATADOR_OBJECT_API std::ostream& operator <<(std::ostream &os, const prototype_node &pn);

  /**
   * Find the underlying proxy of the given primary key.
   * If no proxy is found nullptr is returned
   *
   * @param pk The primary key
   * @return The corresponding object_proxy or nullptr
   */
  object_proxy* find_proxy(const std::shared_ptr<basic_identifier> &pk);

  /**
   * Register relation info for a belongs_to relation part
   * identified by the given type index.
   *
   * @param tindex type index for identification.
   * @param relation_info relation info object for belongs_to
   */
  void register_belongs_to(const std::type_index &tindex, const prototype_node::relation_info &relation_info);

  /**
   * Register relation info for a has_one relation part
   * identified by the given type index.
   *
   * @param tindex type index for identification.
   * @param relation_info relation info object for has_one
   */
  void register_has_one(const std::type_index &tindex, const prototype_node::relation_info &relation_info);

  /**
   * Register relation info for a has_many relation part
   * identified by the given type index.
   *
   * @param tindex type index for identification.
   * @param relation_info relation info object for has_many
   */
  void register_has_many(const std::type_index &tindex, const prototype_node::relation_info &relation_info);

private:

  enum notification_type {
    ATTACH,
    DETACH,
    INSERT,
    UPDATE,
    REMOVE
  };

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

  void register_observer(basic_object_store_observer *obs)
  {
    if (type_index_ != obs->index()) {
      std::cout << "not same type\n";
      throw std::runtime_error("not same type");
    }
    observer_list.push_back(obs);
  }

  typedef std::list<basic_object_store_observer*> t_observer_list;
  typedef void (*deleter)(void*, t_observer_list&);
  typedef void (*notifier)(notification_type, prototype_node&, void*, basic_object_store_observer*);

  template <typename T>
  static void destroy(void* p, t_observer_list &ol)
  {
    delete (T*)p;
    for(auto i : ol) {
      delete (object_store_observer<T>*)i;
    }
  }

  void on_attach()
  {
    notify(ATTACH);
  }

  void on_detach()
  {
    notify(DETACH);
  }

  void notify(notification_type t) {
    if (!notifier_) {
      return;
    }
    for (auto i : observer_list) {
      notifier_(t, *this, prototype, i);
    }
  }

  template < typename T >
  static void notify_observer(notification_type t, prototype_node &pt, void *p, basic_object_store_observer *obs)
  {
    switch (t) {
      case ATTACH:
        static_cast<object_store_observer<T>*>(obs)->on_attach(pt, *(T*)p);
        break;
      case DETACH:
        static_cast<object_store_observer<T>*>(obs)->on_detach(pt, *(T*)p);
        break;
      case INSERT:
        break;
      case UPDATE:
        break;
      case REMOVE:
        break;
      default:
        break;
    }
  }

private:
  friend class prototype_tree;
  friend class object_holder;
  friend class object_store;
  template < class T >
  friend class object_view;
  template < class T >
  friend class const_object_view_iterator;
  template < class T >
  friend class object_view_iterator;
  template < class T, template <class ...> class C >
  friend class has_many;
  template < class T, template <class ...> class C, class Enabled >
  friend class detail::has_many_inserter;
  template < class T, template <class ...> class C, class Enabled >
  friend class detail::has_many_deleter;
  friend class detail::basic_node_analyzer;
  template < class T,  template < class U = T > class O >
  friend class detail::node_analyzer;

  object_store *tree_ = nullptr;   /**< The prototype tree to which the node belongs */

  // tree links
  prototype_node *parent = nullptr;       /**< The parent node */
  prototype_node *prev = nullptr;         /**< The previous node */
  prototype_node *next = nullptr;         /**< The next node */
  std::unique_ptr<prototype_node> first;  /**< The first children node */
  std::unique_ptr<prototype_node> last;   /**< The last children node */

  object_proxy *op_first = nullptr;  /**< The marker of the first list node. */
  object_proxy *op_marker = nullptr; /**< The marker of the last list node of the own elements. */
  object_proxy *op_last = nullptr;   /**< The marker of the last list node of all elements. */
  
  unsigned int depth = 0;  /**< The depth of the node inside of the tree. */
  unsigned long count = 0; /**< The total count of elements. */

  std::string type_;	       /**< The type name of the prototype node */

  bool abstract_ = false;        /**< Indicates whether this node holds a producer of an abstract serializable */

  std::type_index type_index_; /**< type index of the represented object type */

  t_observer_list observer_list;
  deleter deleter_ = nullptr;
  notifier notifier_ = nullptr;

  void* prototype = nullptr;

  /**
   * Holds the primary keys of all proxies in this node
   */
  detail::t_identifier_map id_map_; /**< The identifier to object_proxy map */

  /**
   * a primary key prototype to clone from
   */
  std::unique_ptr<basic_identifier> id_;

//  relation_map belongs_to_map_;
//  relation_map has_one_map_;
//  relation_map has_many_map_;

  relation_map relation_info_map_;

  bool is_relation_node_ = false;
  relation_node_info relation_node_info_;
};

template<class T>
prototype_node *prototype_node::make_node(object_store *store, const char *type, bool abstract)
{
  return new prototype_node(store, type, new T, abstract);
}

template<class T>
prototype_node *prototype_node::make_relation_node(object_store *store, const char *type, bool abstract,
                                                   const char *owner_type, const char *relation_id,
                                                   const char *owner_column, const char *item_column)
{
  prototype_node *node = make_node<T>(store, type, abstract);
  node->relation_node_info_.owner_type_.assign(owner_type);
  node->relation_node_info_.relation_id_.assign(relation_id);
  node->relation_node_info_.owner_id_column_.assign(owner_column);
  node->relation_node_info_.item_id_column_.assign(item_column);
  node->is_relation_node_ = true;
  return node;
}

}

#endif /* PROTOTYPE_NODE_HPP */
