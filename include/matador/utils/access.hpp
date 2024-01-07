#ifndef OOS_ACCESS_HPP
#define OOS_ACCESS_HPP

#include <string>

namespace matador {

enum class cascade_type;

template < class Type, template < class ... > class ContainerType >
class container;

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
void has_many(Operator &op, const char *id, container<Type, ContainerType> &container, cascade_type cascade) {
  op.on_has_many(id, container, cascade);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many(Operator &op, const char *id, container<Type, ContainerType> &container, const char *left_column, const char *right_column, cascade_type cascade) {
  op.on_has_many(id, container, left_column, right_column, cascade);
}

}

}

#endif //OOS_ACCESS_HPP
