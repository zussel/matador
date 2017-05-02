//
// Created by sascha on 1/15/16.
//

#ifndef OOS_TEST_HAS_MANY_LIST_HPP
#define OOS_TEST_HAS_MANY_LIST_HPP

#include "matador/object/basic_has_many.hpp"
#include "matador/object/object_store.hpp"

#include "matador/utils/is_builtin.hpp"

#include <list>

namespace matador {

/// @cond MATADOR_DEV

/// @endcond




namespace detail {

/// @cond MATADOR_DEV

template<class T>
class has_many_inserter<T, std::list, typename std::enable_if<!is_builtin<T>::value>::type>
{
public:
  typedef typename basic_has_many<T, std::list>::iterator iterator;
  typedef typename has_many_iterator_traits<T, std::list>::relation_type relation_type;
  typedef typename basic_has_many<T, std::list>::mark_modified_owner_func mark_modified_owner_func;
  typedef has_many<T, std::list> container_type;
  typedef typename basic_has_many<T, std::vector>::item_type item_type;

  has_many_inserter(container_type &container) : container_(container) {}

  void insert(iterator i)
  {
    relation_type rtype(*i.iter_);
    if (container_.relation_info_ != nullptr) {
      if (container_.relation_info_->type == detail::relation_field_endpoint::BELONGS_TO) {
        container_.relation_info_->set(*container_.store(), *i, container_.owner_);
      } else if (container_.relation_info_->type == detail::relation_field_endpoint::HAS_MANY) {
        container_.relation_info_->append(*container_.store(), *i, container_.owner_);
        container_.store()->insert(rtype);
      }
    } else {
      container_.store()->insert(rtype);
    }
    container_.mark_modified_owner_(*container_.store(), container_.owner_);
  }

  void append_proxy(object_proxy *proxy)
  {
    item_type *item = new item_type(
        container_.owner_field(), container_.item_field(), container_.owner_id_, proxy
    );
    relation_type iptr(item);
    iterator i(container_.container_.insert(container_.container_.end(), iptr));
    if (container_.store()) {
      insert(i);
    }
  }

  container_type &container_;
};

template<class T>
class has_many_inserter<T, std::list, typename std::enable_if<is_builtin<T>::value>::type>
{
public:
  typedef typename basic_has_many<T, std::list>::iterator iterator;
  typedef typename has_many_iterator_traits<T, std::list>::relation_type relation_type;
  typedef has_many<T, std::list> container_type;

  has_many_inserter(container_type &container) : container_(container) {}

  void insert(iterator i)
  {
    relation_type rtype(*i.iter_);
    container_.store()->insert(rtype);
    container_ .mark_modified_owner_(*container_.store(), container_.owner_);
  }

  void append_proxy(object_proxy*) {}

  container_type &container_;
};

template<class T>
class has_many_deleter<T, std::list, typename std::enable_if<!is_builtin<T>::value>::type>
{
public:
  typedef typename basic_has_many<T, std::list>::iterator iterator;
  typedef typename has_many_iterator_traits<T, std::list>::relation_type relation_type;
  typedef typename has_many_iterator_traits<T, std::list>::value_type value_type;
  typedef has_many<T, std::list> container_type;

  has_many_deleter(container_type &container) : container_(container) {}

  void remove(iterator i)
  {
    relation_type rtype(*i.iter_);
    if (container_.relation_info_ != nullptr) {
      if (container_.relation_info_->type == detail::relation_field_endpoint::BELONGS_TO) {
        container_.relation_info_->clear(*container_.store(), *i);
      } else if (container_.relation_info_->type == detail::relation_field_endpoint::HAS_MANY) {
        container_.relation_info_->remove(*container_.store(), *i, container_.owner_);
        container_.store()->remove(rtype);
      }
    } else {
      container_.store()->remove(rtype);
    }
  }

  void remove_proxy(object_proxy *proxy)
  {
    auto val = value_type(proxy);
    container_.remove(val);
  }

  container_type &container_;
};

template<class T>
class has_many_deleter<T, std::list, typename std::enable_if<is_builtin<T>::value>::type>
{
public:
  typedef typename basic_has_many<T, std::list>::iterator iterator;
  typedef typename has_many_iterator_traits<T, std::list>::relation_type relation_type;
  typedef has_many<T, std::list> container_type;

  has_many_deleter(container_type &container) : container_(container) {}

  void remove(iterator i)
  {
    relation_type rtype(*i.iter_);
    container_.store()->remove(rtype);
  }

  void remove_proxy(object_proxy*) {}

  container_type &container_;
};

/// @endcond

}

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
  typedef typename base::size_type size_type;                      /**< Shortcut to size_type */
  typedef typename base::relation_type relation_type;           /**< Shortcut to relation_type */

private:
  typedef typename base::container_iterator container_iterator; /**< Shortcut to container_iterator */

public:
  /**
   * @brief Creates an empty has_many object
   *
   * Creates an empty has_many object with a
   * std::list as container type
   */
  has_many() : inserter_(*this), deleter_(*this)
  {
    this->append_func_ = [=](object_proxy *proxy) {
      this->inserter_.append_proxy(proxy);
    };
    this->remove_func_ = [=](object_proxy *proxy) {
      this->deleter_.remove_proxy(proxy);
    };
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
    // create new has_many
    item_type *item = this->create_item(value);
    relation_type iptr(item);
//    container_iterator i = pos.iter_;
    iterator i(this->container_.insert(pos.iter_, iptr));
    if (this->ostore_) {
      inserter_.insert(i);
//      inserter_.insert(this->relation_info_, *this->ostore_, iptr, *this->owner_, this->mark_modified_owner_);
    }
    return i;
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
   * @brief Clears the list
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
  void remove(const value_type &value)
  {
    iterator first = this->begin();
    iterator last = this->end();
    while (first != last) {
      iterator next = first;
      ++next;
      if (*first == value) {
        erase(first);
      }
      first = next;
    }
  }

  /**
   * Removes all elements from the container for which
   * the given predicate returns true.
   *
   * @tparam P Type of the predicate
   * @param predicate Predicate to be evaluated
   */
  template < class P >
  void remove_if(P predicate)
  {
    iterator first = this->begin();
    iterator last = this->end();
    while (first != last) {
      iterator next = first;
      ++next;
      if (predicate(*first)) {
        erase(first);
      }
      first = next;
    }
  }

  /**
   * @brief Erase the element at given position.
   *
   * Erase the element at given position and return the iterator
   * following the last removed element.
   * @param i Iterator to the element to remove
   * @return Iterator following the last removed element
   */
  iterator erase(iterator i)
  {
    if (this->ostore_) {
      deleter_.remove(i);
    }
    container_iterator ci = this->container_.erase(i.iter_);
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
   * @param end Last iterator of the range
   * @return Iterator following the last removed element
   */
  iterator erase(iterator start, iterator end)
  {
    iterator i = start;
    if (this->ostore_) {
      while (i != end) {
        deleter_.remove(i++);
      }
    }
    return iterator(this->container_.erase(start.iter_, end.iter_));
  }

private:
  detail::has_many_inserter<T, std::list> inserter_;
  detail::has_many_deleter<T, std::list> deleter_;
};

}
#endif //OOS_HAS_MANY_LIST_HPP
