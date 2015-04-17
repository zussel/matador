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

#include <map>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

namespace oos {

class object;
class prototype_tree;
class object_proxy;
class primary_key_base;

/**
 * @class prototype_node
 * @brief Holds the prototype of a concrete object.
 *
 * The prototype_node class holds the prototype of
 * a concrete object inside a producer object. Whenever
 * requested the class produces a new object.
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
   * object from given object_base_producer. The node
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
   * Returns true if object proxy list is empty. If self is true, only
   * list of own objects is checked. If self is false, complete list is
   * checked.
   * 
   * @param self If true only elements inside this node are considered.
   * @return True if the node is empty.
   */
  bool empty(bool self) const;
  
  /**
   * Returns the size of the object proxy list.
   * 
   * @return The number of objects.
   */
  unsigned long size() const;
  
  /**
   * Appends the given prototype node to the list of children.
   * 
   * @param child The child node to add.
   */
  void insert(prototype_node *child);

  /**
   * Inserts a object proxy into the
   */
  void insert(object_proxy *proxy);

  /**
   * @brief Removes an object proxy from prototype node
   *
   * @param oproxy Object proxy to remove
   */
  void remove(object_proxy *proxy);

  /**
   * Delete all objects inside this node
   * if recursive flag is set, delete all
   * objects below this node as well.
   * To adjust the object proxy marker for the
   * remaining objects the corresponding
   * prototype tree must be passed
   *
   * @param tree The corresponding prototype tree
   * @param recursive Indicates wether all or only nodes objects are deleted
   */
  void clear(prototype_tree &tree, bool recursive);

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
   * Returns true if the object represented by this node
   * owns a primary key
   *
   * @return True if object owns a primary key
   */
  bool has_primary_key() const;
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
  std::unique_ptr<object_base_producer> producer = nullptr; /**< The object producer */

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

  std::string type;	   /**< The type name of the object */
  
  bool abstract = false;        /**< Indicates whether this node holds a producer of an abstract object */

  primary_key_serializer pk_serializer;
  std::unordered_map<std::shared_ptr<primary_key_base>, object_proxy*> primary_key_map;

  std::unique_ptr<primary_key_base> primary_key;

  typedef std::unordered_map<std::string, std::shared_ptr<primary_key_base> > t_foreign_key_map;
  t_foreign_key_map foreign_keys;
};

}

#endif /* PROTOTYPE_NODE_HPP */
