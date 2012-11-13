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

#ifndef OBJECTVIEW_HPP
#define OBJECTVIEW_HPP

#include "object/object_store.hpp"
#include "object/object_proxy.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_node.hpp"

#include <sstream>
#include <algorithm>

namespace oos {

#ifndef OOS_DOXYGEN_DOC
template < class T > class const_object_view_iterator;
#endif /* OOS_DOXYGEN_DOC */

/// @cond OOS_DEV

/**
 * @class object_view_iterator
 * @brief Iterator class for an object_view
 * @tparam T Object type of the iterator
 * 
 * This iterator is used by the object_view
 * class.
 */
template < class T >
class object_view_iterator : public std::iterator<std::bidirectional_iterator_tag, T>
{
public:
  typedef object_view_iterator<T> self;	/**< Shortcut for this class. */
  typedef object_ptr<T> value_type;     /**< Shortcut for the value type. */
  typedef T* pointer;                   /**< Shortcut for the pointer type. */
  typedef value_type& reference ;       /**< Shortcut for the reference type */

  /**
   * Creates an empty iterator
   */
  object_view_iterator()
    : node_(NULL)
  {}
  
  /**
   * @brief Creates a iterator for a concrete type.
   * 
   * This constructor creates an iterator for a concrete
   * type and a concrete object.
   * 
   * @param node The prototype_node of the object_proxy
   * @param current The object_proxy for the iterator
   * @param last The last object_proxy for the iterator
   */
  object_view_iterator(const prototype_node *node, object_proxy *current, object_proxy *last)
    : node_(node)
    , current_(current)
    , last_(last)
  {}

  /**
   * Copy from a given object_view_iterator.
   * 
   * @param x The object_view_iterator to copy from.
   */
  object_view_iterator(const object_view_iterator &x)
    : node_(x.node_)
    , current_(x.current_)
    , last_(x.last_)
  {}

  /**
   * Assign from a given object_view_iterator.
   * 
   * @param x The object_view_iterator to assign from.
   * @return The assigned object_view_iterator.
   */
  object_view_iterator& operator=(const object_view_iterator &x)
  {
    node_ = x.node_;
    current_ = x.current_;
    last_ = x.last_;
    return *this;
  }

  ~object_view_iterator() {}

  /**
   * @brief Compares this with another iterators.
   * 
   * Compares this with another iterators. Returns true
   * if the iterators current object_proxy are the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are the same.
   */
  bool operator==(const object_view_iterator &i) const {
    return (current_ == i.current_);
  }

  /**
   * @brief Compares this with another iterators.
   * 
   * Compares this with another iterators. Returns true
   * if the iterators current object_proxy are not the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are not the same.
   */
  bool operator!=(const object_view_iterator &i) const {
    return (current_ != i.current_);
  }

  /**
   * Pre increments the iterator
   * 
   * @return Returns iterators successor.
   */
  self& operator++() {
    increment();
    return *this;
  }

  /**
   * Post increments the iterator
   * 
   * @return Returns iterator before incrementing.
   */
  self operator++(int) {
    object_proxy *tmp = current_;
    increment();
    return object_view_iterator(node_, tmp, last_);
  }

  /**
   * Pre increments the iterator
   * 
   * @return Returns iterators predeccessor.
   */
  self& operator--() {
    decrement();
    return *this;
  }

  /**
   * Post decrements the iterator
   * 
   * @return Returns iterator before decrementing.
   */
  self operator--(int) {
    object_proxy *tmp = current_;
    decrement();
    return object_view_iterator(node_, tmp, last_);
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->() const
  {
    return static_cast<pointer>(current_->obj);
  }

  /**
   * Returns the node.
   * 
   * @return The iterators underlaying node.
   */
  value_type operator*() const
  {
    return this->optr();
  }

  /**
   * Returns the node as object_ptr.
   * 
   * @return The iterators underlaying node as object_ptr.
   */
  value_type optr() const
  {
    if (current_->obj)
      return value_type(current_->obj);
    else
      return value_type();
  }

private:
  void increment() {
    if (current_ != last_) {
      current_ = current_->next;
    }
  }
  void decrement() {
    if (current_ != node_->op_first) {
      current_ = current_->prev;
    }
  }

private:
  friend class const_object_view_iterator<T>;

  const prototype_node *node_;
  object_proxy *current_;
  object_proxy *last_;
};

/**
 * @class const_object_view_iterator
 * @brief Iterator class for an object_view
 * @tparam T Object type of the iterator
 * 
 * This iterator is used by the object_view
 * class.
 */
template < class T >
class const_object_view_iterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
{
public:
  typedef const_object_view_iterator<T> self;	/**< Shortcut for this class. */
  typedef object_ptr<T> value_type;           /**< Shortcut for the value type. */
  typedef T* pointer;                         /**< Shortcut for the pointer type. */
  typedef value_type& reference ;             /**< Shortcut for the reference type */

  /**
   * Creates an empty iterator
   */
  const_object_view_iterator()
    : node_(NULL)
  {}

  /**
   * @brief Creates a iterator for a concrete type.
   * 
   * This constructor creates an iterator for a concrete
   * type and a concrete object.
   * 
   * @param node The prototype_node of the object_proxy
   * @param current The object_proxy for the iterator
   * @param last The last object_proxy for the iterator
   */
  const_object_view_iterator(const prototype_node *node, object_proxy *current, object_proxy *last)
    : node_(node)
    , current_(current)
    , last_(last)
  {}

  /**
   * Copy from a given const_object_view_iterator.
   * 
   * @param x The const_object_view_iterator to copy from.
   */
  const_object_view_iterator(const const_object_view_iterator &x)
    : node_(x.node_)
    , current_(x.current_)
    , last_(x.last_)
  {}

  /**
   * Copy from a given object_view_iterator.
   * 
   * @param x The object_view_iterator to copy from.
   */
  const_object_view_iterator(const object_view_iterator<T> &x)
    : node_(x.node_)
    , current_(x.current_)
    , last_(x.last_)
  {}

  /**
   * Assign from a given const_object_view_iterator.
   * 
   * @param x The const_object_view_iterator to assign from.
   * @return The assigned const_object_view_iterator.
   */
  const_object_view_iterator& operator=(const const_object_view_iterator &x)
  {
    node_ = x.node_;
    current_ = x.current_;
    last_ = x.last_;
    return *this;
  }

  /**
   * Assign from a given object_view_iterator.
   * 
   * @param x The object_view_iterator to assign from.
   * @return The assigned const_object_view_iterator.
   */
  const_object_view_iterator& operator=(const object_view_iterator<T> &x)
  {
    node_ = x.node_;
    current_ = x.current_;
    last_ = x.last_;
    return *this;
  }

  ~const_object_view_iterator() {}

  /**
   * @brief Compares this with another iterators.
   * 
   * Compares this with another iterators. Returns true
   * if the iterators current object_proxy are the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are the same.
   */
  bool operator==(const const_object_view_iterator &i) const {
    return (current_ == i.current_);
  }

  /**
   * @brief Compares this with another iterators.
   * 
   * Compares this with another iterators. Returns true
   * if the iterators current object_proxy are not the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are not the same.
   */
  bool operator!=(const const_object_view_iterator &i) const {
    return (current_ != i.current_);
  }

  /**
   * Pre increments the iterator
   * 
   * @return Returns iterators successor.
   */
  self& operator++() {
    increment();
    return *this;
  }

  /**
   * Post increments the iterator
   * 
   * @return Returns iterator before incrementing.
   */
  self operator++(int) {
    object_proxy *tmp = current_;
    increment();
    return const_object_view_iterator(node_, tmp, last_);
  }

  /**
   * Pre increments the iterator
   * 
   * @return Returns iterators predeccessor.
   */
  self& operator--() {
    decrement();
    return *this;
  }

  /**
   * Post decrements the iterator
   * 
   * @return Returns iterator before decrementing.
   */
  self operator--(int) {
    object_proxy *tmp = current_;
    decrement();
    return const_object_view_iterator(node_, tmp, last_);
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->() const
  {
    return static_cast<pointer>(current_->obj);
  }

  /**
   * Returns the node.
   * 
   * @return The iterators underlaying node.
   */
  value_type operator*() const
  {
    return this->optr();
  }

  /**
   * Returns the node as object_ptr.
   * 
   * @return The iterators underlaying node as object_ptr.
   */
  value_type optr() const {
    if (current_->obj)
      return value_type(current_->obj);
    else
      return value_type();
  }

private:
  void increment() {
    if (current_ != last_) {
      current_ = current_->next;
    }
  }
  void decrement() {
    if (current_ != node_->op_first) {
      current_ = current_->prev;
    }
  }

private:
  const prototype_node *node_;
  object_proxy *current_;
  object_proxy *last_;
};
/// @endcond

/**
 *
 * @class generic_view
 * @brief Creates a generic view of a concrete object type
 *
 * The generic_view class creates a generic view over an object type
 * without casting the object to the concrete type. The class deals
 * with the base class oos::object.
 * When creating it is possible to have the view only over
 * the given type or over the complete subset of objects including
 * child objects.
 */

class generic_view
{
public:
  typedef object_view_iterator<object> iterator;             /**< Shortcut to the iterator type */
  typedef const_object_view_iterator<object> const_iterator; /**< Shortcut to the const_iterator type */
  typedef object_ptr<object> object_pointer;                 /**< Shortcut to object pointer */

  /**
   * @brief Creates an generic_view.
   * 
   * Creates an generic_view over the given object type provided
   * by the type string within the given object_store. When the
   * skip_siblings flag is true, the view only has object of the given
   * type.
   *
   * @param type The type of the object.
   * @param ostore The object_store containing the objects.
   * @param skip_siblings If true only objects of the concrete type are part of the view.
   */
  generic_view(const std::string &type, const object_store &ostore, bool skip_siblings = false)
    : ostore_(ostore)
    , skip_siblings_(skip_siblings)
    , node_(NULL)
  {
    node_ = ostore_.find_prototype(type.c_str());
		if (!node_) {
      std::stringstream str;
      str << "couldn't find object type [" << type << "]";
      throw object_exception(str.str().c_str());
    }
  }

  /**
   * Return the begin of the generic_view.
   * 
   * @return The begin iterator.
   */
  iterator begin() {
    if (skip_siblings_) {
      return ++iterator(node_, node_->op_first, node_->op_marker);
    } else {
      return ++iterator(node_, node_->op_first, node_->op_last);
    }
  }

  /**
   * Return the begin of the generic_view.
   * 
   * @return The begin iterator.
   */
  const_iterator begin() const {
    if (skip_siblings_) {
      return ++const_iterator(node_, node_->op_first, node_->op_marker);
    } else {
      return ++const_iterator(node_, node_->op_first, node_->op_last);
    }
  }

  /**
   * Return the end of the generic_view.
   * 
   * @return The end iterator.
   */
  iterator end() {
    if (skip_siblings_) {
      return iterator(node_, node_->op_marker, node_->op_marker);
    } else {
      return iterator(node_, node_->op_last, node_->op_last);
    }
  }

  /**
   * Return the end of the generic_view.
   * 
   * @return The end iterator.
   */
  const_iterator end() const {
    if (skip_siblings_) {
      return const_iterator(node_, node_->op_marker, node_->op_marker);
    } else {
      return const_iterator(node_, node_->op_last, node_->op_last);
    }
  }

  /**
   * Return the very first object_ptr
   * of the generic_view.
   *
   * @return The first object_ptr of the view.
   */
  object_pointer front() const
  {
    return (*begin());
  }

  /**
   * Return the very last object_ptr
   * of the generic_view.
   *
   * @return The last object_ptr of the view.
   */
  object_pointer back() const
  {
    return (*(--end()));
  }

  /**
   * Returns true if the generic_view is empty.
   * 
   * @return True if object_view is empty.
   */
  bool empty() const {
    return node_->op_first->next == node_->op_last;
  }

  /**
   * Return the size of the generic_view.
   * 
   * @return The size of the generic_view.
   */
  size_t size() const {
    return std::distance(begin(), end());
  }
  
  /**
   * @brief Sets the skip siblings flag.
   * 
   * When set to true all objects which are
   * not of the concrete type are omitted.
   *
   * @param skip Skips siblings when true.
   */
  void skip_siblings(bool skip) {
    skip_siblings_ = skip;
  }

  /**
   * Find object which matches the given condition
   *
   * @tparam Predicate The type for the find predicate
   * @param pred The find predicate
   * @return The first iterator with the object matching the condition.
   */
  template < class Predicate >
  const_iterator find_if(Predicate pred) const
  {
    return std::find_if(begin(), end(), pred);
  }

  /**
   * Find object which matches the given condition
   *
   * @tparam Predicate The type for the find predicate
   * @param pred The find predicate
   * @return The first iterator with the object matching the condition.
   */
  template < class Predicate >
  iterator find_if(Predicate pred)
  {
    return std::find_if(begin(), end(), pred);
  }

  /**
   * Return the underlaying prototype node
   *
   * @return The underlaying prototype node.
   */
  const prototype_node* node() const
  {
    return node_;
  }

private:
    const object_store &ostore_;
    bool skip_siblings_;
    const prototype_node *node_;
};

/**
 * @class object_view
 * @brief Create a view for a concrete object type
 * @tparam T The type of the object_view.
 * 
 * The object_view class creates a view over an object type.
 * When creating it is possible to have the view only over
 * the given type or over the complete subset of objects including
 * child objects.
 */
template < class T >
class object_view
{
public:
  typedef object_view_iterator<T> iterator;             /**< Shortcut to the iterator type */
  typedef const_object_view_iterator<T> const_iterator; /**< Shortcut to the const_iterator type */
  typedef object_ptr<T> object_pointer;                 /**< Shortcut to object pointer */

  /**
   * @brief Creates an object_view.
   * 
   * Creates an object_view over the given template type
   * T within the given object_store. When the skip_siblings
   * flag is true, the view only has object of type T.
   *
   * @param ostore The object_store containing the objects.
   * @param skip_siblings If true only objects of concrete type T are part of the view.
   */
  object_view(const object_store &ostore, bool skip_siblings = false)
    : ostore_(ostore)
    , skip_siblings_(skip_siblings)
    , node_(NULL)
  {
    node_ = ostore_.find_prototype(typeid(T).name());
		if (!node_) {
      std::stringstream str;
      str << "couldn't find object type [" << typeid(T).name() << "]";
      throw object_exception(str.str().c_str());
    }
  }

  /**
   * Return the begin of the object_view.
   * 
   * @return The begin iterator.
   */
  iterator begin() {
    if (skip_siblings_) {
      return ++iterator(node_, node_->op_first, node_->op_marker);
    } else {
      return ++iterator(node_, node_->op_first, node_->op_last);
    }
  }

  /**
   * Return the begin of the object_view.
   * 
   * @return The begin iterator.
   */
  const_iterator begin() const {
    if (skip_siblings_) {
      return ++const_iterator(node_, node_->op_first, node_->op_marker);
    } else {
      return ++const_iterator(node_, node_->op_first, node_->op_last);
    }
  }

  /**
   * Return the end of the object_view.
   * 
   * @return The end iterator.
   */
  iterator end() {
    if (skip_siblings_) {
      return iterator(node_, node_->op_marker, node_->op_marker);
    } else {
      return iterator(node_, node_->op_last, node_->op_last);
    }
  }

  /**
   * Return the end of the object_view.
   * 
   * @return The end iterator.
   */
  const_iterator end() const {
    if (skip_siblings_) {
      return const_iterator(node_, node_->op_marker, node_->op_marker);
    } else {
      return const_iterator(node_, node_->op_last, node_->op_last);
    }
  }

  /**
   * Return the very first object_ptr
   * of the object_view.
   *
   * @return The first object_ptr of the view.
   */
  object_pointer front() const
  {
    return (*begin());
  }

  /**
   * Return the very last object_ptr
   * of the object_view.
   *
   * @return The last object_ptr of the view.
   */
  object_pointer back() const
  {
    return (*(--end()));
  }

  /**
   * Returns true if the object_view is empty.
   * 
   * @return True if object_view is empty.
   */
  bool empty() const {
    return node_->op_first->next == node_->op_last;
  }

  /**
   * Return the size of the object_view.
   * 
   * @return The size of the object_view.
   */
  size_t size() const {
    return std::distance(begin(), end());
  }
  
  /**
   * @brief Sets the skip siblings flag.
   * 
   * When set to true all objects which are
   * not of the concrete type T are omitted.
   *
   * @param skip Skips siblings when true.
   */
  void skip_siblings(bool skip) {
    skip_siblings_ = skip;
  }

  /**
   * Find object which matches the given condition
   *
   * @tparam Predicate The type for the find predicate
   * @param pred The find predicate
   * @return The first iterator with the object matching the condition.
   */
  template < class Predicate >
  const_iterator find_if(Predicate pred) const
  {
    return std::find_if(begin(), end(), pred);
  }

  /**
   * Find object which matches the given condition
   *
   * @tparam Predicate The type for the find predicate
   * @param pred The find predicate
   * @return The first iterator with the object matching the condition.
   */
  template < class Predicate >
  iterator find_if(Predicate pred)
  {
    return std::find_if(begin(), end(), pred);
  }

  /**
   * Return the underlaying prototype node
   *
   * @return The underlaying prototype node.
   */
  const prototype_node* node() const
  {
    return node_;
  }

private:
    const object_store &ostore_;
    bool skip_siblings_;
    const prototype_node *node_;
};

}

#endif /* OBJECTVIEW_HPP */
