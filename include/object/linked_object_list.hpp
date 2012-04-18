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

#ifndef LINKED_OBJECT_LIST_HPP
#define LINKED_OBJECT_LIST_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

#include "object/object_list.hpp"
#include "object/object_atomizer.hpp"
#include "object/object_container.hpp"

/*
 *   linked_object_list layout:
 * 
 *   ID | FIRST | LAST |PREV | NEXT | data
 *   ---+-------+------+-----+------+------+
 *    1 |   1   |   5  |  0  |  2   |    0 |
 *    2 |   1   |   5  |  1  |  3   |    7 |
 *    3 |   1   |   5  |  2  |  4   |    8 |
 *    4 |   1   |   5  |  3  |  5   |    9 |
 *    5 |   1   |   5  |  4  |  0   |    0 |
 */
 
namespace oos {

///@cond OOS_DEV

template < class T, class C >
class linked_object_list_item : public oos::object
{
public:
  typedef oos::object_ref<C> container_ref;
  typedef linked_object_list_item<T, C> self;
  typedef object_ref<self> self_ref;
  typedef T value_type;
  typedef unsigned int size_type;

  linked_object_list_item() {}
  explicit linked_object_list_item(const container_ref &c)
    : container_(c)
  {}
  linked_object_list_item(const container_ref &c, const value_type &v)
    : container_(c)
    , value_(v)
  {}
  virtual ~linked_object_list_item() {}

  virtual void read_from(oos::object_atomizer *oa)
  {
    oos::object::read_from(oa);
    oa->read_object("first", first_);
    oa->read_object("last", last_);
    oa->read_object("prev", prev_);
    oa->read_object("next", next_);
    oa->read_object("container", container_);
//    read(oa, "value", value_);
  }
  virtual void write_to(oos::object_atomizer *oa) const
  {
    oos::object::write_to(oa);
    oa->write_object("first", first_);
    oa->write_object("last", last_);
    oa->write_object("prev", prev_);
    oa->write_object("next", next_);
    oa->write_object("container", container_);
//    write(oa, "value", value_);
  }

  container_ref container() const
  {
    return container_;
  }

  value_type value() const
  {
    return value_;
  }

  void value(const value_type &v)
  {
    modify(value_, v);
  }

private:
  self_ref first_;
  self_ref last_;
  self_ref prev_;
  self_ref next_;
  container_ref container_;
  value_type value_;
};

template < class S, class T > class linked_object_list;
template < class S, class T > class linked_object_list_iterator;
template < class S, class T > class const_linked_object_list_iterator;

/**
 * @cond OOS_DEV
 * @class linked_object_list_base_node
 * @brief Base list node class for all linked list types
 * 
 * This is the base list node class for all further
 * linked list type classes storing objects
 */
class OOS_API linked_object_list_base_node : public object
{
public:
  virtual ~linked_object_list_base_node() {}

  /**
   * Returns list nodes previous list node
   * 
   * @return The previous list node
   */
  virtual linked_object_list_base_node *next_node() const = 0;

  /**
   * Returns list nodes next list node
   * 
   * @return The next list node
   */
  virtual linked_object_list_base_node *prev_node() const = 0;
  
  /**
   * Link the reference node
   * 
   * @param r The reference object.
   * @param n The name of the reference object parameter.
   */
  virtual bool link_reference(object *r, const std::string &n);

  /**
   * Link the reference node
   * 
   * @param o The object to link a reference in.
   * @param r The reference object.
   * @param n The name of the reference object parameter.
   */
  virtual bool link_reference(object *o, object *r, const std::string &n);
};
///@endcond

/// @cond OOS_DEV
/**
 * @class linked_object_list_iterator
 * @brief Iterator class for all linked lists.
 * @tparam T Containing list node type of the iterator.
 * 
 * This is the iterator class used by all linked
 * list types.
 */
template < class S, class T >
class linked_object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
public:
  typedef linked_object_list_iterator<S, T> self;	   /**< Shortcut for this iterator type. */
  typedef T* pointer;                      /**< Shortcut for the pointer type. */
  typedef object_ptr<T> value_type;        /**< Shortcut for the value type. */
  typedef value_type& reference ;          /**< Shortcut for the reference to the value type. */
  typedef linked_object_list<S, T> list_type; /**< Shortcut for the list type. */

  /**
   * Creates an empty iterator
   */
  linked_object_list_iterator()
    : node_(0)
    , list_(0)
  {}
  
  /**
   * @brief Creates an iterator from a list node for a list
   * 
   * The constructor creates an iterator for a given
   * list node in a given list.
   * 
   * @param node The linked list node.
   * @param l The pointer to the list.
   */
  linked_object_list_iterator(const value_type &node, list_type *l)
    : node_(node)
    , list_(l)
  {}

  /**
   * Copies an iterator.
   * 
   * @param x The iterator to copy from.
   */
  linked_object_list_iterator(const linked_object_list_iterator &x)
    : node_(x.node_)
    , list_(x.list_)
  {}

  /**
   * Assign an iterator to this iterator.
   * 
   * @param x The iterator to assign from.
   */
  linked_object_list_iterator& operator=(const linked_object_list_iterator &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }

  ~linked_object_list_iterator() {}

  /**
   * @brief Compares this iterator with another on equality.
   * 
   * The equal operator compares this iterator
   * with a given one. The two iterators are equal
   * if the two nodes are the same.
   * 
   * @param i The iterator to compare with.
   * @return Return true if the iterators are equal.
   */
  bool operator==(const linked_object_list_iterator &i) const {
    return (node_ == i.node_);
  }

  /**
   * @brief Compares this iterator with another on unequality.
   * 
   * The not equal operator compares this iterator
   * with a given one. The two iterators are not equal
   * if the two nodes aren't the same.
   * 
   * @param i The iterator to compare with.
   * @return Return true if the iterators are not equal.
   */
  bool operator!=(const linked_object_list_iterator &i) const {
    return (node_ != i.node_);
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
    self tmp = *this;
    increment();
    return tmp;
  }

  /**
   * Pre decrements the iterator
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
    self tmp = *this;
    decrement();
    return tmp;
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->() const {
    return node_.get();
  }

  /**
   * Returns the node.
   * 
   * @return The iterators underlaying node.
   */
  value_type operator*() const {
    return this->optr();
  }

  /**
   * Returns the node as object_ptr.
   * 
   * @return The iterators underlaying node as object_ptr.
   */
  object_ptr<T> optr() const {
    return node_;
  }

  /**
   * Returns the node as object_ref.
   * 
   * @return The iterators underlaying node as object_ref.
   */
  object_ref<T> oref() const {
    return node_;
  }

private:
  void increment() {
    if (node_ != list_->last_) {
      node_ = node_->next_;
    }
  }
  void decrement() {
    if (node_ != list_->last_) {
      node_ = node_->prev_;
    }
  }

private:
  friend class const_linked_object_list_iterator<S, T>;

  value_type node_;
  list_type *list_;
};

/**
 * @class const_linked_object_list_iterator
 * @brief Constant Iterator class for all linked lists.
 * @tparam T Containing list node type of the iterator.
 * 
 * This is the iterator class used by all linked
 * list types. The containing object is treated
 * as a constant.
 */
template < class S, class T >
class const_linked_object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&> {
public:
  typedef const_linked_object_list_iterator<S, T> self;	/**< Shortcut for this iterator type. */
  typedef T* pointer;                         /**< Shortcut for the pointer type. */
  typedef object_ptr<T> value_type;           /**< Shortcut for the value type. */
  typedef value_type& reference;              /**< Shortcut for the reference to the value type. */
  typedef linked_object_list<S, T> list_type;    /**< Shortcut for the list type. */

  /**
   * Creates an empty const_iterator
   */
  const_linked_object_list_iterator()
    : node_(0)
    , list_(0)
  {}

  /**
   * @brief Creates an const_iterator from a list node for a list
   * 
   * The constructor creates an const_iterator for a given
   * list node in a given list.
   * 
   * @param node The linked list node.
   * @param l The pointer to the list.
   */
  const_linked_object_list_iterator(value_type node, const list_type *l)
    : node_(node)
    , list_(l)
  {}

  /**
   * Copies an const_iterator.
   * 
   * @param x The iterator to copy from.
   */
  const_linked_object_list_iterator(const linked_object_list_iterator<S, T> &x)
    : node_(x.node_)
    , list_(x.list_)
  {}

  /**
   * Assign an iterator to this iterator.
   * 
   * @param x The iterator to assign from.
   */
  const_linked_object_list_iterator& operator=(const linked_object_list_iterator<S, T> &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }

  /**
   * Assign an const_iterator to this const_iterator.
   * 
   * @param x The const_iterator to assign from.
   */
  const_linked_object_list_iterator& operator=(const const_linked_object_list_iterator &x)
  {
    node_ = x.node_;
    list_ = x.list_;
    return *this;
  }

  ~const_linked_object_list_iterator() {}

  /**
   * @brief Compares this iterator with another on equality.
   * 
   * The equal operator compares this iterator
   * with a given one. The two iterators are equal
   * if the two nodes are the same.
   * 
   * @param i The iterator to compare with.
   * @return Return true if the iterators are equal.
   */
  bool operator==(const const_linked_object_list_iterator &i) const {
    return (node_ == i.node_);
  }

  /**
   * @brief Compares this iterator with another on unequality.
   * 
   * The not equal operator compares this iterator
   * with a given one. The two iterators are not equal
   * if the two nodes aren't the same.
   * 
   * @param i The iterator to compare with.
   * @return Return true if the iterators are not equal.
   */
  bool operator!=(const const_linked_object_list_iterator &i) const {
    return (node_ != i.node_);
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
    self tmp = *this;
    increment();
    return tmp;
  }

  /**
   * Pre decrements the iterator
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
    self tmp = *this;
    decrement();
    return tmp;
  }

  /**
   * Returns the pointer to the node.
   * 
   * @return The pointer to the node.
   */
  pointer operator->() const {
    pointer p = node_.get();
    return p;
  }

  /**
   * Returns the node.
   * 
   * @return The iterators underlaying node.
   */
  value_type operator*() const {
    return this->oref();
  }

  /**
   * Returns the node as object_ref.
   * 
   * @return The iterators underlaying node as object_ref.
   */
  object_ref<T> oref() const {
    return node_;
  }

  /**
   * Returns the node as object_ptr.
   * 
   * @return The iterators underlaying node as object_ptr.
   */
  object_ptr<T> optr() const {
    return node_;
  }

private:
  void increment() {
    if (node_ != list_->last_) {
      node_ = node_->next_;
    }
  }
  void decrement() {
    if (node_ != list_->last_) {
      node_ = node_->prev_;
    }
  }

private:
  value_type node_;
  const list_type *list_;
};

/// @endcond

/**
 * @class linked_object_list
 * @brief An linked object list class.
 * @tparam T The concrete object type.
 * 
 * The linked_object_list class stores object of
 * type T where T must be derived from linked_object_list_node.
 * This base class contains the previous and next links as
 * well as the first and last element of the linked list.
 * Therefor the order of the elements is persistent and
 * reliable.
 */
template < class S, class T >
class linked_object_list : public object_container
{
public:
//  typedef std::tr1::function<void (T*, &T::G)> set_ref_func_t;
//  typedef std::tr1::function<object_ref< (T*, &T::G)> get_ref_func_t;
  typedef linked_object_list<S, T> self;
  typedef T value_type;                                       /**< Shortcut for the wrapper class around the value type. */
  typedef S container_type;
  typedef linked_object_list_item<value_type, container_type> item_type;
  typedef object_ptr<item_type> item_ptr;
  typedef object_container::size_type size_type;                             /**< Shortcut for size type. */
  typedef linked_object_list_iterator<S, T> iterator;             /**< Shortcut for the list iterator. */
  typedef const_linked_object_list_iterator<S, T> const_iterator; /**< Shortcut for the list const iterator. */


  /**
   * @brief Creates an empty linked list.
   * 
   * A new linked_object_list is created. The list is part
   * of the given parent object and therefor a reference
   * to the parent object must be found inside the value
   * type object with the given list_ref_name.
   * 
   * @param parent The containing list object.
   * @param list_ref_name The name of the parent in the value type object.
   */
  linked_object_list(S *parent)
    : parent_(parent)
  {}

	virtual ~linked_object_list() {}

  /*
	virtual void read_from(object_atomizer *) {}
	virtual void write_to(object_atomizer *) const {}
  */

  /**
   * Return the begin iterator of the list.
   * 
   * @return The begin iterator.
   */
  iterator begin() {
    return ++iterator(first_, this);
  }

  /**
   * Return the begin iterator of the list.
   * 
   * @return The begin iterator.
   */
  const_iterator begin() const {
    return ++const_iterator(first_, this);
  }

  /**
   * Return the end iterator of the list.
   * 
   * @return The end iterator.
   */
  iterator end() {
    return iterator(last_, this);
  }

  /**
   * Return the end iterator of the list.
   * 
   * @return The end iterator.
   */
  const_iterator end() const {
    return const_iterator(last_, this);
  }

  /**
   * Returns wether the list is empty or not.
   * 
   * @return True if list is empty.
   */
  virtual bool empty() const {
    return first_->next_ == last_;
  }

  /**
   * @brief Clears the list.
   * 
   * All elements are removed from the list
   * and also removed from the object_store.
   */
  virtual void clear()
  {
    erase(begin(), end());
  }

  /**
   * Return the size of the list.
   * 
   * @return The size of the list.
   */
  virtual size_type size() const
  {
    return std::distance(begin(), end());
  }

  /**
   * @brief Push a new object to the front of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed front to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object and the links between the surrounding nodes
   * is done automatilcally.
   *
   * @param elem The element to be pushed front
   */
  virtual void insert(iterator pos, const value_type &elem)
  {
    if (!object_container::ostore()) {
      throw object_exception("invalid object_store pointer");
    } else {
      /*
      if (!elem->link_reference(base_list::parent_object(), base_list::list_name())) {
        // throw object_exception()
      } else {
        value_type_ptr optr = ostore()->insert(elem);
        optr->next_ = first_->next_;
        first_->next_ = optr;
        optr->prev_ = first_;
        first_->next_ = optr;
        optr->first_ = first_;
        optr->last_ = last_;
      }
      */
    }
  }


  /**
   * @brief Push a new object to the front of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed front to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object and the links between the surrounding nodes
   * is done automatilcally.
   *
   * @param elem The element to be pushed front
   */
  virtual void push_front(const value_type &elem)
  {
    insert(begin(), elem);
  }

  /**
   * @brief Push a new object to the end of the list.
   * 
   * An object not inserted into the object_store will
   * be pushed back to the list and inserted to the
   * object_store. Furthermore the reference link to the
   * list object and the links between the surrounding nodes
   * is done automatilcally.
   *
   * @param elem The element to be pushed back
   */
  virtual void push_back(const value_type &elem)
  {
    insert(end(), elem);
  }

  /**
   * @brief Erase the object at iterators position.
   * 
   * The object inside the iterator will first be
   * removed from the object_store and second be unlinked
   * and erased from the list.
   * The next iterator position is returned.
   *
   * @param i The object to be erased containing iterator.
   * @return The next iterator position
   */
  iterator erase(iterator i)
  {
    if (!ostore()) {
      // if list is not in ostore
      // throw exception
      //throw object_exception();
      return i;
    }
    // update predeccessor and successor
    value_type node = (i++).optr();
    node->prev()->next(node->next());
    node->next()->prev(node->prev());
    // delete node
    if (!ostore()->remove(node)) {
//      std::cout << "couldn't remove node (proxy: " << *node->proxy().get() << ")\n";
      node->prev()->next(node);
      node->next()->prev(node);
      // throw exception ?
      return ++iterator(node, this);
    }
    // return i's successor
    return i;
  }

  /**
   * @brief Erases a range defines by iterators
   *
   * Erase a range of elements from the list. The
   * range is defined by a first and a last iterator.
   * 
   * @param first The first iterator of the range to erase.
   * @param last The last iterator of the range to erase.
   * @return Returns the next iterator.
   */
  iterator erase(iterator first, iterator last)
  {
    while (first != last) {
      first = erase(first);
    }
    return first;
  }

protected:
  /**
   * @brief Executes the given function object for all elements.
   *
   * Executes the given function object for all elements.
   *
   * @param nf Function object used to be executed on each element.
   */
  virtual void for_each(const node_func &nf) const
  {
    value_type node = first_;
    while(node.get()) {
      nf(node.get());
      node = node->next();
    }
  }

  virtual void parent(object *p)
  {
    S *temp = dynamic_cast<S*>(p);
    if (!temp) {
      throw object_exception("couldn't cast object to concrete type");
    }
    parent_ = temp;
  }

private:
  virtual void append_proxy(object_proxy *) {};

  virtual void install(object_store *os)
  {
    object_container::install(os);
    
    // create first and last element
    first_ = ostore()->insert(new value_type(self_ref(this)));
    last_ = ostore()->insert(new value_type(self_ref(this)));
    // link object elements
    first_->first_ = first_;
    first_->last_ = last_;
    first_->next_ = last_;
    last_->first_ = first_;
    last_->last_ = last_;
    last_->prev_ = first_;
  }

  virtual void uninstall()
  {
    object_container::uninstall();
    first_.reset();
    last_.reset();
  }

  virtual void reset()
  {
    // link object elements
    first_->first_ = first_;
    first_->last_ = last_;
    first_->next_ = last_;
    last_->first_ = first_;
    last_->last_ = last_;
    last_->prev_ = first_;
  }

private:
  friend class object_store;
  friend class linked_object_list_iterator<S, T>;
  friend class const_linked_object_list_iterator<S, T>;

  S *parent_;

  item_ptr first_;
  item_ptr last_;
};

}

#endif /* LINKED_OBJECT_LIST_HPP */
