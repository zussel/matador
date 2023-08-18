#ifndef MATADOR_TYPE_TRAITS_HPP
#define MATADOR_TYPE_TRAITS_HPP

#include <string>

namespace matador {

template < class Type >
class object_ptr;

template<long SIZE>
class varchar;

template < typename Type >
struct type_traits
{
  using real_type = Type;
  using object_type = Type;
  using value_type = Type;
};

template < class ObjectType >
struct type_traits<object_ptr<ObjectType>>
{
  using real_type = ObjectType;
  using object_type = ObjectType;
  using value_type = object_ptr<ObjectType>;
};

template < long Size >
struct type_traits<varchar<Size>>
{
  using real_type = varchar<Size>;
  using object_type = std::string;
  using value_type = std::string;
};

}
#endif //MATADOR_TYPE_TRAITS_HPP
