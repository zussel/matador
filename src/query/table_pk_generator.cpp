#include "matador/query/table_pk_generator.hpp"
#include "matador/query/query.hpp"
#include "matador/query/criteria.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/expression/expression_operators.hpp"

namespace matador::query {
table_pk_generator::table_pk_generator(const std::string& table_name, const std::string &sequence_name)
: abstract_pk_generator(generator_type::Table)
, next_id_query_(query::update(table{table_name})
  .set("next_id"_col, "next_id"_col + 1)
  .where("name"_col == sequence_name)
  .returning(("next_id"_col - 1).as("id")))
, current_id_query_(query::select({"next_id"_col})
  .from(table{table_name})
  .where("name"_col == sequence_name)
) {}

utils::result<int64_t, utils::error> table_pk_generator::next_id(const executor &exec) {
    return next_id_query_.fetch_value<int64_t>(exec).and_then([](const std::optional<int64_t> id) -> utils::result<int64_t, utils::error> {
        if (!id) {
            return utils::failure(utils::error(error_code::RetrieveDataFailed, "Sequence returned no value"));
        }
        return utils::ok(*id);
    });
}

utils::result<int64_t, utils::error> table_pk_generator::current_id(const executor& exec) {
    return current_id_query_.fetch_value<int64_t>(exec).and_then([](const std::optional<int64_t> id) -> utils::result<int64_t, utils::error> {
        if (!id) {
            return utils::failure(utils::error(error_code::RetrieveDataFailed, "Sequence returned no value"));
        }
        return utils::ok(*id);
    });
}
}
