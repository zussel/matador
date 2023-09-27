#ifndef MATADOR_VALUE_TYPE_TRAITS_HPP
#define MATADOR_VALUE_TYPE_TRAITS_HPP

#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_ptr.hpp"

namespace matador {

template < class Type, class Enable = void >
struct value_type_traits;

template < class Type >
struct value_type_traits<Type, typename std::enable_if<is_builtin<Type>::value>::type>
{
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
};

template < class Type >
struct value_type_traits<Type, typename std::enable_if<!is_builtin<Type>::value>::type>
{
  using value_type = object_ptr<Type>;
  using pointer = object_ptr<Type>*;
  using reference = object_ptr<Type>&;
};

}

#endif //MATADOR_VALUE_TYPE_TRAITS_HPP
