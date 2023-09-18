#ifndef OOS_ACCESS_HPP
#define OOS_ACCESS_HPP

#include <string>

namespace matador {

enum class cascade_type;

template < class Type, template < class ... > class ContainerType >
class has_many;

class field_attributes;

namespace access {
template<class Operator, class Type>
void process(Operator &op, Type &object) {
  object.process(op);
}

template<class Operator, class Type>
void process(Operator &op, const Type &object) {
  const_cast<Type &>(object).process(op);
}

template< class Operator, class Type >
void primary_key(Operator &op, const char *id, Type &value) {
  op.on_primary_key(id, value);
}

template< class Operator >
void primary_key(Operator &op, const char *id, std::string &value, size_t size ) {
  op.on_primary_key(id, value, size);
}

template<class Operator>
void revision(Operator &op, const char *id, unsigned long long &value) {
  op.on_revision(id, value);
}

template<class Operator, class Type>
void attribute(Operator &op, const char *id, Type &value, const field_attributes &attr) {
  op.on_attribute(id, value, attr);
}

template<class Operator, class Type>
void attribute(Operator &op, const char *id, Type &value) {
  op.on_attribute(id, value);
}

template<class Operator, class Type>
void has_one(Operator &op, const char *id, Type &value, cascade_type cascade) {
  op.on_has_one(id, value, cascade);
}

template<class Operator, class Type>
void belongs_to(Operator &op, const char *id, Type &value, cascade_type cascade) {
  op.on_belongs_to(id, value, cascade);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many_(Operator &op, const char *id, has_many<Type, ContainerType> &container, cascade_type cascade) {
  op.on_has_many(id, container, cascade);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many_(Operator &op, const char *id, has_many<Type, ContainerType> &container, const char *left_column, const char *right_column, cascade_type cascade) {
  op.on_has_many(id, container, left_column, right_column, cascade);
}

}
/**
 * @class access
 * @brief Give access to the private members to a serializer
 */
//class access
//{
//public:
  /**
   * @fn void serialize(S &s, T &t)
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
//  template<class S, class T>
//  friend inline void serialize(S &, T &) {}

  /**
   * @fn void serialize(S &s, const T &t)
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
//  template<class S, class T>
//  friend inline void serialize(S &, const T &) {}

  /**
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
//  template<class S, class T>
//  static void serialize(S &s, T &t) {
//    t.serialize(s);
//  }

  /**
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
//  template<class S, class T>
//  static void serialize(S &s, const T &t) {
//    const_cast<T &>(t).serialize(s);
//  }
//};

}

#endif //OOS_ACCESS_HPP
