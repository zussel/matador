#include "matador/query/sequence_pk_generator.hpp"

#include "matador/query/query.hpp"
#include "matador/query/error_code.hpp"

namespace matador::query {
sequence_pk_generator::sequence_pk_generator(const std::string& sequence_name)
: abstract_pk_generator(generator_type::Sequence)
, next_id_query_(query::select().nextval(sequence_name))
, current_id_query_(query::select().currval(sequence_name)) {
}

utils::result<int64_t, utils::error> sequence_pk_generator::next_id(const executor& exec) {
    return next_id_query_.fetch_value<int64_t>(exec).and_then([](const std::optional<int64_t> id) -> utils::result<int64_t, utils::error> {
        if (!id) {
            return failure<error>(error(error_code::RetrieveDataFailed, "Sequence returned no value"));
        }
        return utils::ok(*id);
    });
}

utils::result<int64_t, utils::error> sequence_pk_generator::current_id(const executor& exec) {
    return current_id_query_.fetch_value<int64_t>(exec).and_then([](const std::optional<int64_t> id) -> utils::result<int64_t, utils::error> {
        if (!id) {
            return failure<error>(error(error_code::RetrieveDataFailed, "Sequence returned no value"));
        }
        return utils::ok(*id);
    });
}
}
