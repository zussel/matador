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

template < class T > class linked_object_list;
template < class T > class linked_object_list_iterator;
template < class T > class const_linked_object_list_iterator;

/**
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

/**
 * @class linked_object_list_node
 * @brief List node class for linked list
 * @tparam T List node type.
 * 
 * This class represents the list node class
 * for the linked list. It contains already
 * fields for next and preivious nodes as well
 * as fields for the first and last list node.
 */
template < class T >
class linked_object_list_node : public linked_object_list_base_node
{
public:
  linked_object_list_node() {}
  virtual ~linked_object_list_node() {}

  /**
   * @brief Reads the data from the object_atomizer
   * 
   * All data next, previous, first and last node
   * is read in from the given object_atomizer.
   * 
   * @param reader The object_atomizer object to read from.
   */
	void read_from(object_atomizer *reader)
  {
    object::read_from(reader);
    reader->read_object("first", first_);
    reader->read_object("last", last_);
    reader->read_object("prev", prev_);
    reader->read_object("next", next_);
  }

  /**
   * @brief Writes the data to the object_atomizer
   * 
   * All data next, previous, first and last node
   * is written to the given object_atomizer.
   * 
   * @param writer The object_atomizer object to write to.
   */
	void write_to(object_atomizer *writer) const
  {
    object::write_to(writer);
    writer->write_object("first", first_);
    writer->write_object("last", last_);
    writer->write_object("prev", prev_);
    writer->write_object("next", next_);
  }

  /**
   * Returns nodes next node as linked_object_list_base_node.
   * 
   * @return Nodes next node.
   */
  virtual linked_object_list_base_node *next_node() const
  {
    return next_.get();
  }

  /**
   * Returns nodes previous node as linked_object_list_base_node.
   * 
   * @return Nodes previous node.
   */
  virtual linked_object_list_base_node *prev_node() const
  {
    return prev_.get();
  }

  /**
   * Returns lists first node.
   * 
   * @return Lists first node.
   */
  object_ref<T> first() const
  {
    return first_;
  }

  /**
   * Returns lists last node.
   * 
   * @return Lists last node.
   */
  object_ref<T> last() const
  {
    return last_;
  }

  /**
   * Returns nodes previous node.
   * 
   * @return Nodes previous node.
   */
  object_ref<T> prev() const
  {
    return prev_;
  }

  /**
   * Returns nodes next node.
   * 
   * @return Nodes next node.
   */
  object_ref<T> next() const
  {
    return next_;
  }

protected:
  /**
   * Sets lists first node.
   * 
   * @param f Lists first node.
   */
  void first(const object_ref<T> &f)
  {
    mark_modified();
    first_ = f;
  }

  /**
   * Sets lists last node.
   * 
   * @param l Lists last node.
   */
  void last(const object_ref<T> &l)
  {
    mark_modified();
    last_ = l;
  }

  /**
   * Sets nodes previous node.
   * 
   * @param p Nodes previous node.
   */
  void prev(const object_ref<T> &p)
  {
    mark_modified();
    prev_ = p;
  }

  /**
   * Sets nodes next node.
   * 
   * @param n Nodes next node.
   */
  void next(const object_ref<T> &n)
  {
    mark_modified();
    next_ = n;
  }

private:
  friend class linked_object_list<T>;
  friend class linked_object_list_iterator<T>;
  friend class const_linked_object_list_iterator<T>;

  object_ref<T> first_;
  object_ref<T> last_;
  object_ref<T> prev_;
  object_ref<T> next_;
};

/**
 * @class linked_object_ptr_list_node
 * @brief Node type for linked lists with an object_ptr as value.
 * @tparam T The object type of the object_ptr class.
 * 
 * This node class is specialization for the linked_object_list_node class
 * for linked object lists. It contains only an object_ptr and wraps
 * it with direct accessors.
 */
template < class T >
class linked_object_ptr_list_node : public linked_object_list_node<linked_object_ptr_list_node<T> >
{
public:
  typedef linked_object_list_node<linked_object_ptr_list_node<T> > base_node; /**< Shortcut to the base node */
  typedef T value_type; /**< Shortcut for the nodes value type. */

  /**
   * @brief Creates a new linked_object_ptr_list_node
   *
   * Creates a new linked list node for object_ptr
   * the default parameter name for the object is
   * "object".
   */
  linked_object_ptr_list_node() : name_("object") {}

  /**
   * @brief Creates a new linked_object_ptr_list_node
   *
   * Creates a new linked list node for object_ptr
   * the parameter name for the object must be given.
   * 
   * @param name Name of the parameter for object_atomizer.
   */
  linked_object_ptr_list_node(const std::string &name)
    : name_(name)
  {}

  /**
   * @brief Creates a new linked_object_ptr_list_node
   *
   * Creates a new linked list node for object_ptr
   * the parameter name for the object must be given.
   * Also the value object_ptr will be set.
   * 
   * @param optr The object value for this node.
   * @param name Name of the parameter for object_atomizer.
   */
  linked_object_ptr_list_node(object_ptr<T> optr, const std::string &name)
    : object_(optr)
    , name_(name)
  {}

  virtual ~linked_object_ptr_list_node() {}

  /**
   * @brief Reads the data from the object_atomizer.
   * 
   * In addition to the base fields, the object_ptr is read
   * from the object_atomizer.
   * 
   * @param reader The object_reader to read from.
   */
	void read_from(object_atomizer *reader)
  {
    base_node::read_from(reader);
    reader->read_object(name_.c_str(), object_);
  }

  /**
   * @brief Writes the data to the object_atomizer.
   * 
   * In addition to the base fields, the object_ptr is written
   * to the object_atomizer.
   * 
   * @param writer The object_reader to write to.
   */
	void write_to(object_atomizer *writer) const
  {
    base_node::write_to(writer);
    writer->write_string(name_.c_str(), object_);
  }

  /**
   * Returns the object value.
   * 
   * @return The object_ptr value.
   */
  object_ptr<T> optr() const {
    return object_;
  }

  /**
   * Link the reference node
   * 
   * @param r The reference object.
   * @param n The name of the reference object parameter.
   */
  virtual bool link_reference(object *r, const std::string &n)
  {
    return base_node::link_reference(object_.ptr(), r, n);
  }

private:
  object_ptr<T> object_;
  std::string name_;
};

/**
 * @class linked_object_ref_list_node
 * @brief Node type for linked lists with an object_ref as value.
 * @tparam T The object type of the object_ref class.
 * 
 * This node class is specialization for the linked_object_list_node class
 * for linked object lists. It contains only an object_ref and wraps
 * it with direct accessors.
 */
template < class T >
class linked_object_ref_list_node : public linked_object_list_node<linked_object_ref_list_node<T> >
{
public:
  typedef linked_object_list_node<linked_object_ref_list_node<T> > base_node; /**< Shortcut to the base node */
  typedef T value_type; /**< Shortcut for the nodes value type. */

  /**
   * @brief Creates a new linked_object_ref_list_node
   *
   * Creates a new linked list node for object_ref
   * the default parameter name for the object is
   * "object".
   */
  linked_object_ref_list_node() : name_("object") {}

  /**
   * @brief Creates a new linked_object_ref_list_node
   *
   * Creates a new linked list node for object_ref
   * the parameter name for the object must be given.
   * 
   * @param name Name of the parameter for object_atomizer.
   */
  linked_object_ref_list_node(const std::string &name)
    : name_(name)
  {}

  /**
   * @brief Creates a new linked_object_ref_list_node
   *
   * Creates a new linked list node for object_ref
   * the parameter name for the object must be given.
   * Also the value object_ref will be set.
   * 
   * @param oref The object_ref value for this node.
   * @param name Name of the parameter for object_atomizer.
   */
  linked_object_ref_list_node(object_ref<T> oref, const std::string &name)
    : object_(oref)
    , name_(name)
  {}

  virtual ~linked_object_ref_list_node() {}

  /**
   * @brief Reads the data from the object_atomizer.
   * 
   * In addition to the base fields, the object_ref is read
   * from the object_atomizer.
   * 
   * @param reader The object_reader to read from.
   */
	void read_from(object_atomizer *reader)
  {
    linked_object_list_node<linked_object_ref_list_node<T> >::read_from(reader);
    reader->read_object(name_.c_str(), object_);
  }

  /**
   * @brief Writes the data to the object_atomizer.
   * 
   * In addition to the base fields, the object_ref is written
   * to the object_atomizer.
   * 
   * @param writer The object_reader to write to.
   */
	void write_to(object_atomizer *writer) const
  {
    linked_object_list_node<linked_object_ref_list_node<T> >::write_to(writer);
    writer->write_object(name_.c_str(), object_);
  }

  /**
   * Returns the object value.
   * 
   * @return The object_ref value.
   */
  object_ref<T> oref() const {
    return object_;
  }

  /**
   * Link the reference node
   * 
   * @param r The reference object.
   * @param n The name of the reference object parameter.
   */
  virtual bool link_reference(object *r, const std::string &n)
  {
    return base_node::link_reference(object_.ptr(), r, n);
  }

private:
  object_ref<T> object_;
  std::string name_;
};

/**
 * @class linked_object_list_iterator
 * @brief Iterator class for all linked lists.
 * @tparam T Containing list node type of the iterator.
 * 
 * This is the iterator class used by all linked
 * list types.
 */
template < class T >
class linked_object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
public:
  typedef linked_object_list_iterator<T> self;	   /**< Shortcut for this iterator type. */
  typedef T* pointer;                      /**< Shortcut for the pointer type. */
  typedef object_ptr<T> value_type;        /**< Shortcut for the value type. */
  typedef value_type& reference ;          /**< Shortcut for the reference to the value type. */
  typedef linked_object_list<T> list_type; /**< Shortcut for the list type. */

  /**
   * Creates an empty iterator
   */
  linked_object_list_iterator()
    : node_(NULL)
    , list_(NULL)
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
  friend class const_linked_object_list_iterator<T>;

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
template < class T >
class const_linked_object_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&> {
public:
  typedef const_linked_object_list_iterator<T> self;	/**< Shortcut for this iterator type. */
  typedef T* pointer;                         /**< Shortcut for the pointer type. */
  typedef object_ptr<T> value_type;           /**< Shortcut for the value type. */
  typedef value_type& reference;              /**< Shortcut for the reference to the value type. */
  typedef linked_object_list<T> list_type;    /**< Shortcut for the list type. */

  /**
   * Creates an empty const_iterator
   */
  const_linked_object_list_iterator()
    : node_(NULL)
    , list_(NULL)
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
  const_linked_object_list_iterator(const linked_object_list_iterator<T> &x)
    : node_(x.node_)
    , list_(x.list_)
  {}

  /**
   * Assign an iterator to this iterator.
   * 
   * @param x The iterator to assign from.
   */
  const_linked_object_list_iterator& operator=(const linked_object_list_iterator<T> &x)
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
template < class T >
class linked_object_list : public object_list_base
{
public:
//  typedef std::tr1::function<void (T*, &T::G)> set_ref_func_t;
//  typedef std::tr1::function<object_ref< (T*, &T::G)> get_ref_func_t;
  typedef object_list_base base_list;                   /**< Shortcut for the object_list class. */
	typedef T value_type;                                 /**< Shortcut for the value type. */
	typedef object_ptr<value_type> value_type_ptr;        /**< Shortcut for the value type pointer. */
  typedef linked_object_list_iterator<T> iterator;             /**< Shortcut for the list iterator. */
  typedef const_linked_object_list_iterator<T> const_iterator; /**< Shortcut for the list const iterator. */


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
  linked_object_list(object *parent, const std::string &list_ref_name)
    : object_list_base(parent, list_ref_name)
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
    return first_->next_ == last_->prev_;
  }

  /**
   * @brief Clears the list.
   * 
   * All elements are removed from the list
   * and also removed from the object_store.
   */
  virtual void clear()
  {
    base_list::clear();
    erase(begin(), end());
  }

  /**
   * Return the size of the list.
   * 
   * @return The size of the list.
   */
  virtual size_t size() const
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
  virtual void push_front(value_type *elem)
  {
    if (!ostore()) {
      //throw object_exception();
    } else {
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
    }
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
  virtual void push_back(value_type *elem)
  {
    if (!ostore()) {
      //throw object_exception();
    } else {
      if (!elem->link_reference(base_list::parent_object(), base_list::list_name())) {
        // throw object_exception()
      } else {
        value_type_ptr optr = ostore()->insert(elem);
        optr->prev_ = last_->prev_;
        last_->prev_->next_ = optr;
        optr->next_ = last_;
        last_->prev_ = optr;
        optr->first_ = first_;
        optr->last_ = last_;
      }
    }
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
    value_type_ptr node = (i++).optr();
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
    value_type_ptr node = first_;
    while(node.get()) {
      nf(node.get());
      node = node->next();
    }
  }

private:
  virtual void push_front_proxy(object_proxy *) {};
  virtual void push_back_proxy(object_proxy *) {};

  virtual void install(object_store *os)
  {
    object_list_base::install(os);
    
    // create first and last element
    first_ = ostore()->insert(new value_type(list_name()));
    last_ = ostore()->insert(new value_type(list_name()));
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
    object_list_base::uninstall();
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
  friend class linked_object_list_iterator<T>;
  friend class const_linked_object_list_iterator<T>;

  value_type_ptr first_;
  value_type_ptr last_;
};

/**
 * @class linked_object_ptr_list
 * @brief An linked object list class for object_ptr.
 * @tparam T The concrete object type.
 * 
 * The linked_object_ptr_list class stores object of
 * type T. T is the only single element in teach node and
 * is stored in an object_ptr.
 * This base class contains the previous and next links as
 * well as the first and last element of the linked list.
 * Therefor the order of the elements is persistent and
 * reliable.
 */
template < class T >
class linked_object_ptr_list : public linked_object_list<linked_object_ptr_list_node<T> >
{
public:
  typedef linked_object_ptr_list_node<T> value_type;       /**< Shortcut for the value type */
  typedef object_ptr<value_type> value_type_ptr;    /**< Shortcut for the value type pointer */
  typedef linked_object_list<value_type> base_list; /**< Shortcut for the base list class */

  /**
   * @brief Creates an empty linked object ptr list.
   *
   * Creates an empty linked object ptr list. The list
   * is part of the given parent object and each element
   * in this has a reference to the parent within a parameter
   * of the given name.
   *
   * @param parent The list containing object.
   * @param list_ref_name The name of the reference list object in each node.
   */
  linked_object_ptr_list(object *parent, const std::string &list_ref_name)
    : linked_object_list<value_type>(parent, list_ref_name)
  {}

  virtual ~linked_object_ptr_list() {}

  /**
   * @brief Insert an element at the beginning of the list.
   *
   * Insert an element at the beginning of the list. The
   * reference link the object containing list is done
   * automatically.
   *
   * @param optr The pointer object to be pushed front.
   */
  void push_front(const object_ptr<T> &optr)
  {
    base_list::push_front(new value_type(optr, base_list::list_name()));
  }

  /**
   * @brief Insert an element at the end of the list.
   *
   * Insert an element at the end of the list. The
   * reference link to the object containing list is done
   * automatically.
   *
   * @param optr The pointer object to be pushed back.
   */
  void push_back(const object_ptr<T> &optr)
  {
    base_list::push_front(new value_type(optr, base_list::list_name()));
  }
};

/**
 * @class linked_object_ref_list
 * @brief An linked object list class for object_ref.
 * @tparam T The concrete object type.
 * 
 * The linked_object_ref_list class stores object of
 * type T. T is the only single element in teach node and
 * is stored in an object_ref.
 * This base class contains the previous and next links as
 * well as the first and last element of the linked list.
 * Therefor the order of the elements is persistent and
 * reliable.
 */
template < class T >
class linked_object_ref_list : public linked_object_list<linked_object_ref_list_node<T> >
{
public:
  typedef linked_object_ref_list_node<T> value_type;       /**< Shortcut for the value type */
  typedef object_ptr<value_type> value_type_ptr;    /**< Shortcut for the value type pointer */
  typedef linked_object_list<value_type> base_list; /**< Shortcut for the base list class */

  /**
   * @brief Creates an empty linked object ref list.
   *
   * Creates an empty linked object ref list. The list
   * is part of the given parent object and each element
   * in this has a reference to the parent within a parameter
   * of the given name.
   *
   * @param parent The list containing object.
   * @param list_ref_name The name of the reference list object in each node.
   */
  linked_object_ref_list(object *parent, const std::string &list_ref_name)
    : linked_object_list<value_type>(parent, list_ref_name)
  {}

  virtual ~linked_object_ref_list() {}

  /**
   * @brief Insert an element at the beginning of the list.
   *
   * Insert an element at the beginning of the list. The
   * reference link to the object containing list is done
   * automatically.
   *
   * @param oref The reference object to be pushed front.
   */
  void push_front(const object_ref<T> &oref)
  {
    base_list::push_front(new value_type(oref, base_list::list_name()));
  }

  /**
   * @brief Insert an element at the end of the list.
   *
   * Insert an element at the end of the list. The
   * reference link to the object containing list is done
   * automatically.
   *
   * @param oref The reference object to be pushed back.
   */
  void push_back(const object_ref<T> &oref)
  {
    base_list::push_back(new value_type(oref, base_list::list_name()));
  }
};

}

#endif /* LINKED_OBJECT_LIST_HPP */
