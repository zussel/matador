#ifndef MATADOR_IDENTITY_PK_GENERATOR_HPP
#define MATADOR_IDENTITY_PK_GENERATOR_HPP

#include "matador/query/abstract_pk_generator.hpp"

namespace matador::query {
class identity_pk_generator : public abstract_pk_generator {
public:
  identity_pk_generator();
  [[nodiscard]] utils::result<int64_t, utils::error> next_id(const executor& exec) override;
  [[nodiscard]] utils::result<int64_t, utils::error> current_id(const executor& exec) override;
};
}
#endif //MATADOR_IDENTITY_PK_GENERATOR_HPP
