#ifndef QUERY_KEY_VALUE_PAIR_HPP
#define QUERY_KEY_VALUE_PAIR_HPP

#include "matador/query/expression/column_expression.hpp"
#include "matador/query/column.hpp"

namespace matador::query::internal {

class column_value_pair {
public:
  column_value_pair() = default;
  column_value_pair(column_value_pair&& x) = default;
  column_value_pair& operator=(column_value_pair&& x) = default;
  column_value_pair(column col, column_expression_ptr expression);

  friend bool operator==(const column_value_pair &lhs, const column_value_pair &rhs);
  friend bool operator!=(const column_value_pair &lhs, const column_value_pair &rhs);

  [[nodiscard]] const column& col() const;
  [[nodiscard]] const abstract_column_expression& expression() const;

private:
  column column_;
  column_expression_ptr expression_;

};

}
#endif //QUERY_KEY_VALUE_PAIR_HPP
