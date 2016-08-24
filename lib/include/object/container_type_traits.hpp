#ifndef CONTAINER_TYPE_TRAITS_HPP
#define CONTAINER_TYPE_TRAITS_HPP

#include <vector>
#include <list>
#include <set>
#include <type_traits>

template < template < class ... > class Container>
struct is_container_type : std::false_type { };

template <> struct is_container_type<std::vector> : std::true_type {};
template <> struct is_container_type<std::list> : std::true_type {};
template <> struct is_container_type<std::set> : std::true_type {};

template < template < class ... > class ContainerA, template < class ... > class ContainerB >
struct is_same_container_type : std::false_type { };

template < template < class ... > class Container >
struct is_same_container_type<Container, Container> : std::true_type { };


//template <typename... Ts> struct is_container<std::list<Ts...> > : std::true_type { };
//template <typename... Ts> struct is_container<std::vector<Ts...> > : std::true_type { };
// ...

#endif /* CONTAINER_TYPE_TRAITS_HPP */
