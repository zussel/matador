#ifndef OBJECT_CONVERT_HPP
#define OBJECT_CONVERT_HPP

#include "object/object.hpp"
#include "object/object_ptr.hpp"

#include <iostream>
#include <typeinfo>

namespace oos {

template < class T >
void
convert(const object_ptr<T> &from, object_ptr<T> &to)
{
  std::cout << "1 typeid: " << typeid(T).name() << "\n";
  to = from;
}

void
convert(const object_base_ptr &from, object_ptr<object> &to);

template < class T >
void
convert(const object_ref<T> &from, object_ptr<T> &to)
{
  std::cout << "2 typeid: " << typeid(T).name() << "\n";
  to = from;
}

template < class T >
void
convert(const object_ptr<T> &from, object_ref<T> &to)
{
  std::cout << "3 typeid: " << typeid(T).name() << "\n";
  to = from;
}

template < class T >
void
convert(const T &from, object_base_ptr &to)
{
  std::cout << "4 typeid: " << typeid(T).name() << "\n";
}

template < class T >
void
convert(const object_base_ptr &from, T &to)
{
  std::cout << "5 typeid: " << typeid(T).name() << "\n";
}

}

#endif /* OBJECT_CONVERT_HPP */
