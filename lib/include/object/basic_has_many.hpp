//
// Created by sascha on 1/6/16.
//

#ifndef OOS_BASIC_HAS_MANY_HPP
#define OOS_BASIC_HAS_MANY_HPP

#include "object/has_one.hpp"
#include "object/has_many_item.hpp"
#include "object/abstract_has_many.hpp"
#include "object/container_type_traits.hpp"

namespace oos {

namespace detail {
class object_inserter;
}

/// @cond OOS_DEV

template < class T, template <class ...> class C = std::vector >
class basic_has_many;

template < class T, template <class ...> class C = std::vector  >
class has_many;

template < class T, template <class ...> class C, class Enable = void >
struct has_many_iterator_traits;

template < class T, template <class ...> class C, class Enable = void >
struct const_has_many_iterator_traits;

template < class T, template <class ...> class C >
class has_many_iterator;

template < class T, template <class ...> class C >
class const_has_many_iterator;

/// @endcond

/**
 * @brief Base class for has many relationship classes
 *
 * This class acts as a base class for all relationship
 * classes. It provides all common functionality for these
 * classes.
 *
 * @tparam T The type of the contained objects
 * @tparam C The underlying container class
 */
template < class T, template <class ...> class C >
class basic_has_many : public abstract_has_many
{
public:
  typedef basic_has_many<T, C> base;                             /**< Shortcut to self */
  typedef has_many_iterator<T, C> iterator;                      /**< Shortcut to iterator class */
  typedef typename iterator::value_type value_type;              /**< Shortcut to value type */
  typedef typename iterator::item_type item_type;                /**< Shortcut to item type */
  typedef typename iterator::container_type container_type;      /**< Shortcut to container type */
  typedef typename iterator::relation_type relation_type;        /**< Shortcut to relation type */
  typedef typename iterator::internal_type internal_type;        /**< Shortcut to internal type */
  typedef const_has_many_iterator<T,C> const_iterator;           /**< Shortcut to const iterator */
  typedef typename container_type::size_type size_type;          /**< Shortcut to size type */
  typedef typename container_type::iterator container_iterator;  /**< Shortcut to container iterator */

public:
  /**
   * @brief Returns the begin iterator of the container
   *
   * @return The begin iterator
   */
  iterator begin() { return iterator(container_.begin()); }
  /**
   * @brief Returns the end iterator of the container
   *
   * @return The end iterator
   */
  iterator end() { return iterator(container_.end()); }

  /**
   * @brief Returns the const begin iterator of the container
   *
   * @return The const begin iterator
   */
  const_iterator begin() const { return const_iterator(container_.begin()); }
  /**
   * @brief Returns the const end iterator of the container
   *
   * @return The const end iterator
   */
  const_iterator end() const { return const_iterator(container_.end()); }

  /**
   * @brief Returns a copy of the first element
   *
   * @return A copy of the first element
   */
  value_type front() { return *begin(); }

  /**
   * @brief Returns a const copy of the first element
   *
   * @return A const copy of the first element
   */
  const value_type front() const { return *begin(); }

  /**
   * @brief Returns a const copy of the first element
   *
   * @return A const copy of the first element
   */
  value_type back()
  {
    iterator tmp = end();
    --tmp;
    return *tmp;
  }

  /**
   * @brief Returns a const copy of the last element
   *
   * @return A const copy of the last element
   */
  const value_type back() const
  {
    const_iterator tmp = end();
    --tmp;
    return *tmp;
  }

  /**
   * @brief Returns the current size of the container
   *
   * @return The current size
   */
  size_type size() const { return container_.size(); }

  /**
   * @brief Returns true if the container is empty
   *
   * @return True if the container is empty
   */
  bool empty() const { return container_.empty(); }

  /**
   * @brief Clears the container
   */
  void reset() { container_.clear(); }

  /**
   * @brief Appends an item to the end of the container
   *
   * @param item Element to be appended
   */
  void append(const typename iterator::internal_type &item) { container_.push_back(item); }

  /**
   * @brief Returns true if the container uses a join table
   *
   * @return True if the container uses a join table
   */
  bool has_join_table() const { return true; }

protected:
  /// @cond OOS_DEV

  friend class detail::object_inserter;
  friend class object_store;

  object_proxy *owner_ = nullptr;
  std::shared_ptr<basic_identifier> owner_id_;

  std::function<void(object_store&, object_proxy*)> mark_modified_owner_;

  container_type container_;

  /// @endcond

};

}
#endif //OOS_BASIC_HAS_MANY_HPP
