#ifndef MATADOR_PLACEHOLDER_EXPRESSION_HPP
#define MATADOR_PLACEHOLDER_EXPRESSION_HPP

#include "matador/query/expression/abstract_column_expression.hpp"

namespace matador::query {
class placeholder_expression : public abstract_column_expression {
public:
  placeholder_expression() = default;

  void accept(expression_visitor& visitor) const override;
};
}
#endif //MATADOR_PLACEHOLDER_EXPRESSION_HPP