#include "matador/query/abstract_pk_generator.hpp"

namespace matador::query {
abstract_pk_generator::abstract_pk_generator(const generator_type type)
: generator_type_{type} {}

generator_type abstract_pk_generator::type() const {
  return generator_type_;
}
}
