#ifndef BASE_CLASS_HPP
#define BASE_CLASS_HPP

#include <type_traits>

namespace matador {

/**
 * @brief Safely casts a given derived class to its base class
 *
 * @tparam B The base class type
 * @tparam D The class type of the derived class
 * @param derived The derived object
 * @return The casted object
 */
template < class B, class D>
const B* base_class(const D *derived)
{
  static_assert(!std::is_same<B, D>::value, "class B must not be of same type as class D");
  static_assert(std::is_base_of<B, D>::value, "class B must be base of class D");
  return static_cast<const B*>(derived);
}

/**
 * @brief Safely casts a given derived class to its base class
 *
 * @tparam B The base class type
 * @tparam D The class type of the derived class
 * @param derived The derived object
 * @return The casted object
 */
template < class B, class D>
B* base_class(D *derived)
{
  static_assert(!std::is_same<B, D>::value, "class B must not be of same type as class D");
  static_assert(std::is_base_of<B, D>::value, "class B must be base of class D");
  return static_cast<B*>(derived);
}

}

#endif /* BASE_CLASS_HPP */
