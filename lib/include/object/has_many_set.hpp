//
// Created by sascha on 1/15/16.
//

#ifndef OOS_HAS_MANY_SET_HPP
#define OOS_HAS_MANY_SET_HPP

#include "object/basic_has_many.hpp"

#include <set>

namespace oos {

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

#endif //OOS_HAS_MANY_SET_HPP
