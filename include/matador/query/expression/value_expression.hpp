#ifndef MATADOR_VALUE_EXPRESSION_HPP
#define MATADOR_VALUE_EXPRESSION_HPP

#include "matador/query/expression/abstract_column_expression.hpp"

#include "matador/utils/types.hpp"

namespace matador::query {
class value_expression : public abstract_column_expression {
public:
  value_expression() = delete;
  explicit value_expression(utils::database_type value);

  void accept(expression_visitor& visitor) const override;

  [[nodiscard]] const utils::database_type& value() const;

private:
  utils::database_type value_;
};
}
#endif //MATADOR_VALUE_EXPRESSION_HPP