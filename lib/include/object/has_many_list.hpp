//
// Created by sascha on 1/15/16.
//

#ifndef OOS_HAS_MANY_LIST_HPP
#define OOS_HAS_MANY_LIST_HPP

#include "object/basic_has_many.hpp"

#include <list>

namespace oos {

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

}
#endif //OOS_HAS_MANY_LIST_HPP
