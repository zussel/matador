//
// Created by sascha on 10.10.19.
//

#ifndef MATADOR_HAS_MANY_ITERATOR_TRAITS_HPP
#define MATADOR_HAS_MANY_ITERATOR_TRAITS_HPP

#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/has_many_item_holder.hpp"

#include <vector>

namespace matador {

/// @cond MATADOR_DEV
template < class T, template < class ... > class C, class Enable = void >
struct has_many_iterator_traits;
template < class T, template < class... > class C, class Enable = void >
struct const_has_many_iterator_traits;

/*
 * has_many_vector iterator traits
 */
template < class T >
struct has_many_iterator_traits<T, std::vector, typename std::enable_if<
  is_builtin<T>::value &&
  !std::is_convertible<T*, varchar_base*>::value
>::type>
: public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef T value_type;
  typedef T object_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<T> holder_type;
  typedef std::vector<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < class T >
struct has_many_iterator_traits<T, std::vector, typename std::enable_if<
  !is_builtin<T>::value &&
  !std::is_convertible<T*, varchar_base*>::value
>::type>
: public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef object_ptr<T> value_type;
  typedef object_ptr<T> object_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<T> holder_type;
  typedef std::vector<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < int SIZE, class T >
struct has_many_iterator_traits<varchar<SIZE, T>, std::vector>
: public std::iterator<std::random_access_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::vector> self;
  typedef varchar<SIZE, T> object_type;
  typedef typename object_type::value_type value_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<object_type> holder_type;
  typedef std::vector<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < class T >
struct const_has_many_iterator_traits<T, std::vector, typename std::enable_if<
!is_builtin<T>::value &&
!std::is_convertible<T*, varchar_base*>::value
>::type>
: public std::iterator<std::random_access_iterator_tag, T>
{
  typedef const_has_many_iterator_traits<T, std::vector> self;
  typedef object_ptr<T> value_type;
  typedef object_ptr<T> object_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<T> holder_type;
  typedef std::vector<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < class T >
struct const_has_many_iterator_traits<T, std::vector, typename std::enable_if<
  is_builtin<T>::value &&
  !std::is_convertible<T*, varchar_base*>::value
  >::type>
: public std::iterator<std::random_access_iterator_tag, T>
{
  typedef const_has_many_iterator_traits<T, std::vector> self;
  typedef T value_type;
  typedef T object_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<T> holder_type;
  typedef std::vector<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < int SIZE, class T >
struct const_has_many_iterator_traits<varchar<SIZE, T>, std::vector>
: public std::iterator<std::random_access_iterator_tag, T>
{
  typedef const_has_many_iterator_traits<T, std::vector> self;
  typedef varchar<SIZE, T> object_type;
  typedef typename object_type::value_type value_type;
  typedef typename std::iterator<std::random_access_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<object_type> holder_type;
  typedef std::vector<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

/*
 * has_many_list iterator traits
 */
template < class T >
struct has_many_iterator_traits<T, std::list, typename std::enable_if<
  is_builtin<T>::value &&
  !std::is_convertible<T*, varchar_base*>::value
  >::type>
: public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef T value_type;
  typedef T object_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<T> holder_type;
  typedef std::list<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < class T >
struct has_many_iterator_traits<T, std::list, typename std::enable_if<
  !is_builtin<T>::value &&
  !std::is_convertible<T*, varchar_base*>::value
  >::type>
: public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef object_ptr<T> value_type;
  typedef object_ptr<T> object_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<T> holder_type;
  typedef std::list<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < int SIZE, class T >
struct has_many_iterator_traits<varchar<SIZE, T>, std::list>
: public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef has_many_iterator_traits<T, std::list> self;
  typedef varchar<SIZE, T> object_type;
  typedef typename object_type::value_type value_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<object_type> holder_type;
  typedef std::list<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < class T >
struct const_has_many_iterator_traits<T, std::list, typename std::enable_if<
  !is_builtin<T>::value &&
  !std::is_convertible<T*, varchar_base*>::value
>::type>
: public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef const_has_many_iterator_traits<T, std::list> self;
  typedef object_ptr<T> value_type;
  typedef object_ptr<T> object_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<T> holder_type;
  typedef std::list<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < class T >
struct const_has_many_iterator_traits<T, std::list, typename std::enable_if<
  is_builtin<T>::value &&
  !std::is_convertible<T*, varchar_base*>::value
>::type>
: public std::iterator<std::bidirectional_iterator_tag, T>
{
  typedef const_has_many_iterator_traits<T, std::list> self;
  typedef T value_type;
  typedef T object_type;
  typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
  typedef has_many_item_holder<value_type> holder_type;
  typedef std::list<holder_type, std::allocator<holder_type>> holder_container_type;
  typedef typename holder_container_type::iterator container_iterator;
  typedef typename holder_container_type::const_iterator const_container_iterator;
};

template < int SIZE, class T >
struct const_has_many_iterator_traits<varchar<SIZE, T>, std::list>
: public std::iterator<std::bidirectional_iterator_tag, T>
{
typedef const_has_many_iterator_traits<T, std::list> self;
typedef varchar<SIZE, T> object_type;
typedef typename object_type::value_type value_type;
typedef typename std::iterator<std::bidirectional_iterator_tag, T>::difference_type difference_type;
typedef has_many_item_holder<object_type> holder_type;
typedef std::list<holder_type, std::allocator<holder_type>> holder_container_type;
typedef typename holder_container_type::iterator container_iterator;
typedef typename holder_container_type::const_iterator const_container_iterator;
};
/// @endcond

}
#endif //MATADOR_HAS_MANY_ITERATOR_TRAITS_HPP
