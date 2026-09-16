#ifndef MATADOR_ABSTRACT_PK_GENERATOR_HPP
#define MATADOR_ABSTRACT_PK_GENERATOR_HPP

#include "matador/query/primary_key_generator_type.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <cstdint>

namespace matador::query {
class executor;
class abstract_pk_generator {
public:
  virtual ~abstract_pk_generator() = default;
  virtual utils::result<int64_t, utils::error> next_id(const executor& exec) = 0;
  virtual utils::result<int64_t, utils::error> current_id(const executor& exec) = 0;

  [[nodiscard]] generator_type type() const;

protected:
  explicit abstract_pk_generator(generator_type type);

private:
  generator_type generator_type_{};
};
}
#endif //MATADOR_ABSTRACT_PK_GENERATOR_HPP