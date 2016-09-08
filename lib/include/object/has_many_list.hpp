//
// Created by sascha on 1/15/16.
//

#ifndef OOS_TEST_HAS_MANY_LIST_HPP
#define OOS_TEST_HAS_MANY_LIST_HPP

#include "object/basic_has_many.hpp"
#include "object/object_store.hpp"

#include <list>

namespace oos {

/// @cond OOS_DEV

template < class T >
struct has_many_iterator_traits<T, std::list, typename std::enable_if<!std::is_scalar<T>::value>::type>
  : public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef object_ptr<T> value_type;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> internal_type;
  typedef object_ptr<item_type> relation_type;
  typedef std::list<internal_type, std::allocator<internal_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
};

template < class T >
struct has_many_iterator_traits<T, std::list, typename std::enable_if<std::is_scalar<T>::value>::type>
  : public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef T value_type;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> internal_type;
  typedef object_ptr<item_type> relation_type;
  typedef std::list<internal_type, std::allocator<internal_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
};

/// @endcond

template < class T >
class has_many_iterator<T, std::list>
  : public has_many_iterator_traits<T, std::list>
{
public:
  typedef has_many_iterator_traits<T, std::list> traits;
  typedef has_many_iterator<T, std::list> self;
  typedef typename traits::value_type value_type;
  typedef typename traits::item_type item_type;
  typedef typename traits::internal_type internal_type;
  typedef typename traits::relation_type relation_type;
  typedef typename traits::container_type container_type;
  typedef typename traits::container_iterator container_iterator;
  typedef typename traits::difference_type difference_type;

public:
  has_many_iterator() {}
  explicit has_many_iterator(container_iterator iter)
    : iter_(iter)
  {}
  ~has_many_iterator() {}

  bool operator==(const self &i) const { return (iter_ == i.iter_); }
  bool operator!=(const self &i) const { return !this->operator==(i); }

  friend difference_type operator-(self a, self b) { return a.iter_ - b.iter_; }

  self& operator++()
  {
    ++iter_;
    return *this;
  }

  self operator++(int)
  {
    self tmp = *this;
    ++iter_;
    return tmp;
  }

  self& operator--()
  {
    --iter_;
    return *this;
  }

  self operator--(int)
  {
    --iter_;
    return self();
  }

  value_type operator->() const { return get(); }
  value_type operator*() const { return get(); }
  value_type get() const { return (*iter_)->value(); }
  relation_type relation_item() const { return *iter_; }

private:
  friend class has_many<T, std::list>;
  friend class const_has_many_iterator<T, std::list>;
  friend class basic_has_many<T, std::list>;

  container_iterator iter_;
};

/// @cond OOS_DEV

template < class T >
struct const_has_many_iterator_traits<T, std::list, typename std::enable_if<!std::is_scalar<T>::value>::type>
  : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef object_ptr<T> value_type;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> internal_type;
  typedef object_ptr<item_type> relation_type;
  typedef std::list<internal_type, std::allocator<internal_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
};

template < class T >
struct const_has_many_iterator_traits<T, std::list, typename std::enable_if<std::is_scalar<T>::value>::type>
  : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef T value_type;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> internal_type;
  typedef object_ptr<item_type> relation_type;
  typedef std::list<internal_type, std::allocator<internal_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename container_type::const_iterator const_container_iterator;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
};

/// @endcond

template < class T >
class const_has_many_iterator<T, std::list>
  : public const_has_many_iterator_traits<T, std::list>
{
public:
  typedef const_has_many_iterator_traits<T, std::list> traits;
  typedef const_has_many_iterator<T, std::list> self;
  typedef typename traits::value_type value_type;
  typedef typename traits::item_type item_type;
  typedef typename traits::internal_type internal_type;
  typedef typename traits::relation_type relation_type;
  typedef typename traits::difference_type difference_type;
  typedef typename traits::container_type container_type;
  typedef typename traits::container_iterator container_iterator;
  typedef typename traits::const_container_iterator const_container_iterator;

public:
  const_has_many_iterator() {}
  explicit const_has_many_iterator(container_iterator iter) : iter_(iter) {}
  explicit const_has_many_iterator(const_container_iterator iter) : iter_(iter) {}
  const_has_many_iterator(const has_many_iterator<T, std::list> &iter) : iter_(iter.iter_) {}
  const_has_many_iterator& operator=(const self &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }
  const_has_many_iterator& operator=(const has_many_iterator<T, std::list> &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }
  ~const_has_many_iterator() {}

  bool operator==(const self &i) const { return (iter_ == i.iter_); }
  bool operator!=(const self &i) const { return !this->operator==(i); }

  self& operator++()
  {
    ++iter_;
    return *this;
  }

  self operator++(int)
  {
    self tmp = *this;
    ++iter_;
    return tmp;
  }

  self& operator--()
  {
    --iter_;
    return *this;
  }

  self operator--(int)
  {
    self tmp = *this;
    --iter_;
    return tmp;
  }

  const value_type operator->() const { return get(); }
  const value_type operator*() const { return get(); }
  const value_type get() const { return (*iter_)->value(); }
  const relation_type relation_item() const { return *iter_; }

private:
  friend class has_many<T, std::list>;
  friend class basic_has_many<T, std::list>;

  const_container_iterator iter_;
};

/**
 * @brief Has many relation class using a std::list as container
 *
 * The has many relation class uses a std::list as internal
 * container to store the objects.
 *
 * It provides all main interface functions std::list provides
 * - insert element at a iterator position
 * - push back an element
 * - push front an element
 * - erase an element at iterator position
 * - erase a range of elements within first and last iterator position
 * - clear the container
 *
 * All of these methods are wrappes around the std::list methods plus
 * the modification in the corresponding object_store and notification
 * of the transaction observer
 *
 * The relation holds object_ptr elements as well as scalar data elements.
 *
 * @tparam T The type of the elements
 */
template < class T >
class has_many<T, std::list> : public basic_has_many<T, std::list>
{
public:

  typedef basic_has_many<T, std::list> base;                    /**< Shortcut to self */
  typedef typename base::iterator iterator;                     /**< Shortcut to iterator type */
  typedef typename base::value_type value_type;                 /**< Shortcut to value_type */
  typedef typename base::item_type item_type;                   /**< Shortcut to item_type */
  typedef typename base::relation_type relation_type;           /**< Shortcut to relation_type */
  typedef typename base::container_iterator container_iterator; /**< Shortcut to container_iterator */

  /**
   * brief Creates an empty has_many object
   */
  explicit has_many() {}

  /**
   * @brief Inserts an element at the given position.
   *
   * @param pos The position to insert at
   * @param value The element to be inserted
   * @return The iterator at position of inserted element
   */
  iterator insert(iterator pos, const value_type &value)
  {
    // create new has_many
    item_type *item = this->create_item(value);
    relation_type iptr(item);
    if (this->ostore_) {
      this->ostore_->insert(iptr);
      this->mark_modified_owner_(*this->ostore_, this->owner_);
    }
    container_iterator i = pos.iter_;
    return iterator(this->container_.insert(i, iptr));
  }

  /**
   * @brief Inserts an element at first position.
   *
   * @param value The element to be inserted
   */
  void push_front(const value_type &value)
  {
    insert(this->begin(), value);
  }

  /**
   * @brief Inserts an element at last position.
   *
   * @param value The element to be inserted
   */
  void push_back(const value_type &value)
  {
    insert(this->end(), value);
  }

  /**
   * Clears the list
   */
  void clear()
  {
    erase(this->begin(), this->end());
  }

  iterator erase(iterator i)
  {
    if (this->ostore_) {
      relation_type iptr = i.relation_item();
      this->ostore_->remove(iptr);
    }
    container_iterator ci = this->container_.erase(i.iter_);
    return iterator(ci);
  }

  iterator erase(iterator start, iterator end)
  {
    iterator i = start;
    if (this->ostore_) {
      while (i != end) {
        typename base::relation_type iptr = (i++).relation_item();
        this->ostore_->remove(iptr);
      }
    }
    return iterator(this->container_.erase(start.iter_, end.iter_));
  }

private:
  item_type* create_item(const value_type &value)
  {
    return new item_type(this->owner_field_, this->item_field_, this->owner_id_, value);
  }
};

}
#endif //OOS_HAS_MANY_LIST_HPP
