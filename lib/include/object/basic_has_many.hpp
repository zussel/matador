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

template < class T, template <class ...> class C >
class basic_has_many : public abstract_has_many
{
public:
  typedef basic_has_many<T, C> base;
  typedef has_many_iterator<T, C> iterator;
  typedef typename iterator::value_type value_type;
  typedef typename iterator::item_type item_type;
  typedef typename iterator::container_type container_type;
  typedef typename iterator::relation_type relation_type;
  typedef typename iterator::internal_type internal_type;
  typedef const_has_many_iterator<T,C> const_iterator;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::iterator container_iterator;

public:
  iterator begin() { return iterator(container_.begin()); }
  iterator end() { return iterator(container_.end()); }

  const_iterator begin() const { return const_iterator(container_.begin()); }
  const_iterator end() const { return const_iterator(container_.end()); }

  size_type size() const { return container_.size(); }
  bool empty() const { return container_.empty(); }

  void reset() { container_.clear(); }

  void append(const typename iterator::internal_type &item) { container_.push_back(item); }

  bool has_join_table() const
  {
    return true;
  }

protected:
  friend class detail::object_inserter;
  friend class object_store;

  object_proxy *owner_ = nullptr;
  std::shared_ptr<basic_identifier> owner_id_;

//  detail::modified_marker mark_modified_owener_;

  std::function<void(object_store&, object_proxy*)> mark_modified_owener_;

  container_type container_;
};

}
#endif //OOS_BASIC_HAS_MANY_HPP
