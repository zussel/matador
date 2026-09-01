#ifndef MATADOR_ACCESS_HPP
#define MATADOR_ACCESS_HPP

#include "matador/query/primary_key_options.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/foreign_options.hpp"

#include <optional>

namespace matador::query::field {
template<class Operator, class Type>
void process(Operator &op, Type &object) {
  object.process(op);
}

template<class Operator, class Type>
void process(Operator &op, const Type &object) {
  process(op, const_cast<Type &>(object));
}

template<class Base, class Derived, class Operator>
void process_base(Operator &op, const Derived &object) {
  static_assert(!std::is_same_v<Base, Derived>, "class Base must not be of same type as class Derived");
  static_assert(std::is_base_of_v<Base, Derived>, "class Base must be base of class Derived");
  op.on_base(static_cast<const Base&>(object));
}

template<class Base, class Derived, class Operator>
void process_base(Operator &op, Derived &object) {
  static_assert(!std::is_same_v<Base, Derived>, "class Base must not be of same type as class Derived");
  static_assert(std::is_base_of_v<Base, Derived>, "class Base must be base of class Derived");
  op.on_base(static_cast<Base&>(object));
}

template< class Operator, class Type >
void primary_key(Operator &op, const char *id, Type &value, const primary_key_options &attr = DefaultPkAttributes) {
  op.on_primary_key(id, value, attr);
}

template<class Operator>
void revision(Operator &op, const char *id, uint64_t &value) {
  op.on_revision(id, value);
}

template<class Operator, class Type>
void attribute(Operator &op, const char *id, Type &value, const column_options &attr = NullAttributes) {
  op.on_attribute(id, value, attr);
}

template<class Operator, class Type>
void attribute(Operator &op, const char *id, std::optional<Type> &value, const column_options &attr = NullAttributes) {
  op.on_attribute(id, value, attr);
}

template<class Operator, class Type>
void belongs_to(Operator &op, const char *id, Type &value, const foreign_options &attr = CascadeNoneFetchLazy) {
  op.on_belongs_to(id, value, attr);
}

template<class Operator, class Type>
void has_one(Operator &op, const char *id, Type &value, const char *join_column, const foreign_options &attr = CascadeNoneFetchLazy) {
  op.on_has_one(id, value, join_column, attr);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many(Operator &op, const char *id, ContainerType<Type> &c, const char *join_column, const foreign_options &attr = CascadeNoneFetchLazy) {
  op.on_has_many(id, c, join_column, attr);
}

template<class Operator, class Type, template<class ...> class ContainerType>
void has_many(Operator &op, const char *id, ContainerType<Type> &c, const foreign_options &attr = CascadeNoneFetchLazy) {
  op.on_has_many(id, c, attr);
}

template<class Operator, class ContainerType>
void has_many_to_many(Operator &op, const char *id, ContainerType &c, const char *join_column, const char *inverse_join_column, const foreign_options &attr = CascadeNoneFetchLazy) {
  op.on_has_many_to_many(id, c, join_column, inverse_join_column, attr);
}

template<class Operator, class ContainerType>
void has_many_to_many(Operator &op, const char *id, ContainerType &c, const foreign_options &attr = CascadeNoneFetchLazy) {
  op.on_has_many_to_many(id, c, attr);
}
}

#endif //MATADOR_ACCESS_HPP
