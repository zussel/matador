#ifndef MATADOR_TABLE_COLUMN_EXPRESSION_HPP
#define MATADOR_TABLE_COLUMN_EXPRESSION_HPP

#include "matador/query/expression/abstract_column_expression.hpp"
#include "matador/query/column.hpp"

namespace matador::query {
class table_column_expression : public abstract_column_expression {
public:
  table_column_expression() = delete;
  explicit table_column_expression(column col);

  void accept(expression_visitor& visitor) const override;

  [[nodiscard]] const column& col() const;

private:
  column column_;
};
}
#endif //MATADOR_TABLE_COLUMN_EXPRESSION_HPP