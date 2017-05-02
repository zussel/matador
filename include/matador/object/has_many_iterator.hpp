//
// Created by sascha on 01.05.17.
//

#ifndef MATADOR_HAS_MANY_ITERATOR_HPP
#define MATADOR_HAS_MANY_ITERATOR_HPP

#include "matador/object/object_ptr.hpp"
#include "matador/object/has_many_to_many_item.hpp"

#include "matador/utils/is_builtin.hpp"

#include <vector>
#include <list>

namespace matador {

/// @cond MATADOR_DEV

template < class T, template < class ... > class C, class Enable = void >
struct has_many_iterator_traits;

template<class T>
struct has_many_iterator_traits<T, std::vector, typename std::enable_if<!is_builtin<T>::value>::type>
  : public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef object_ptr<T> value_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;

  typedef has_many_item_holder <T> holder_type;
  typedef std::vector<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template<class T>
struct has_many_iterator_traits<T, std::vector, typename std::enable_if<is_builtin<T>::value>::type>
  : public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef T value_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;

  typedef has_many_item_holder <T> holder_type;
  typedef std::vector<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < class T >
struct has_many_iterator_traits<T, std::list, typename std::enable_if<!is_builtin<T>::value>::type>
  : public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef object_ptr<T> value_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;

  typedef has_many_item_holder<T> holder_type;
  typedef std::list<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < class T >
struct has_many_iterator_traits<T, std::list, typename std::enable_if<is_builtin<T>::value>::type>
  : public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef T value_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;

  typedef has_many_item_holder<T> holder_type;
  typedef std::list<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

/// @endcond

template < class T, template < class ... > class C, class Enable = void >
struct has_many_iterator;

/**
 * @brief Represents a has many iterator
 *
 * Represents a has many iterator for a has many
 * relationship with std::list container
 *
 * @tparam T The type of the iterator/container
 */
template < class T >
class has_many_iterator<T, std::list> : public has_many_iterator_traits<T, std::list>
{
private:
  typedef has_many_iterator_traits<T, std::list> traits;

public:
  typedef has_many_iterator<T, std::list> self;                               /**< Shortcut value self */
  typedef typename traits::value_type value_type;                             /**< Shortcut value type */
  typedef typename traits::difference_type difference_type;                   /**< Shortcut to the difference type*/
  typedef typename traits::container_iterator container_iterator;             /**< Shortcut to the internal container iterator */
  typedef typename traits::const_container_iterator const_container_iterator; /**< Shortcut to the internal const container iterator */

public:
  /**
   * @brief Creates an empty has many iterator
   */
  has_many_iterator() {}

  /**
   * @brief Copy constructs an iterator from another iterator
   *
   * @param iter The iterator to copy from
   */
  has_many_iterator(const self &iter) : iter_(iter.iter_) {}

  //has_many_iterator(self &&iter) = default;
  //has_many_iterator& operator=(self &&iter) = default;
  /**
   * @brief Creates a has many iterator from given internal container iterator
   *
   * @param iter The iterator to create the has many iterator from
   */
  explicit has_many_iterator(container_iterator iter) : iter_(iter) {}

  /**
   * @brief Copy assign an iterator from another iterator
   *
   * @param iter The iterator to copy from
   * @return A reference to self
   */
  has_many_iterator& operator=(const self &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }

  ~has_many_iterator() {}

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

  //@{
  /**
   * Return the current value
   * represented by the iterator
   *
   * @return The current value
   */
  value_type operator->() const { return get(); }
  value_type operator*() const { return get(); }
  value_type get() const { return iter_->value(); }
  //@}

private:
  friend class has_many<T, std::list>;
  friend class const_has_many_iterator<T, std::list>;
  friend class basic_has_many<T, std::list>;
  friend class detail::has_many_inserter<T, std::list>;
  friend class detail::has_many_deleter<T, std::list>;
  friend class object_serializer;
  friend class detail::object_inserter;

  holder_type holder_item() const { return *iter_; }

  container_iterator iter_;
};

/// @cond MATADOR_DEV

template < class T, template < class... > class C, class Enable = void >
struct const_has_many_iterator_traits;

template < class T >
struct const_has_many_iterator_traits<T, std::list, typename std::enable_if<!is_builtin<T>::value>::type>
  : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef object_ptr<T> value_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
};

template < class T >
struct const_has_many_iterator_traits<T, std::list, typename std::enable_if<is_builtin<T>::value>::type>
  : public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef T value_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
};

/// @endcond

template < class T, template < class... > class C, class Enable = void >
class const_has_many_iterator;

/**
 * @brief Represents a const has many iterator
 *
 * Represents a const has many iterator for a has many
 * relationship with std::list container
 *
 * @tparam T The type of the iterator/container
 */
template < class T >
class const_has_many_iterator<T, std::list> : public const_has_many_iterator_traits<T, std::list>
{
private:
  typedef const_has_many_iterator_traits<T, std::list> traits;

public:
  typedef const_has_many_iterator<T, std::list> self;                         /**< Shortcut to self */
  typedef typename traits::value_type value_type;                             /**< Shortcut value type */
  typedef typename traits::difference_type difference_type;                   /**< Shortcut to the difference type*/

public:
  /**
   * @brief Creates an empty const has many iterator
   */
  const_has_many_iterator() {}

  /**
   * @brief Creates a const has many iterator from given internal container iterator
   *
   * @param iter The iterator to create the const has many iterator from
   */
  explicit const_has_many_iterator(container_iterator iter) : iter_(iter) {}

  /**
   * @brief Creates a const has many iterator from given internal const container iterator
   *
   * @param iter The const iterator to create the const has many iterator from
   */
  explicit const_has_many_iterator(const_container_iterator iter) : iter_(iter) {}

  /**
   * @brief Creates a const has many iterator from a has many iterator
   *
   * @param iter The iterator to create the const has many iterator from
   */
  const_has_many_iterator(const has_many_iterator<T, std::list> &iter) : iter_(iter.iter_) {}

  /**
   * @brief Copy construct a const_has_many_iterator from given iterator.
   *
   * @param iter Iterator to copy construct from.
   */
  const_has_many_iterator(const self &iter) : iter_(iter.iter_) {}

  //const_has_many_iterator(self &&iter) = default;
  //const_has_many_iterator& operator=(self &&iter) = default;

  /**
   * @brief Copy assigns a new const has many iterator
   *
   * @param iter The iterator to be copy assigned from
   * @return Reference to the created iterator
   */
  const_has_many_iterator& operator=(const self &iter)
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
  const_has_many_iterator& operator=(const has_many_iterator<T, std::list> &iter)
  {
    iter_ = iter.iter_;
    return *this;
  }
  ~const_has_many_iterator() {}

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

  //@{
  /**
   * Return the current value
   * represented by the iterator
   *
   * @return The current value
   */
  const value_type operator->() const { return get(); }
  const value_type operator*() const { return get(); }
  const value_type get() const { return iter_->value(); }
  //@}

private:
  friend class has_many<T, std::list>;
  friend class basic_has_many<T, std::list>;
  friend class object_serializer;
  friend class detail::object_inserter;

  const relation_type relation_item() const { return *iter_; }

  const_container_iterator iter_;
};

}

#endif //MATADOR_HAS_MANY_ITERATOR_HPP
