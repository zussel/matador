#ifndef MATADOR_BINARY_COLUMN_EXPRESSION_HPP
#define MATADOR_BINARY_COLUMN_EXPRESSION_HPP

#include "matador/query/expression/abstract_column_expression.hpp"

namespace matador::query {
enum class binary_expression_operator {
  Plus,
  Minus,
  Multiply,
  Divide,
  Modulo
};

class binary_column_expression : public abstract_column_expression {
public:
  binary_column_expression() = delete;
  binary_column_expression(column_expression_ptr left_column, binary_expression_operator operand, column_expression_ptr right_column);

  void accept(expression_visitor& visitor) const override;

  [[nodiscard]] const column_expression_ptr& left_column() const;
  [[nodiscard]] binary_expression_operator operand() const;
  [[nodiscard]] const column_expression_ptr& right_column() const;
private:
  column_expression_ptr left_column_;
  binary_expression_operator operand_;
  column_expression_ptr right_column_;
};
}
#endif //MATADOR_BINARY_COLUMN_EXPRESSION_HPP