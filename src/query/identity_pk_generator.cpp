#include "matador/query/identity_pk_generator.hpp"

#include "matador/query/error_code.hpp"

namespace matador::query {
identity_pk_generator::identity_pk_generator()
: abstract_pk_generator(generator_type::Identity){
}

result<int64_t, error> identity_pk_generator::next_id(const executor &/*exec*/) {
  return failure<error>(error(error_code::Failure, "Identity PK generator not implemented"));
}

utils::result<int64_t, utils::error> identity_pk_generator::current_id(const executor &/*exec*/) {
  return failure<error>(error(error_code::Failure, "Identity PK generator not implemented"));
}
}
