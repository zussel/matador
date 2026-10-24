#ifndef MATADOR_IDENTITY_PK_GENERATOR_HPP
#define MATADOR_IDENTITY_PK_GENERATOR_HPP

#include "matador/query/abstract_pk_generator.hpp"

namespace matador::query {
class identity_pk_generator : public abstract_pk_generator {
public:
  identity_pk_generator();
  [[nodiscard]] result<int64_t, error> next_id(const executor& exec) override;
  [[nodiscard]] result<int64_t, error> current_id(const executor& exec) override;
};
}
#endif //MATADOR_IDENTITY_PK_GENERATOR_HPP
