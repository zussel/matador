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
#include "matador/utils/memory.hpp"

#include "matador/object/identifier_proxy_map.hpp"
#include "matador/object/typed_object_store_observer.hpp"
#include "matador/object/relation_field_endpoint.hpp"
#include "matador/object/prototype_info.hpp"

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace matador {

class object_store;
class object_proxy;

/// @cond MATADOR_DEV
namespace detail {
class basic_node_analyzer;
template < class T, template < class U = T > class O >
class node_analyzer;
class object_inserter;
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
public:
  // copying not permitted
  prototype_node(const prototype_node&) = delete;
  prototype_node& operator=(const prototype_node&) = delete;

public:
  /// @cond MATADOR_DEV

//  typedef std::unordered_map<std::type_index, std::shared_ptr<detail::relation_field_endpoint>> t_endpoint_map;

  typedef detail::abstract_prototype_info::t_endpoint_map::const_iterator const_endpoint_iterator;
  typedef detail::abstract_prototype_info::t_endpoint_map::iterator endpoint_iterator;

  struct relation_node_info
  {
    basic_identifier *owner_id_ = nullptr;
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
   * @param prototype The one prototype object
   * @param abstract Flag indicating if node is abstract.
   * @return The created node.
   */
  template < class T >
  static prototype_node* make_node(object_store *store, const char *type, T* prototype, bool abstract = false);

  /**
   * Creates a relation prototype node.
   *
   * @tparam T Type of the node
   * @param store Corresponding object_store
   * @param type Type name
   * @param item Prototype item
   * @param abstract Flag indicating if node is abstract.
   * @param owner_type Type name of the owner node
   * @param relation_id Name of the relation in the prototype object
   * @return The created node.
   */
  template < class T >
  static prototype_node* make_relation_node(object_store *store, const char *type,
                                            T *item, bool abstract,
                                            const char *owner_type, const char *relation_id);

  prototype_node() = default;

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
    : info_(matador::make_unique<detail::prototype_info<T>>(*this, proto))
    , tree_(tree)
    , first(new prototype_node)
    , last(new prototype_node)
    , type_(type)
    , abstract_(abstract)
  {
    first->next = last.get();
    last->prev = first.get();
  }


  ~prototype_node() = default;

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
   * capacity objects the corresponding
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
   * Create the represented object. The type
   * is checked. If the type is invalid nullptr
   * is returned.
   *
   * @tparam T Type of object to create
   * @return A new instance or nullptr
   */
  template < class T >
  T* create() const;

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
   * @param prnt The parent node.
   * @return True if node is child of given parent node.
   */
  bool is_child_of(const prototype_node *prnt) const;

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
   * Registers an object store observer for the object
   * type of the node. The observer is notified
   * on attach, detach, insert, update and delete.
   * 
   * @tparam T The object type of the observer
   * @param obs The observer to register
   */
  template < class T >
  void register_observer(typed_object_store_observer<T> *obs);

  /**
   * Returns the prototype object of the node
   * 
   * @tparam T The object type of the prototype object
   * @return The prototype object
   */
  template < class T >
  T* prototype() const;

  /**
   * Find the underlying proxy of the given primary key.
   * If no proxy is found nullptr is returned
   *
   * @param pk The primary key
   * @return The corresponding object_proxy or nullptr
   */
  object_proxy* find_proxy(basic_identifier *pk);

  /**
   * Register relation_field_endpoint identified by the given type index.
   *
   * @param tindex type index for identification.
   * @param endpoint pointer to a relation_field_endpoint object
   */
  void register_relation_endpoint(const std::type_index &tindex,
                                  const std::shared_ptr<detail::basic_relation_endpoint> &endpoint);

  /**
   * Unregister relation_field_endpoint identified by the given type index.
   *
   * @param tindex
   */
  void unregister_relation_endpoint(const std::type_index &tindex);

/// @cond MATADOR_DEV
  const_endpoint_iterator find_endpoint(const std::type_index &tindex) const;
  endpoint_iterator find_endpoint(const std::type_index &tindex);

  const_endpoint_iterator find_endpoint(const std::string &field) const;
  endpoint_iterator find_endpoint(const std::string &field);

  endpoint_iterator endpoint_begin();
  const_endpoint_iterator endpoint_begin() const;

  endpoint_iterator endpoint_end();
  const_endpoint_iterator endpoint_end() const;

  std::size_t endpoints_size() const;
  bool endpoints_empty() const;

  const detail::abstract_prototype_info::t_endpoint_map& endpoints() const;
/// @endcond

private:

  /**
   * @internal
   *
   * Adjust first marker of all successor nodes with given serializable proxy.
   *
   * @param old_proxy The old first marker proxy.
   * @param new_proxy The new first marker proxy.
   */
  static void adjust_right_marker(prototype_node *root, object_proxy *old_proxy, object_proxy *new_proxy);

  /**
   * @internal
   *
   * Adjusts self and last marker of all predeccessor nodes with given
   * serializable proxy.
   *
   * @param old_proxy The old last marker proxy.
   * @param new_proxy The new last marker proxy.
   */
  static void adjust_left_marker(prototype_node *root, object_proxy *old_proxy, object_proxy *new_proxy);

  void on_attach() const;
  void on_detach() const;
  void on_insert_proxy(object_proxy *proxy) const;
  void on_update_proxy(object_proxy *proxy) const;
  void on_delete_proxy(object_proxy *proxy) const;

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
  friend class detail::basic_node_analyzer;
  template < class T,  template < class U = T > class O >
  friend class detail::node_analyzer;
  friend class detail::object_inserter;

  std::unique_ptr<matador::detail::abstract_prototype_info> info_;

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

  /**
   * Holds the primary keys of all proxies in this node
   */
  detail::t_identifier_map id_map_; /**< The identifier to object_proxy map */

  /**
   * a primary key prototype to clone from
   */
  std::unique_ptr<basic_identifier> id_;

//  t_endpoint_map relation_field_endpoint_map_;

  bool is_relation_node_ = false;
  relation_node_info relation_node_info_;
};

template<class T>
T *prototype_node::create() const
{
  return static_cast<T*>(info_->create());
}

template<class T>
void prototype_node::register_observer(typed_object_store_observer<T> *obs)
{
  info_->register_observer(obs);
}

template<class T>
T *prototype_node::prototype() const
{
  return static_cast<T*>(info_->prototype());
}

template<class T>
prototype_node *prototype_node::make_node(object_store *store, const char *type, T *prototype, bool abstract)
{
  return new prototype_node(store, type, prototype, abstract);
}

template<class T>
prototype_node *prototype_node::make_relation_node(object_store *store, const char *type,
                                                   T *item, bool abstract,
                                                   const char *owner_type, const char *relation_id)
{
  prototype_node *node = make_node<T>(store, type, item, abstract);
  node->relation_node_info_.owner_type_.assign(owner_type);
  node->relation_node_info_.relation_id_.assign(relation_id);
  node->relation_node_info_.owner_id_column_.assign(item->left_column());
  node->relation_node_info_.item_id_column_.assign(item->right_column());
  node->is_relation_node_ = true;
  return node;
}

}

#endif /* PROTOTYPE_NODE_HPP */
