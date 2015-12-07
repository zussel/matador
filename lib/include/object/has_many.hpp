//
// Created by sascha on 07.12.15.
//

#ifndef OOS_HAS_MANY_HPP
#define OOS_HAS_MANY_HPP

#include <vector>
#include <list>

namespace oos {
/*
template < class T >
class basic_has_many
{
public:
  typedef holder<T> value_type;
  typedef value_type& reference;
  typedef value_type* pointer;
};

template < class T, template <class ...> class C = std::vector, class Enable = void >
class has_many;

template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::vector>::value || is_same_container_type<C, std::list>::value>::type> : public basic_has_many<T>
{
public:

  C<holder<T>, std::allocator<holder<T>>> container;
};

template < class T, template <class ...> class C >
class has_many<T, C, typename std::enable_if<is_same_container_type<C, std::set>::value>::type> : public basic_has_many<T>
{
public:
  typedef C<holder<T>, std::less<holder<T>>, std::allocator<holder<T>>> container_type;
  typedef container_type& container_reference;

  void insert(T *value) {}

private:
  C<holder<T>, std::less<holder<T>>, std::allocator<holder<T>>> container;
};
*/
}
#endif //OOS_HAS_MANY_HPP
