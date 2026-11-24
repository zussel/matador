#include "matador/query/mapping/abstract_pk_generator.hpp"
#include "matador/query/runtime/error_code.hpp"

namespace matador::query {
abstract_pk_generator::abstract_pk_generator(const generator_type type)
: generator_type_{type} {}

generator_type abstract_pk_generator::type() const {
  return generator_type_;
}

null_pk_generator::null_pk_generator()
: abstract_pk_generator(generator_type::None){}

result<int64_t, error> null_pk_generator::next_id(const executor &/*exec*/) {
  return failure(error(error_code::Failure, "No next id generated"));
}

result<int64_t, error> null_pk_generator::current_id(const executor &/*exec*/) {
  return failure(error(error_code::Failure, "No next id generated"));
}
} // namespace matador::query
