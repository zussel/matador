#ifndef MATADOR_CONTAINER_ITERATOR_HPP
#define MATADOR_CONTAINER_ITERATOR_HPP

#include "matador/object/container_iterator_traits.hpp"

namespace matador {

namespace detail {
class object_inserter;
class object_deleter;
}

class object_serializer;

template < class Type, template < class ... > class ContainerType >
class basic_container;

template < class Type, template < class ... > class ContainerType >
class container;

template < class Type, template < class ... > class ContainerType >
class const_container_iterator;

/**
 * @brief Represents a has many iterator
 *
 * Represents a has many iterator for a has many
 * relationship with std::vector container
 *
 * @tparam Type The type of the iterator/container
 */
template < class Type, template < class ... > class ContainerType >
class container_iterator
{
private:
  using traits = container_iterator_traits<Type, ContainerType>;
  using container_holder_type = typename traits::holder_type;

public:
  typedef container_iterator<Type, ContainerType> self;                             /**< Shortcut value self */
  typedef typename traits::value_type value_type;                             /**< Shortcut value type */
  typedef typename traits::holder_type holder_type;                           /**< Shortcut to the relation type */
  typedef typename traits::difference_type difference_type;                   /**< Shortcut to the difference type */
  typedef typename traits::holder_container_iterator holder_container_iterator;             /**< Shortcut to the internal container iterator */
  typedef typename traits::holder_const_container_iterator holder_const_container_iterator; /**< Shortcut to the internal const container iterator */

public:
  /**
   * @brief Creates an empty has many iterator
   */
  container_iterator() = default;

  /**
   * @brief Copy constructs an iterator from another iterator
   *
   * @param iter The iterator to copy from
   */
  container_iterator(const self &iter) : iter_(iter.iter_) {}

  /**
   * @brief Creates a has many iterator from given internal container iterator
   *
   * @param iter The iterator to create the has many iterator from
   */
  explicit container_iterator(holder_container_iterator iter) : iter_(iter) {}

  /**
   * @brief Copy assign an iterator from another iterator
   *
   * @param iter The iterator to copy from
   * @return A reference to self
   */
  container_iterator& operator=(const self &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }
  ~container_iterator() = default;

  /**
   * @brief Compares equality iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator contain
   * the same element true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators contain the same element
   */
  bool operator==(const self &i) const { return (iter_ == i.iter_); }

  /**
   * @brief Compares inequality iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator doesn't
   * contain the same element true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators doesn't contain the same element
   */
  bool operator!=(const self &i) const { return !this->operator==(i); }

  /**
   * @brief Compares less than iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator isn't
   * less than this iterator true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators isn't less than this iterator
   */
  bool operator<(const self &i) const { return iter_ < i.iter_; }

  /**
   * @brief Returns the difference of two iterators a and b.
   *
   * @param a The minuend iterator
   * @param b The subtrahend iterator
   * @return The difference between both iterators
   */
  friend difference_type operator-(self a, self b) { return a.iter_ - b.iter_; }

  /**
   * @brief Pre increments self
   *
   * @return A reference to incremented self
   */
  self& operator++()
  {
    ++iter_;
    return *this;
  }

  /**
   * @brief Post increments iterator
   *
   * Post increments iterator and returns a
   * new iterator object.
   *
   * @return Returns new incremented iterator
   */
  self operator++(int)
  {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  /**
   * @brief Pre decrements self
   *
   * @return A reference to decremented self
   */
  self& operator--()
  {
    --iter_;
    return *this;
  }

  /**
   * @brief Post decrements iterator
   *
   * Post decrements iterator and returns a
   * new iterator object.
   *
   * @return Returns new decremented iterator
   */
  self operator--(int)
  {
    --iter_;
    return self();
  }

  /**
   * @brief Increments iterator with offset
   *
   * @param offset The offset to be incremented
   * @return Reference to incremented iterator
   */
  self& operator+=(difference_type offset)
  {
    iter_ += offset;
    return *this;
  }

  /**
   * @brief Decrements iterator with offset
   *
   * @param offset The offset to be decremented
   * @return Reference to decremented iterator
   */
  self& operator-=(difference_type offset)
  {
    iter_ -= offset;
    return *this;

  }

  /**
   * @brief Increments iterator with offset
   *
   * @param offset The offset to be incremented
   * @return New incremented iterator
   */
  self operator+(difference_type offset)
  {
    self tmp = *this;
    return tmp += offset;
  }

  /**
   * @brief Decrements iterator with offset
   *
   * @param offset The offset to be decremented
   * @return New decremented iterator
   */
  self operator-(difference_type offset)
  {
    self tmp = *this;
    return tmp -= offset;
  }

  /**
   * Increment given iterator with the given
   * offset.
   *
   * @param offset Increment offset value
   * @param out Iterator to increment
   * @return Incremented iterator
   */
  friend const self operator+(difference_type offset, self out)
  {
    out.iter_ += offset;
    return out;
  }

  /**
   * Return the current value
   * represented by the iterator
   *
   * @return The current value
   */
  value_type operator->() const
  {
    value_type v = this->iter_->value();
    return v;
  }

  /**
   * Return the current value
   * represented by the iterator
   *
   * @return The current value
   */
  value_type& operator*() const
  {
    return this->iter_->value();
  }

  /**
   * @brief Returns the holder item
   *
   * Returns the holder item representing
   * one half of the relation table
   *
   * @return The holder item
   */
  holder_type& holder_item() const { return *iter_; }

private:
  friend class container<Type, ContainerType>;
  friend class const_container_iterator<Type, ContainerType>;
  friend class basic_container<Type, ContainerType>;
  friend class object_serializer;
  friend class detail::object_inserter;
  friend class detail::object_deleter;

  void move_to(self &i)
  {
    *iter_ = std::move(*i.iter_);
  }

  holder_container_iterator iter_;
};

/**
 * @brief Represents a const has many iterator
 *
 * Represents a const has many iterator for a has many
 * relationship with std::vector container
 *
 * @tparam T The type of the iterator/container
 */
template < class Type, template < class ... > class ContainerType >
class const_container_iterator
{
private:
  using traits = const_container_iterator_traits<Type, ContainerType>;

public:
  typedef const_container_iterator<Type, ContainerType> self;                       /**< Shortcut value self */
  typedef typename traits::value_type value_type;                             /**< Shortcut value type */
  typedef typename traits::holder_type holder_type;                           /**< Shortcut to the relation type */
  typedef typename traits::difference_type difference_type;                   /**< Shortcut to the difference type*/
  typedef typename traits::holder_container_iterator holder_container_iterator;             /**< Shortcut to the internal container iterator */
  typedef typename traits::holder_const_container_iterator const_holder_container_iterator; /**< Shortcut to the internal const container iterator */

public:
  /**
   * @brief Creates an empty const has many iterator
   */
  const_container_iterator() = default;

  /**
   * @brief Creates a const has many iterator from given internal container iterator
   *
   * @param iter The iterator to create the const has many iterator from
   */
  explicit const_container_iterator(holder_container_iterator iter) : iter_(iter) {}

  /**
   * @brief Creates a const has many iterator from given internal const container iterator
   *
   * @param iter The const iterator to create the const has many iterator from
   */
  explicit const_container_iterator(const_holder_container_iterator iter) : iter_(iter) {}

  /**
   * @brief Creates a const has many iterator from a has many iterator
   *
   * @param iter The iterator to create the const has many iterator from
   */
  const_container_iterator(const container_iterator<Type, ContainerType> &iter) : iter_(iter.iter_) {}

  /**
   * @brief Copy construct a const_has_many_iterator from given iterator.
   *
   * @param iter Iterator to copy construct from.
   */
  const_container_iterator(const self &iter) : iter_(iter.iter_) {}

  //const_has_many_iterator(self &&iter) = default;
  //const_has_many_iterator& operator=(self &&iter) = default;
  /**
   * @brief Copy assigns a new const has many iterator
   *
   * @param iter The iterator to be copy assigned from
   * @return Reference to the created iterator
   */
  const_container_iterator& operator=(const self &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }

  /**
   * @brief Copy assigns a new const has many iterator
   *
   * Copy assigns a new const has many iterator from
   * a non const has many iterator
   *
   * @param iter The iterator to be copy assigned from
   * @return Reference to the created iterator
   */
  const_container_iterator& operator=(const container_iterator<Type, ContainerType> &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }
  ~const_container_iterator() = default;

  /**
   * @brief Compares equality iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator contain
   * the same element true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators contain the same element
   */
  bool operator==(const self &i) const { return (iter_ == i.iter_); }

  /**
   * @brief Compares unequality iterator with another iterator.
   *
   * Compares iterator with another iterator. If other iterator doesn't
   * contain the same element true es returned.
   *
   * @param i The iterator to compare with
   * @return True if iterators doesn't contain the same element
   */
  bool operator!=(const self &i) const { return !this->operator==(i); }

  /**
  * @brief Compares less than iterator with another iterator.
  *
  * Compares iterator with another iterator. If other iterator isn't
  * less than this iterator true es returned.
  *
  * @param i The iterator to compare with
  * @return True if iterators isn't less than this itertor
  */
  bool operator<(const self &i) const { return iter_ < i.iter_; }

  /**
   * @brief Returns the difference of two iterators a and b.
   *
   * @param a The minuend iterator
   * @param b The subtrahend iterator
   * @return The difference between both iterators
   */
  friend difference_type operator-(self a, self b) { return a.iter_ - b.iter_; }

  /**
   * @brief Pre increments self
   *
   * @return A reference to incremented self
   */
  self& operator++()
  {
    ++iter_;
    return *this;
  }

  /**
   * @brief Post increments iterator
   *
   * Post increments iterator and returns a
   * new iterator object.
   *
   * @return Returns new incremented iterator
   */
  self operator++(int)
  {
    self tmp = *this;
    ++iter_;
    return tmp;
  }

  /**
   * @brief Pre decrements self
   *
   * @return A reference to decremented self
   */
  self& operator--()
  {
    --iter_;
    return *this;
  }

  /**
   * @brief Post decrements iterator
   *
   * Post decrements iterator and returns a
   * new iterator object.
   *
   * @return Returns new decremented iterator
   */
  self operator--(int)
  {
    --iter_;
    return self();
  }

  /**
   * @brief Increments iterator with offset
   *
   * @param offset The offset to be incremented
   * @return Reference to incremented iterator
   */
  self& operator+=(difference_type offset)
  {
    iter_ += offset;
    return *this;
  }

  /**
   * @brief Decrements iterator with offset
   *
   * @param offset The offset to be decremented
   * @return Reference to decremented iterator
   */
  self& operator-=(difference_type offset)
  {
    iter_ -= offset;
    return *this;

  }

  /**
   * @brief Increments iterator with offset
   *
   * @param offset The offset to be incremented
   * @return New incremented iterator
   */
  self operator+(difference_type offset) const
  {
    self tmp = *this;
    return tmp += offset;
  }

  /**
   * @brief Decrements iterator with offset
   *
   * @param offset The offset to be decremented
   * @return New decremented iterator
   */
  self operator-(difference_type offset) const
  {
    self tmp = *this;
    return tmp -= offset;
  }

  /**
   * Increment given iterator with the given
   * offset.
   *
   * @param offset Increment offset value
   * @param out Iterator to increment
   * @return Incremented iterator
   */
  friend const self operator+(difference_type offset, self out)
  {
    out.iter_ += offset;
    return out;
  }

  /**
   * Return the current value
   * represented by the iterator
   *
   * @return The current value
   */
  const value_type operator->() const { return iter_->value(); }

  /**
   * Return a const reference to the current value
   * represented by the iterator
   *
   * @return The current value
   */
  const value_type& operator*() const { return iter_->value(); }

  /// @cond MATADOR_DEV
  holder_type holder_item() const { return *iter_; }
  /// @endcond

private:
  void move_to(self &i)
  {
    *iter_ = std::move(*i.iter_);
  }

private:
  friend class container<Type, ContainerType>;
  friend class basic_container<Type, ContainerType>;
  friend class object_serializer;
  friend class detail::object_inserter;

  const_holder_container_iterator iter_;
};

}
#endif //MATADOR_CONTAINER_ITERATOR_HPP
