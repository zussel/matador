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

#include "tools/identifier.hpp"

#include "object/identifier_proxy_map.hpp"

#include <map>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

namespace oos {

class object_store;
class object_proxy;

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
class OOS_API prototype_node
{
private:
  // copying not permitted
  prototype_node(const prototype_node&) = delete;
  prototype_node& operator=(const prototype_node&) = delete;

public:
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
   * @param typeinfo The typeinfo of this node.
   * @param abstract Tells the node if its prototype is abstract.
   */
  prototype_node(object_store *tree, const char *type, const std::type_info &typeinfo, bool abstract = false)
    : tree_(tree)
    , first(new prototype_node)
    , last(new prototype_node)
    , type_(type)
    , abstract_(abstract)
    , type_index_(typeinfo)
  {
    first->next = last.get();
    last->prev = first.get();
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
   * @brief Returns the count of relations
   * @return The count of relations
   */
  size_t relation_count() const;

  /**
   * @brief Returns true if a relation with the given name exists
   *
   * Returns true if a relation with the given name exists in the
   * object prototype.
   *
   * @param relation_name The name of the relation to be checked
   * @return True if a relation with the given name exists
   */
  bool has_relation(const std::string &relation_name) const;

  /**
   * @brief Returns the count of foreign keys
   * @return The count of foreign keys
   */
  size_t foreign_key_count() const;

  /**
   * @brief Returns true if a foreign key with the given name exists
   *
   * Returns true if a foreign key with the given name exists in the
   * object prototype.
   *
   * @param foreign_key_name The name of the foreign key to be checked
   * @return True if a relation with the given name exists
   */
  bool has_foreign_key(const std::string &foreign_key_name) const;

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

  /// @cond OOS_DEV

  void register_foreign_key(const char *id, const std::shared_ptr<basic_identifier> &foreign_key);
  void register_relation(const char *type, prototype_node *node, const char *id);
  void prepare_foreign_key(prototype_node *master_node, const char *id);

  /// @endcond

  /**
   * Prints the node in graphviz layout to the stream.
   *
   * @param os The ostream to write to.
   * @param pn The prototype_node to be written.
   * @return The modified ostream.
   */
  friend OOS_API std::ostream& operator <<(std::ostream &os, const prototype_node &pn);

  /**
   * Find the underlying proxy of the given primary key.
   * If no proxy is found nullptr is returned
   *
   * @param pk The primary key
   * @return The corresponding object_proxy or nullptr
   */
  object_proxy* find_proxy(const std::shared_ptr<basic_identifier> &pk);

private:

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

private:
  friend class prototype_tree;
  friend class object_store;
  template < class T >
  friend class object_view;
  template < class T >
  friend class const_object_view_iterator;
  template < class T >
  friend class object_view_iterator;

  /*
   * The field_prototype_map_t contains a map
   * of all foreign key field relations, where each value
   * is a pair of the foreign prototype and its
   * corresponding field
   */
  typedef std::pair<prototype_node*, std::string> prototype_field_info_t;    /**< Shortcut for prototype fieldname pair. */
  typedef std::map<std::string, prototype_field_info_t> field_prototype_map_t; /**< Holds the fieldname and the prototype_node. */

  object_store *tree_ = nullptr;   /**< The prototype tree to which the node belongs */

  // tree links
  prototype_node *parent = nullptr;       /**< The parent node */
  prototype_node *prev = nullptr;         /**< The previous node */
  prototype_node *next = nullptr;         /**< The next node */
  std::unique_ptr<prototype_node> first;  /**< The first children node */
  std::unique_ptr<prototype_node> last;   /**< The last children node */

  /* this map holds information about
   * all prototypes in which this prototype
   * is used as a child item (to many
   * relation). The string tells the name
   * of the attribute
   */
  field_prototype_map_t relations; /**< Map holding relation information for type. */

  object_proxy *op_first = nullptr;  /**< The marker of the first list node. */
  object_proxy *op_marker = nullptr; /**< The marker of the last list node of the own elements. */
  object_proxy *op_last = nullptr;   /**< The marker of the last list node of all elements. */
  
  unsigned int depth = 0;  /**< The depth of the node inside of the tree. */
  unsigned long count = 0; /**< The total count of elements. */

  std::string type_;	       /**< The type name of the prototype node */

  bool abstract_ = false;        /**< Indicates whether this node holds a producer of an abstract serializable */

  std::type_index type_index_; /**< type index of the represented object type */

  /**
   * Holds the primary keys of all proxies in this node
   */
  detail::t_identifier_map id_map_; /**< The identifier to object_proxy map */

  /**
   * a primary key prototype to clone from
   */
  std::unique_ptr<basic_identifier> id_;

  /**
   * a list of prototype_node and ids for
   * which the relation map is yet to be filled
   * once the object type is really inserted
   * this list is processed
   */
  typedef std::list<std::pair<prototype_node*, std::string> > t_node_id_list;
  t_node_id_list foreign_key_ids; /**< The foreign key id list */

  /**
   * a list of all foreign keys inside nodes object
   */
  typedef std::unordered_map<std::string, std::shared_ptr<basic_identifier> > t_foreign_key_map;
  t_foreign_key_map foreign_keys; /**< The foreign key map */
};

}

#endif /* PROTOTYPE_NODE_HPP */
