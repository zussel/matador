//
// Created by sascha on 1/15/16.
//

#ifndef OOS_HAS_MANY_VECTOR_HPP
#define OOS_HAS_MANY_VECTOR_HPP

#include "object/basic_has_many.hpp"
#include "object/object_store.hpp"

#include <vector>

namespace oos {

template < class T, template <class ...> class C >
class has_many_iterator<T, C, typename std::enable_if<is_same_container_type<C, std::vector>::value>::type> : public std::iterator<std::random_access_iterator_tag, T>
{
public:
  typedef has_many_iterator<T, C> self;
  typedef object_ptr<T> value_pointer;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> value_type;
  typedef object_ptr<item_type> item_ptr;
  typedef C<value_type, std::allocator<value_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;

public:
  has_many_iterator() {}
  explicit has_many_iterator(container_iterator iter)
    : iter_(iter)
  {}
  ~has_many_iterator() {}

  /**
   * @brief Compares this with another iterators.
   *
   * Compares this with another iterators. Returns true
   * if the iterators current container iterator and
   * others iterator are the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are the same.
   */
  bool operator==(const self &i) const
  {
    return (iter_ == i.iter_);
  }

  /**
   * @brief Compares this with another iterators.
   *
   * Compares this with another iterators. Returns true
   * if the iterators current containers iterator and
   * others iterator are not the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are not the same.
   */
  bool operator!=(const self &i) const
  {
    return !this->operator==(i);
  }

  friend difference_type operator-(self a, self b)
  {
    return a.iter_ - b.iter_;
  }

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
    self tmp = *this;
    --iter_;
    return tmp;
  }

  self operator--(int)
  {
    return self();
  }

  value_pointer operator->() const
  {
    return (*iter_)->value();
  }

  value_pointer operator*() const
  {
    return (*iter_)->value();
  }

  value_pointer get() const
  {
    return (*iter_)->value();
  }

  item_ptr relation_item() const
  {
    return *iter_;
  }

private:
  friend class has_many<T, C>;
  friend class basic_has_many<T, C>;

  container_iterator iter_;
};

template < class T, template <class ...> class C >
class const_has_many_iterator<T, C, typename std::enable_if<is_same_container_type<C, std::vector>::value>::type>
  : public std::iterator<std::random_access_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
{
public:
  typedef const_has_many_iterator<T, C> self;
  typedef object_ptr<T> value_pointer;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> value_type;
  typedef C<value_type, std::allocator<value_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename container_type::const_iterator const_container_iterator;
public:
  const_has_many_iterator() {}
  explicit const_has_many_iterator(container_iterator iter)
    : iter_(iter)
  {}
  explicit const_has_many_iterator(const_container_iterator iter)
    : iter_(iter)
  {}
  ~const_has_many_iterator() {}

  /**
   * @brief Compares this with another iterators.
   *
   * Compares this with another iterators. Returns true
   * if the iterators current container iterator and
   * others iterator are the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are the same.
   */
  bool operator==(const self &i) const
  {
    return (iter_ == i.iter_);
  }

  /**
   * @brief Compares this with another iterators.
   *
   * Compares this with another iterators. Returns true
   * if the iterators current containers iterator and
   * others iterator are not the same.
   *
   * @param i The iterator to compare with.
   * @return True if the iterators are not the same.
   */
  bool operator!=(const self &i) const
  {
    return !this->operator==(i);
  }

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
    self tmp = *this;
    --iter_;
    return tmp;
  }

  self operator--(int)
  {
    return self();
  }

  value_pointer operator->() const
  {
    return iter_->value();
  }

private:
//  template < class T, template <class ...> class C >
  friend class has_many<T, C>;
  friend class basic_has_many<T, C>;

  const_container_iterator iter_;
};

/**
 * has_many with std::vector
 */
template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<
  is_same_container_type<C, std::vector>::value &&
  !std::is_scalar<T>::value>::type
> : public basic_has_many<T, C>
{
public:

  typedef basic_has_many<T, C> base;
  typedef typename base::item_type item_type;
  typedef typename base::value_type value_type;
  typedef typename base::container_type container_type;
  typedef typename base::iterator iterator;
  typedef typename container_type::size_type size_type;

  has_many() {}

  iterator insert(iterator pos, const oos::object_ptr<T> &value)
  {
    // create new has_many
    if (indexed_) {

    }
    item_type *item = this->create_item(value);
    object_ptr<item_type> iptr(item);
    if (this->ostore_) {
      this->ostore_->insert(iptr);
//      ostore_->mark_modified()
    }
    return iterator(this->container_.insert(pos.iter_, iptr));
  }

  void push_back(const oos::object_ptr<T> &value)
  {
    insert(this->end(), value);
  }

private:
  item_type* create_item(const object_ptr<T> &value)
  {
    return new item_type(this->owner_field_, this->item_field_, this->owner_id_, value);
  }

private:
  bool indexed_ = false;
};

template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<
  is_same_container_type<C, std::vector>::value &&
  std::is_scalar<T>::value>::type
> : public basic_has_many<T, C>
{
public:

  typedef basic_has_many<T, C> base;
  typedef typename base::item_type item_type;
  typedef typename base::value_type value_type;
  typedef typename base::container_type container_type;
  typedef typename base::iterator iterator;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::iterator container_iterator;
//  typedef typename container_type::const_iterator const_iterator;

  has_many() {}

  iterator insert(iterator pos, T value)
  {
    // create new has_many
    if (indexed_) {

    }
    item_type *item = create_item(value);
    object_ptr<item_type> iptr(item);
    if (this->ostore_) {
      this->ostore_->insert(iptr);
//      ostore_->mark_modified()
    }
    return iterator(this->container_.insert(pos.iter_, iptr));
  }

  void push_back(T value)
  {
    insert(this->end(), value);
  }

private:
  item_type* create_item(T value)
  {
    return new item_type(this->owner_field_, this->item_field_, this->owner_id_, value);
  }

private:
  bool indexed_ = false;
};

}
#endif //OOS_HAS_MANY_VECTOR_HPP
