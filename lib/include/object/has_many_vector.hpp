//
// Created by sascha on 1/15/16.
//

#ifndef OOS_HAS_MANY_VECTOR_HPP
#define OOS_HAS_MANY_VECTOR_HPP

#include "object/basic_has_many.hpp"
#include "object/object_store.hpp"

#include <vector>

namespace oos {

template < class T >
struct has_many_iterator_traits<T, std::vector, typename std::enable_if<!std::is_scalar<T>::value>::type>
  : public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef object_ptr<T> value_type;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> internal_type;
  typedef object_ptr<item_type> relation_type;
  typedef std::vector<internal_type, std::allocator<internal_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
};

template < class T >
struct has_many_iterator_traits<T, std::vector, typename std::enable_if<std::is_scalar<T>::value>::type>
  : public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef T value_type;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> internal_type;
  typedef object_ptr<item_type> relation_type;
  typedef std::vector<internal_type, std::allocator<internal_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
};

template < class T >
class has_many_iterator<T, std::vector>
  : public has_many_iterator_traits<T, std::vector>
{
public:
  typedef has_many_iterator_traits<T, std::vector> traits;
  typedef has_many_iterator<T, std::vector> self;
  typedef typename traits::value_type value_type;
  typedef typename traits::item_type item_type;
  typedef typename traits::internal_type internal_type;
  typedef typename traits::relation_type relation_type;
  typedef typename traits::container_type container_type;
  typedef typename traits::container_iterator container_iterator;
  typedef typename traits::difference_type difference_type;

public:
  has_many_iterator() {}
  has_many_iterator(const self &iter) : iter_(iter.iter_) {}
  explicit has_many_iterator(container_iterator iter) : iter_(iter) {}
  has_many_iterator& operator=(const self &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }
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
    self tmp = *this;
    --iter_;
    return tmp;
  }

  self& operator+=(difference_type offset)
  {
    iter_ += offset;
    return *this;
  }

  self& operator-=(difference_type offset)
  {
    iter_ -= offset;
    return *this;

  }

  self operator+(difference_type offset)
  {
    self tmp = *this;
    return tmp += offset;
  }

  self operator-(difference_type offset)
  {
    self tmp = *this;
    return tmp -= offset;
  }

  friend const self operator+(difference_type offset, self out)
  {
    out.iter_ += offset;
    return out;
  }

  value_type operator->() const { return (*iter_)->value(); }
  value_type operator*() const { return (*iter_)->value(); }
  value_type get() const { return (*iter_)->value(); }
  relation_type relation_item() const { return *iter_; }

private:
  friend class has_many<T, std::vector>;
  friend class const_has_many_iterator<T, std::vector>;
  friend class basic_has_many<T, std::vector>;

  container_iterator iter_;
};

template < class T >
struct const_has_many_iterator_traits<T, std::vector, typename std::enable_if<!std::is_scalar<T>::value>::type>
  : public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef object_ptr<T> value_type;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> internal_type;
  typedef object_ptr<item_type> relation_type;
  typedef std::vector<internal_type, std::allocator<internal_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename container_type::const_iterator const_container_iterator;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
};

template < class T >
struct const_has_many_iterator_traits<T, std::vector, typename std::enable_if<std::is_scalar<T>::value>::type>
  : public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef T value_type;
  typedef has_many_item<T> item_type;
  typedef has_one<item_type> internal_type;
  typedef object_ptr<item_type> relation_type;
  typedef std::vector<internal_type, std::allocator<internal_type>> container_type;
  typedef typename container_type::iterator container_iterator;
  typedef typename container_type::const_iterator const_container_iterator;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
};

template < class T >
class const_has_many_iterator<T, std::vector>
  : public const_has_many_iterator_traits<T, std::list>
{
public:
  typedef const_has_many_iterator_traits<T, std::vector> traits;
  typedef const_has_many_iterator<T, std::vector> self;
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
  const_has_many_iterator(const has_many_iterator<T, std::vector> &iter) : iter_(iter.iter_) {}
  const_has_many_iterator& operator=(const self &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }
  const_has_many_iterator& operator=(const has_many_iterator<T, std::vector> &iter)
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

  friend const self operator+(difference_type offset, self out)
  {
    out.iter_ += offset;
    return out;
  }

  friend const self operator+(self out, difference_type offset)
  {
    out.iter_ += offset;
    return out;
  }

  const value_type operator->() const { return get(); }
  const value_type operator*() const { return get(); }
  const value_type get() const { return (*iter_)->value(); }
  const relation_type relation_item() const { return *iter_; }

private:
  friend class has_many<T, std::vector>;
  friend class basic_has_many<T, std::vector>;

  const_container_iterator iter_;
};

/**
 * has_many with std::vector
 */
template < class T >
class has_many<T, std::vector> : public basic_has_many<T, std::vector>
{
public:
  typedef basic_has_many<T, std::vector> base;
  typedef typename base::iterator iterator;
  typedef typename base::value_type value_type;
  typedef typename base::item_type item_type;
  typedef typename base::relation_type relation_type;
  typedef typename base::container_iterator container_iterator;

  has_many() {}

  iterator insert(iterator pos, const value_type &value)
  {
    // create new has_many
    if (indexed_) {

    }
    item_type *item = create_item(value);
    relation_type iptr(item);
    if (this->ostore_) {
      this->ostore_->insert(iptr);
//      ostore_->mark_modified()
    }
    return iterator(this->container_.insert(pos.iter_, iptr));
  }

  void push_back(const value_type &value)
  {
    insert(this->end(), value);
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

private:
  bool indexed_ = false;
};

}
#endif //OOS_HAS_MANY_VECTOR_HPP
