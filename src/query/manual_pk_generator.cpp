#include "matador/query/manual_pk_generator.hpp"

#include "matador/query/error_code.hpp"

namespace matador::query {
manual_pk_generator::manual_pk_generator()
: abstract_pk_generator(generator_type::Manual){
}

utils::result<int64_t, utils::error> manual_pk_generator::next_id(const executor &/*exec*/) {
  return failure<error>(error(error_code::Failure, "Manual PK generator not implemented"));
}

utils::result<int64_t, utils::error> manual_pk_generator::current_id(const executor& /*exec*/) {
  return failure<error>(error(error_code::Failure, "Manual PK generator not implemented"));
}
}
