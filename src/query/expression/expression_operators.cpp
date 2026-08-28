#include "matador/query/expression/expression_operators.hpp"
#include "matador/query/expression/placeholder_expression.hpp"

namespace matador::query {
column_expression operator+(const column &col, placeholder /*placeholder*/) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Plus,
    std::make_unique<placeholder_expression>()
  )};
}

column_expression operator+(placeholder /*placeholder*/, const column &col) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<placeholder_expression>(),
      binary_expression_operator::Plus,
      std::make_unique<table_column_expression>(col)
  )};
}

column_expression operator-(const column &col, placeholder /*placeholder*/) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<table_column_expression>(col),
      binary_expression_operator::Minus,
      std::make_unique<placeholder_expression>()
    )};
}

column_expression operator-(placeholder /*placeholder*/, const column &col) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<placeholder_expression>(),
      binary_expression_operator::Minus,
      std::make_unique<table_column_expression>(col)
    )};
}

column_expression operator*(const column& col, placeholder /*placeholder*/) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<table_column_expression>(col),
      binary_expression_operator::Multiply,
      std::make_unique<placeholder_expression>()
    )};
}

column_expression operator*(placeholder /*placeholder*/, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<placeholder_expression>(),
      binary_expression_operator::Multiply,
      std::make_unique<table_column_expression>(col)
    )};
}

column_expression operator*(column_expression&& left, column_expression&& right) {
    return column_expression{std::make_unique<binary_column_expression>(
        left.release(), binary_expression_operator::Multiply, right.release()
    )};
}

column_expression operator/(const column& col, placeholder /*placeholder*/) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<table_column_expression>(col),
      binary_expression_operator::Divide,
      std::make_unique<placeholder_expression>()
      )};
}

column_expression operator/(placeholder /*placeholder*/, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<placeholder_expression>(),
      binary_expression_operator::Divide,
      std::make_unique<table_column_expression>(col)
    )};
}

column_expression operator%(const column& col, placeholder /*placeholder*/) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<table_column_expression>(col),
      binary_expression_operator::Modulo,
      std::make_unique<placeholder_expression>()
    )};
}

column_expression operator%(placeholder /*placeholder*/, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
      std::make_unique<placeholder_expression>(),
      binary_expression_operator::Modulo,
      std::make_unique<table_column_expression>(col)
    )};
}
}