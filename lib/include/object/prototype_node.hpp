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

#include "object/primary_key_serializer.hpp"
#include "object/object_producer.hpp"
#include "object_proxy.hpp"
#include "primary_key.hpp"

#include <map>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

namespace oos {

class serializable;
class prototype_tree;
class object_proxy;
class primary_key_base;

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
   * @brief Creates a prototype_node only with typename
   *
   * Creates a new prototype node only with typename and
   * corresponding tree. Concrete type and producer will be
   * added later.
   *
   * @param tr The corresponding prototype_tree
   * @param t The typename
   */
  prototype_node(prototype_tree *tr, const char *t);

  /**
   * @brief Creates a new prototype_node.
   * 
   * Creates a new prototype_node which creates an
   * serializable from given object_base_producer. The node
   * gets the given type name t.
   * 
   * @param tr The node containing tree.
   * @param p The object_base_producer.
   * @param t The type name of this node.
   * @param a Tells the node if its prototype is abstract.
   */
  prototype_node(prototype_tree *tr, object_base_producer *p, const char *t, bool a = false);

  ~prototype_node();

  /**
   * @brief Initializes a prototype_node.
   * 
   * Initializes a prototype_node. The node
   * gets the given type name alias t.
   *
   * @param tr The node containing tree.
   * @param p The object_base_producer.
   * @param t The type name of this node.
   * @param a Tells the node if its prototype is abstract.
   */
  void initialize(prototype_tree *tr, object_base_producer *p, const char *t, bool a);

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
   * Appends the given prototype node as a sibling
   * on the same level.
   *
   * @params sibling The new sibling node.
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
   * @param oproxy Object proxy to remove
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
  prototype_node* next_node(const prototype_node *root) const;

  /**
   * Return nodes predeccessor node or NULL if node is first.
   * 
   * @return The previous node.
   */
  prototype_node* previous_node() const;
  prototype_node* previous_node(const prototype_node *root) const;

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
   * Find the underlying proxy of the given primary key.
   * If no proxy is found nullptr is returned
   *
   * @param pk The primary key
   * @return The corresponding object_proxy or nullptr
   */
  object_proxy* find_proxy(const std::shared_ptr<primary_key_base> &pk);

  /**
   * Prints the node in graphviz layout to the stream.
   * 
   * @param os The ostream to write to.
   * @param pn The prototype_node to be written.
   * @return The modified ostream.
   */
  friend std::ostream& operator <<(std::ostream &os, const prototype_node &pn);

  typedef std::pair<prototype_node*, std::string> prototype_field_info_t;    /**< Shortcut for prototype fieldname pair. */
  typedef std::map<std::string, prototype_field_info_t> field_prototype_map_t; /**< Holds the fieldname and the prototype_node. */

  prototype_tree *tree = nullptr;   /**< The prototype tree to which the node belongs */

  // tree links
  prototype_node *parent = nullptr; /**< The parent node */
  prototype_node *prev = nullptr;   /**< The previous node */
  prototype_node *next = nullptr;   /**< The next node */
  std::unique_ptr<prototype_node> first = nullptr;  /**< The first children node */
  std::unique_ptr<prototype_node> last = nullptr;   /**< The last children node */

  // data
  std::unique_ptr<object_base_producer> producer = nullptr; /**< The serializable producer */

  /* this map holds information about
   * all prototypes in which this prototype
   * is used as a child item (one to many
   * relation). The string tells the name
   * of the attribute
   */
  field_prototype_map_t relations; /**< Map holding relation information for type. */

  object_proxy *op_first = nullptr;  /**< The marker of the first list node. */
  object_proxy *op_marker = nullptr; /**< The marker of the last list node of the own elements. */
  object_proxy *op_last = nullptr;   /**< The marker of the last list node of all elements. */
  
  unsigned int depth = 0;  /**< The depth of the node inside of the tree. */
  unsigned long count = 0; /**< The total count of elements. */

  std::string type;	   /**< The type name of the serializable */
  
  bool abstract = false;        /**< Indicates whether this node holds a producer of an abstract serializable */

  primary_key_serializer pk_serializer;

  /*
   * Holds the primary keys of all proxies in this node
   */
  typedef std::shared_ptr<primary_key_base> pk_ptr;
  struct pk_equal : public std::binary_function<pk_ptr, pk_ptr, bool>
  {
      bool operator()(const pk_ptr &a, const pk_ptr &b) const { return *a == *b; }
  };

  typedef std::unordered_map<pk_ptr, object_proxy*, std::hash<pk_ptr>, pk_equal> t_primary_key_map;
  t_primary_key_map primary_key_map;

  /*
   * a primary key prototype to clone from
   */
  std::unique_ptr<primary_key_base> primary_key;

  /*
   * a list of prototype_node and ids for
   * which the relation map is yet to be filled
   * once the object type is really inserted
   * this list is processed
   */
  typedef std::list<std::pair<prototype_node*, std::string> > t_node_id_list;
  t_node_id_list foreign_key_ids;

  /*
   * a list of all foreign keys inside nodes object
   */
  typedef std::unordered_map<std::string, std::shared_ptr<primary_key_base> > t_foreign_key_map;
  t_foreign_key_map foreign_keys;
};

}

#endif /* PROTOTYPE_NODE_HPP */
