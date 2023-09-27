#ifndef OOS_HAS_MANY_VECTOR_HPP
#define OOS_HAS_MANY_VECTOR_HPP

//#include "matador/object/generic_access.hpp"
#include "matador/object/abstract_container.hpp"
//#include "matador/object/has_many_to_many_item.hpp"
#include "matador/object/container_item_holder.hpp"
#include "matador/object/container_iterator.hpp"
#include "matador/object/container_iterator_traits.hpp"
#include "matador/object/relation_endpoint.hpp"

//#include "matador/utils/is_builtin.hpp"

#include <vector>
#include <algorithm>

namespace matador {

namespace detail {
template < class Value >
class relation_endpoint_value_inserter;

template < class Value >
class relation_endpoint_value_remover;
}

/**
 * @brief Has many relation class using a std::vector as container
 *
 * The has many relation class uses a std::vector as internal
 * container to store the objects.
 *
 * It provides all main interface functions std::vector provides
 * - insert element at a iterator position
 * - access with bracket operator []
 * - push back an element
 * - erase an element at iterator position
 * - erase a range of elements within first and last iterator position
 * - clear the container
 *
 * All of these methods are wrapped around the std::vector methods plus
 * the modification in the corresponding object_store and notification
 * of the transaction observer
 *
 * The relation holds object_ptr elements as well as scalar data elements.
 *
 * @tparam Type The type of the elements
 */
template<class Type, template < class ... > class ContainerType = std::vector>
class container : public abstract_container
{
public:
  using traits = container_iterator_traits<Type, ContainerType>;                /**< Shortcut to traits class */
  using iterator = container_iterator<Type, ContainerType>;                            /**< Shortcut to iterator class */
  using const_iterator = const_container_iterator<Type, ContainerType>;                            /**< Shortcut to iterator class */
  using value_type = typename traits::value_type;                    /**< Shortcut to value_type */
  using holder_type = typename traits::holder_type;                  /**< Shortcut to holder_type */
  using holder_container_type = typename traits::holder_container_type; /**< Shortcut to container type */
  using size_type = typename holder_container_type::size_type;                      /**< Shortcut to size_type */

private:
  using container_type_iterator = typename traits::holder_container_iterator;

public:
  /**
   * @brief Creates an empty container object
   *
   * Creates an empty container object with a
   * std::vector as container type
   */
  template < typename U = Type >
  container()
  {
    static_assert(!std::is_same<U, std::string>::value, "std::string needs a size");
  }

  template < typename U = Type >
  explicit container(size_t size)
  : size_(size)
  {
    static_assert(std::is_same<U, std::string>::value, "Size is only needed for std::string");
  }

  ~container()
  {
    for(holder_type &item : holder_container_) {
      if (item.is_inserted()) {
        auto &val = item.value();
        this->decrement_reference_count(val);
      }
    }
  }

  /**
 * @brief Returns the begin iterator of the container
 *
 * @return The begin iterator
 */
  iterator begin() { return iterator(holder_container_.begin()); }
  /**
   * @brief Returns the end iterator of the container
   *
   * @return The end iterator
   */
  iterator end() { return iterator(holder_container_.end()); }

  /**
   * @brief Returns the const begin iterator of the container
   *
   * @return The const begin iterator
   */
  const_iterator begin() const { return const_iterator(holder_container_.begin()); }
  /**
   * @brief Returns the const end iterator of the container
   *
   * @return The const end iterator
   */
  const_iterator end() const { return const_iterator(holder_container_.end()); }

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
  value_type front() const { return *begin(); }

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
  value_type back() const
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
  size_type size() const { return holder_container_.size(); }

  /**
   * @brief Returns true if the container is empty
   *
   * @return True if the container is empty
   */
  bool empty() const { return holder_container_.empty(); }

  /**
   * @brief Clears the container
   */
  void reset() { holder_container_.clear(); }

  /**
   * @brief Appends an item to the end of the container
   *
   * @param item Element to be appended
   */
  void append(const typename iterator::holder_type &item)
  {
    this->mark_holder_as_inserted(const_cast<typename iterator::holder_type &>(item));
    holder_container_.push_back(item);
  }

  /**
   * @brief Inserts an element at the given position.
   *
   * @param pos The position to insert at
   * @param value The element to be inserted
   * @return The iterator at position of inserted element
   */
  iterator insert(iterator pos, const value_type &value)
  {
    holder_type holder(value, nullptr);

    if (this->ostore_) {
      this->relation_info_->insert_holder(*this->ostore_, holder, this->owner_);

      this->mark_holder_as_inserted(holder);

      if (!matador::is_builtin<Type>::value) {
        this->relation_info_->insert_value_into_foreign(holder, this->owner_);
      }

      this->mark_modified_owner_(*this->ostore_, this->owner_);
    }

    return iterator(this->holder_container_.insert(pos.iter_, holder));
  }

  /**
   * @brief Inserts an element at first position.
   *
   * @param value The element to be inserted
   */
  template < template < class ... > class C = ContainerType >
  void push_front(const value_type &value)
  {
    static_assert(!std::is_same<C<holder_type>, std::vector<ContainerType<holder_type>>>::value, "No push_front in vector");
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
   * @brief Clears the vector
   */
  void clear()
  {
    erase(this->begin(), this->end());
  }

  /**
   * Removes all values equal to given value
   * from the container
   *
   * @param value Value to remove
   */
  iterator remove(const value_type &value)
  {
    return remove_if([&value](const value_type &val) {
      return val == value;
    });
  }

  /**
   * Removes all elements from the container for which
   * the given predicate returns true.
   *
   * @tparam P Type of the predicate
   * @param predicate Predicate to be evaluated
   */
  template<class P>
  iterator remove_if(P predicate)
  {
    auto first = this->holder_container_.begin();
    auto last = this->holder_container_.end();

    first = find_if(first, last, predicate);
    if (first != last) {
      // remove first
      remove_it(*first);
      for (auto i = first; ++i != last;) {
        if (!predicate(i->value())) {
          *first = std::move(*i);
          ++first;
        } else {
          remove_it(*i);
        }
      }
    }
    return iterator(this->holder_container_.erase(first, last));
  }

  /**
   * @brief Remove the element at given position.
   *
   * Erase the element at given position and return the iterator
   * following the last removed element.
   *
   * @param i Iterator to the element to remove
   * @return Iterator following the last removed element
   */
  iterator erase(iterator i)
  {
    remove_it(i.holder_item());
    auto ci = this->holder_container_.erase(i.iter_);
    return iterator(ci);
  }

  /**
   * @brief Remove the elements of given range.
   *
   * Remove the elements of the given range identified
   * by the first and last iterator position. Where the first
   * iterator is included and the last iterator is not included
   * [first; last)
   *
   * @param start First iterator of the range
   * @param end Last iterator of the ranges
   * @return Iterator following the last removed element
   */
  iterator erase(iterator start, iterator end)
  {
    iterator i = start;
    if (this->ostore_) {
      while (i != end) {
        this->mark_holder_as_removed(i.holder_item());

        if (!matador::is_builtin<Type>::value) {
          this->relation_info_->remove_value_from_foreign(i.holder_item(), this->owner_);
        }
        this->relation_info_->remove_holder(*this->ostore_, i.holder_item(), this->owner_);
        ++i;
      }
      this->mark_modified_owner_(*this->ostore_, this->owner_);
    }
    return iterator(this->holder_container_.erase(start.iter_, end.iter_));
  }

  /**
   * @brief Finds an element matching the predicate
   *
   * Finds the first element where the given predicate
   * matches. If it doesn't match end() is returned
   *
   * @tparam P The type of the predicate object
   * @param predicate The predicate object
   * @return The found element of end()
   */
  template<class P>
  iterator find_if(P predicate)
  {
    return iterator(find_if(this->holder_container_.begin(), this->holder_container_.end(), predicate));
  }

  size_t size_{};

private:
  void remove_it(holder_type &holder)
  {
    if (this->ostore_) {
      this->mark_holder_as_removed(holder);
      if (!matador::is_builtin<Type>::value) {
        this->relation_info_->remove_value_from_foreign(holder, this->owner_);
      }
      this->relation_info_->remove_holder(*this->ostore_, holder, this->owner_);
      this->mark_modified_owner_(*this->ostore_, this->owner_);
    }
  }

  template<class InputIt, class P>
  container_type_iterator find_if(InputIt first, InputIt last, P predicate)
  {
    for (; first != last; ++first) {
      if (predicate(first->value())) {
        return first;
      }
    }
    return last;
  }

private:
  void insert_holder(const holder_type &holder)
  {
    this->mark_holder_as_inserted(const_cast<holder_type &>(holder));
    this->increment_reference_count(holder.value());
    this->holder_container_.push_back(holder);
  }

  void remove_holder(const holder_type &holder)
  {
    auto i = std::remove(this->holder_container_.begin(), this->holder_container_.end(), holder);
    if (i != this->holder_container_.end()) {
      this->mark_holder_as_removed(*i);
      this->decrement_reference_count(holder.value());
      this->holder_container_.erase(i, this->holder_container_.end());
    } else {
      std::cout << "couldn't find holder to remove " << *holder.item_proxy() << "\n";
    }
  }

private:
  friend class detail::relation_endpoint_value_inserter<Type>;
  friend class detail::relation_endpoint_value_remover<Type>;
  friend class detail::object_deleter;
  friend class detail::object_inserter;
  friend class object_store;
  friend class object_serializer;
  friend class object_deserializer;

  holder_container_type holder_container_;

  std::shared_ptr<detail::relation_endpoint<Type>> relation_info_;
};

}
#endif //OOS_HAS_MANY_VECTOR_HPP
