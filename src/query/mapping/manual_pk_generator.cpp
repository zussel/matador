#include "matador/query/mapping/manual_pk_generator.hpp"

#include "matador/query/runtime/error_code.hpp"

namespace matador::query {
manual_pk_generator::manual_pk_generator()
: abstract_pk_generator(generator_type::Manual){
}

result<int64_t, error> manual_pk_generator::next_id(const executor &/*exec*/) {
  return failure<error>(error(error_code::Failure, "Manual PK generator not implemented"));
}

result<int64_t, error> manual_pk_generator::current_id(const executor& /*exec*/) {
  return failure<error>(error(error_code::Failure, "Manual PK generator not implemented"));
}
}
