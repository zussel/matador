//
// Created by sascha on 1/15/16.
//

#ifndef OOS_HAS_MANY_VECTOR_HPP
#define OOS_HAS_MANY_VECTOR_HPP

#include "object/basic_has_many.hpp"
#include "object/object_store.hpp"

#include <vector>

namespace oos {

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
