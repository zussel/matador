#ifndef MATADOR_SEQUENCE_PK_GENERATOR_H
#define MATADOR_SEQUENCE_PK_GENERATOR_H

#include "matador/query/abstract_pk_generator.hpp"
#include "matador/query/intermediates/fetchable_query.hpp"

#include <string>

namespace matador::query {
class sequence_pk_generator : public abstract_pk_generator {
public:
    explicit sequence_pk_generator(const std::string& sequence_name);

    utils::result<int64_t, utils::error> next_id(const executor& exec) override;
    utils::result<int64_t, utils::error> current_id(const executor& exec) override;

private:
    fetchable_query next_id_query_;
    fetchable_query current_id_query_;
};
}
#endif //MATADOR_SEQUENCE_PK_GENERATOR_H