//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_MANY_HPP
#define OOS_HAS_MANY_HPP

#include "object/has_one.hpp"
#include "object/basic_has_many.hpp"
#include "object/container_type_traits.hpp"
#include "object/object_store.hpp"

#include <vector>
#include <list>
#include <memory>

namespace oos {

class object_store;

/**
 * has_many with std::vector
 */
template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::vector>::value>::type> : public basic_has_many<T, C>
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

  iterator erase(iterator i)
  {
    return iterator(this->container_.erase(i.iter_));
  }
  iterator erase(iterator start, iterator end)
  {
    return iterator(this->container_.erase(start.iter_, end.iter_));
  }

private:
  bool indexed_ = false;
};

template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::list>::value>::type> : public basic_has_many<T, C>
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

  explicit has_many() {}

  iterator insert(iterator pos, const oos::object_ptr<T> &value)
  {
    // create new has_many
    item_type *item = this->create_item(value);
    return this->container_.insert(pos, item);
  }

  void push_front(const oos::object_ptr<T> &value)
  {
    // create new has_many
    value_type item = create_item(value);
    this->container_.push_front(item);
  }

  void push_back(const oos::object_ptr<T> &value)
  {
    // create new has_many
    value_type item = create_item(value);
    this->container_.push_back(item);
  }

//  const_iterator begin() const { return container_.begin(); }
//  const_iterator end() const { return container_.end(); }

//  iterator erase(iterator i) { return container_.erase(i); }
//  iterator erase(iterator start, iterator end) { return container_.erase(start, end); }
};
/*
template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::set>::value>::type> : public basic_has_many<T>
{
public:
  typedef C<oos::has_one<T>, std::less<oos::has_one<T>>, std::allocator<oos::has_one<T>>> container_type;
  typedef container_type& container_reference;

  void insert(T *value) {}

private:
  C<oos::has_one<T>, std::less<oos::has_one<T>>, std::allocator<oos::has_one<T>>> container;
};
*/
}
#endif //OOS_HAS_MANY_HPP
