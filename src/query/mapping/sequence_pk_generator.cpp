#include "matador/query/mapping/sequence_pk_generator.hpp"

#include "matador/query/builder/query.hpp"
#include "matador/query/runtime/error_code.hpp"

namespace matador::query {
sequence_pk_generator::sequence_pk_generator(const std::string& sequence_name)
: abstract_pk_generator(generator_type::Sequence)
, next_id_query_(query::select().nextval(sequence_name))
, current_id_query_(query::select().currval(sequence_name)) {
}

result<int64_t, error> sequence_pk_generator::next_id(const executor& exec) {
    return next_id_query_.fetch_value<int64_t>(exec).and_then([](const std::optional<int64_t> id) -> result<int64_t, error> {
        if (!id) {
            return failure<error>(error(error_code::RetrieveDataFailed, "Sequence returned no value"));
        }
        return ok(*id);
    });
}

result<int64_t, error> sequence_pk_generator::current_id(const executor& exec) {
    return current_id_query_.fetch_value<int64_t>(exec).and_then([](const std::optional<int64_t> id) -> result<int64_t, error> {
        if (!id) {
            return failure<error>(error(error_code::RetrieveDataFailed, "Sequence returned no value"));
        }
        return ok(*id);
    });
}
}
