//
// Created by sascha on 12/2/15.
//

#ifndef BASE_CLASS_HPP
#define BASE_CLASS_HPP

#include <type_traits>

namespace oos {

template < class B, class D>
const B* base_class(const D *derived)
{
  static_assert(!std::is_same<B, D>::value, "class B must not be of same type as class D");
  static_assert(std::is_base_of<B, D>::value, "class B must be base of class D");
  return static_cast<const B*>(derived);
}

template < class B, class D>
B* base_class(D *derived)
{
  static_assert(!std::is_same<B, D>::value, "class B must not be of same type as class D");
  static_assert(std::is_base_of<B, D>::value, "class B must be base of class D");
  return static_cast<B*>(derived);
}

}

#endif /* BASE_CLASS_HPP */
