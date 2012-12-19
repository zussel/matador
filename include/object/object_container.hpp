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

#ifndef OBJECT_CONTAINER_HPP
#define OBJECT_CONTAINER_HPP

#include "object/object.hpp"
#include "object/prototype_node.hpp"

#ifdef WIN32
#include <functional>
#else
#include <tr1/functional>
#endif

namespace oos {

/**
 * @tparam T Type of value
 * @class value_item
 * @brief Holder class for the value type
 *
 * This class comes as a proxy between the container
 * class and the values stored in the container.
 */
template < class T >
class value_item : public object
{
public:
  typedef T value_type; /**< Shortcut for the value type. */

public:
  value_item() {}
  /**
   * Copies a value_item.
   * 
   * @param v value_item to copy from.
   */
  value_item(const value_type &v)
    : value_(v)
  {}
  virtual ~value_item() {}

  virtual void deserialize(object_reader &deserializer)
  {
    oos::object::deserialize(deserializer);
    deserializer.read("value", value_);
  }

  virtual void serialize(object_writer &serializer) const
  {
    oos::object::serialize(serializer);
    serializer.write("value", value_);
  }

  /**
   * Return the current value of the value_item.
   * 
   * @return The value of the value_item.
   */
  value_type value() const
  {
    return value_;
  }

  /**
   * Sets a new value for the value_item.
   * 
   * @param v The new value to set.
   */
  void value(const value_type &v)
  {
    modify(value_, v);
  }

private:
  value_type value_;
};

/**
 * @tparam T Type of the container item
 * @tparam C Type of the container
 * @brief Base class for all container items
 * 
 * The class is the base class for all container
 * item classes. It holds the item and a reference
 * to the container to which the item belongs.
 */
template < class T, class C >
class container_item : public value_item<T>
{
public:
  typedef oos::object_ref<C> container_ref; /**< Shortcut for the container ref type. */
  typedef C container_type;                 /**< Shortcut for the container type. */
  typedef T value_type;                     /**< Shortcut for the value type. */
  typedef typename container_type::size_type size_type; /**< Shortcut for the size type. */

  container_item() {}

  /**
   * Creates a container_item with a given
   * reference to its container.
   * 
   * @param c The container reference to set.
   */
  explicit container_item(const container_ref &c)
    : container_(c)
  {}
  
  /**
   * Creates a container_item with a given
   * reference to its container and a value.
   * 
   * @param c The container reference to set.
   * @param v The value of the container_item
   */
  container_item(const container_ref &c, const value_type &v)
    : value_item<value_type>(v)
    , container_(c)
  {}
  virtual ~container_item() {}

  virtual void deserialize(object_reader &deserializer)
  {
    value_item<T>::deserialize(deserializer);
    deserializer.read("container", container_);
  }
  virtual void serialize(object_writer &serializer) const
  {
    value_item<T>::serialize(serializer);
    serializer.write("container", container_);
  }

  /**
   * Return a reference to the container
   *
   * @return A reference to the container
   */
  container_ref container() const
  {
    return container_;
  }

private:
  container_ref container_;
};

/**
 * @cond OOS_DEV
 * @class object_container
 * @brief Base class of all container types
 * 
 * This is the base class of all container types.
 */
class OOS_API object_container
{
public:
  typedef std::tr1::function<void (object *)> node_func; /**< Shortcut to the function type of the for_each method. */
  typedef unsigned long size_type;                       /**< Shortcut for size type. */
//  typedef long unsigned int size_type;                             /**< Shortcut for size type. */

public:
  /**
   * @brief Creates an empty object_container object.
   * 
   * The constructor creates an empty object_container.
   */
  object_container()
    : ostore_(0)
  {}

  virtual ~object_container() {}

  /**
   * Return the class id of the container
   * item type
   * 
   * @return The class id
   */
  virtual const char* classname() const = 0;

  /**
   * Tell wether the list is empty or not.
   * 
   * @return Returns true if the list is empty.
   */
  virtual bool empty() const = 0;

  /**
   * @brief Clears the list.
   * 
   * Removes every element from the list and
   * subsequently from the object_store.
   */
  virtual void clear() = 0;
  
  /**
   * Returns the size of the list.
   * 
   * @return The size of the list.
   */
  virtual size_type size() const = 0;

  /**
   * @brief Returns the containing object_store.
   * 
   * Returns the containing object_store. If the list
   * isn't inserted into an object_store NULL is
   * returned.
   * 
   * @return The containing object_store.
   */
  object_store* ostore() const
  {
    return ostore_;
  }

protected:
  friend class object_store;
  friend class object_creator;
  friend class object_deleter;
  friend class object_serializer;
  friend class relation_handler;

  /**
   * @brief Append a object via its object_proxy.
   *
   * Append a object via its object_proxy to the list.
   *
   * @param op The object_proxy containing the list element object.
   */
  virtual void append_proxy(object_proxy *op) = 0;

  /**
   * Mark the list containing object as modified
   * in the object_store.
   *
   * @param o The object containig list
   */
  void mark_modified(object *o)
  {
    o->mark_modified();
  }

  /**
   * @brief Executes the given function object for all elements.
   *
   * Executes the given function object for all elements.
   *
   * @param nf Function object used to be executed on each element.
   */
  virtual void for_each(const node_func &nf) const = 0;

  /**
   * Provides an interface which is called
   * when inserting the list into the object_store.
   */
  virtual void install(object_store *ostore)
  {
    ostore_ = ostore;
  }

  /**
   * Provides an interface which is called
   * when removing the list from the object_store.
   */
  virtual void uninstall()
  {
    ostore_ = 0;
  }
  
  /**
   * Sets the parent for the
   * concrete container
   *
   * @param p The parent object of the container.
   */
  virtual void parent(object *p) = 0;

  /**
   * Create a producer object for
   * the relation/value table.
   *
   * @param ostore The object_store.
   * @param id The name of the relation.
   * @return The producer object;
   */
  void handle_container_item(object_store &ostore, const char *id, prototype_node *node) const
  {
    object_base_producer *p = create_item_producer();
    if (p) {
      ostore.insert_prototype(p, id);
    }
    // get prototype node of container item (child)
    prototype_node *item_node = this->find_prototype_node(ostore, classname());
    if (!item_node) {
      // if there is no such prototype node
      // insert a new one (it is automatically marked
      // as uninitialzed)
      item_node = new prototype_node();
      ostore.prototype_node_map_[classname()] = item_node;
    }
    // add container node to item node
    std::cout << "DEBUG: storing relation: object type [" << node->producer->classname() << "] has relation field [" << id << "] of type [" << classname() << "]\n";
    prototype_node::type_map_t::iterator i = node->relations.insert(std::make_pair(classname(), prototype_node::field_prototype_node_map_t())).first;
    i->second.insert(std::make_pair(id, item_node));
//    node->relations.push_back(std::make_pair(id, item_node));
  }

  /**
   * Create a producer for the item type.
   * 
   * @return The producer for the item type.
   */
  virtual object_base_producer* create_item_producer() const = 0;

  prototype_node* find_prototype_node(object_store &ostore, const char *id) const
  {
    object_store::t_prototype_node_map::iterator i = ostore.prototype_node_map_.find(id);
    if (i == ostore.prototype_node_map_.end()) {
      return 0;
    }
    return i->second;
  }

private:
  virtual void reset() {}

private:
  object_store *ostore_;
};
/// @endcond

}

#endif /* OBJECT_CONTAINER_HPP */
