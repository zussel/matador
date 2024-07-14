#ifndef OOS_ACCESS_HPP
#define OOS_ACCESS_HPP

#include <string>

namespace matador {

enum class cascade_type;

template < class Type, template < class ... > class ContainerType >
class container;

class field_attributes;
class foreign_attributes;

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
void has_one(Operator &op, const char *id, Type &value, const foreign_attributes &attr) {
  op.on_has_one(id, value, attr);
}

template<class Operator, class Type>
void has_one(Operator &op, const char *id, Type &value) {
  op.on_has_one(id, value);
}

template<class Operator, class Type>
void belongs_to(Operator &op, const char *id, Type &value, const foreign_attributes &attr) {
  op.on_belongs_to(id, value, attr);
}

template<class Operator, class Type>
void belongs_to(Operator &op, const char *id, Type &value) {
  op.on_belongs_to(id, value);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many(Operator &op, const char *id, container<Type, ContainerType> &c, const char *join_column, const foreign_attributes &attr) {
  op.on_has_many(id, c, join_column, attr);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many(Operator &op, const char *id, container<Type, ContainerType> &c, const char *join_column) {
  op.on_has_many(id, c, join_column);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many(Operator &op, const char *id, container<Type, ContainerType> &c, const foreign_attributes &attr) {
  op.on_has_many(id, c, attr);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many(Operator &op, const char *id, container<Type, ContainerType> &c) {
  op.on_has_many(id, c);
}

template<class Operator, class ContainerType>
void has_many_to_many(Operator &op, const char *id, ContainerType &c, const char *join_column, const char *inverse_join_column, const foreign_attributes &attr) {
  op.on_has_many_to_many(id, c, join_column, inverse_join_column, attr);
}

template<class Operator, class ContainerType>
void has_many_to_many(Operator &op, const char *id, ContainerType &c, const char *join_column, const char *inverse_join_column) {
  op.on_has_many_to_many(id, c, join_column, inverse_join_column);
}

template<class Operator, class ContainerType>
void has_many_to_many(Operator &op, const char *id, ContainerType &c, const foreign_attributes &attr) {
  op.on_has_many_to_many(id, c, attr);
}

template<class Operator, class ContainerType>
void has_many_to_many(Operator &op, const char *id, ContainerType &c) {
  op.on_has_many_to_many(id, c);
}

}

}

#endif //OOS_ACCESS_HPP
