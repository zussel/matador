#ifndef MATADOR_CONTAINER_ITERATOR_TRAITS_HPP
#define MATADOR_CONTAINER_ITERATOR_TRAITS_HPP

#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/container_item_holder.hpp"
#include "matador/object/value_type_traits.hpp"

#include <vector>

namespace matador {

/// @cond MATADOR_DEV
template < class Type, template < class ... > class ContainerType >
struct container_iterator_traits;
template < class Type, template < class... > class ContainerType >
struct const_container_iterator_traits;

/*
 * has_many_vector iterator traits
 */
template < class Type >
struct container_iterator_traits<Type, std::vector>
{
  using self = container_iterator_traits<Type, std::vector>;
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename value_type_traits<Type>::value_type;
  using difference_type = std::ptrdiff_t;
  using holder_type = container_item_holder<Type>;
  using holder_container_type = std::vector<holder_type, std::allocator<holder_type>>;
  using holder_container_iterator = typename holder_container_type::iterator ;
  using holder_const_container_iterator = typename holder_container_type::const_iterator;
};

template < class Type >
struct const_container_iterator_traits<Type, std::vector>
{
  using self = const_container_iterator_traits<Type, std::vector>;
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename value_type_traits<Type>::value_type;
  using difference_type = std::ptrdiff_t;
  using holder_type = container_item_holder<Type>;
  using holder_container_type = std::vector<holder_type, std::allocator<holder_type>>;
  using holder_container_iterator = typename holder_container_type::iterator ;
  using holder_const_container_iterator = typename holder_container_type::const_iterator;
};

/*
 * has_many_list iterator traits
 */
template < class Type >
struct container_iterator_traits<Type, std::list>
{
  using self = container_iterator_traits<Type, std::list>;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename value_type_traits<Type>::value_type;
  using difference_type = std::ptrdiff_t;
  using holder_type = container_item_holder<Type>;
  using holder_container_type = std::list<holder_type, std::allocator<holder_type>>;
  using holder_container_iterator = typename holder_container_type::iterator ;
  using holder_const_container_iterator = typename holder_container_type::const_iterator;
};

template < class Type >
struct const_container_iterator_traits<Type, std::list>
{
  using self = const_container_iterator_traits<Type, std::list>;
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename value_type_traits<Type>::value_type;
  using difference_type = std::ptrdiff_t;
  using holder_type = container_item_holder<Type>;
  using holder_container_type = std::list<holder_type, std::allocator<holder_type>>;
  using holder_container_iterator = typename holder_container_type::iterator ;
  using holder_const_container_iterator = typename holder_container_type::const_iterator;
};

/// @endcond

}
#endif //MATADOR_CONTAINER_ITERATOR_TRAITS_HPP
