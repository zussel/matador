//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_MANY_HPP
#define OOS_HAS_MANY_HPP

#include "object/has_one.hpp"
#include "object/container_type_traits.hpp"

#include <vector>
#include <list>

namespace oos {

template < class T >
class basic_has_many
{
public:
  typedef has_one<T> value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
};

template < class T, template <class ...> class C = std::vector, class Enable = void >
class has_many;

template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::vector>::value || is_same_container_type<C, std::list>::value>::type> : public basic_has_many<T>
{
public:

  typedef oos::has_one<T> value_type;
  typedef C<oos::has_one<T>, std::allocator<oos::has_one<T>>> container_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::iterator iterator;
  typedef typename container_type::const_iterator const_iterator;

  void push_back(const value_type &value)
  {
    container_.push_back(value);
  }

  iterator begin() { return container_.begin(); }
  iterator end() { return container_.end(); }

  const_iterator begin() const { return container_.begin(); }
  const_iterator end() const { return container_.end(); }

  iterator erase(iterator i) { return container_.erase(i); }

  size_type size() const { return container_.size(); }
  bool empty() const { return container_.empty(); }
private:
  container_type container_;
};

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

}
#endif //OOS_HAS_MANY_HPP
