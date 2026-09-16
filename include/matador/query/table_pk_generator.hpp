#ifndef MATADOR_TABLE_PK_GENERATOR_HPP
#define MATADOR_TABLE_PK_GENERATOR_HPP

#include "matador/query/abstract_pk_generator.hpp"
#include "matador/query/intermediates/fetchable_query.hpp"

namespace matador::query {
class table_pk_generator : public abstract_pk_generator {
public:
    table_pk_generator(const std::string& table_name, const std::string& sequence_name);
    [[nodiscard]] utils::result<int64_t, utils::error> next_id(const executor& exec) override;
    [[nodiscard]] utils::result<int64_t, utils::error> current_id(const executor& exec) override;

private:
    fetchable_query next_id_query_;
    fetchable_query current_id_query_;
};
}
#endif //MATADOR_TABLE_PK_GENERATOR_HPP