#ifndef MATADOR_IS_NULL_CRITERIA_HPP
#define MATADOR_IS_NULL_CRITERIA_HPP

#include "matador/query/criteria/abstract_column_criteria.hpp"

namespace matador::query {
enum class check_null_operator {
  IsNull,
  IsNotNull,
};

class check_null_criteria final : public abstract_column_criteria {
public:
  check_null_criteria() = delete;
  check_null_criteria(const column& col, check_null_operator op);

  void accept(criteria_visitor& visitor) const override;

  [[nodiscard]] check_null_operator operand() const;

private:
  check_null_operator operator_{};
};

}
#endif  // MATADOR_IS_NULL_CRITERIA_HPP
