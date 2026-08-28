#ifndef MATADOR_EXPRESSION_OPERATORS_HPP
#define MATADOR_EXPRESSION_OPERATORS_HPP

#include "matador/query/expression/binary_column_expression.hpp"
#include "matador/query/expression/table_column_expression.hpp"
#include "matador/query/expression/value_expression.hpp"
#include "matador/query/expression/column_expression.hpp"
#include "matador/query/placeholder.hpp"

namespace matador::query {
template<typename Type>
column_expression operator+(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Plus,
    std::make_unique<value_expression>(std::forward<Type>(val))
)};
}

template<typename Type>
column_expression operator+(Type&& val, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Plus,
    std::make_unique<table_column_expression>(col)
  )};
}

column_expression operator+(const column& col, placeholder /*val*/);
column_expression operator+(placeholder /*val*/, const column& col);

template<typename Type>
column_expression operator-(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Minus,
    std::make_unique<value_expression>(std::forward<Type>(val))
  )};
}

template<typename Type>
column_expression operator-(Type&& val, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Minus,
    std::make_unique<table_column_expression>(col)
  )};
}

column_expression operator-(const column& col, placeholder /*val*/);
column_expression operator-(placeholder /*val*/, const column& col);

template<typename Type>
column_expression operator*(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Multiply,
    std::make_unique<value_expression>(std::forward<Type>(val))
  )};
}

template<typename Type>
column_expression operator*(Type&& val, const column& col) {
  return std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Multiply,
    std::make_unique<table_column_expression>(col)
);
}

column_expression operator*(const column& col, placeholder /*val*/);
column_expression operator*(placeholder /*val*/, const column& col);
column_expression operator*(column_expression&& left, column_expression&& right);

template<typename Type>
column_expression operator/(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Divide,
    std::make_unique<value_expression>(std::forward<Type>(val))
  )};
}

template<typename Type>
column_expression operator/(Type&& val, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Divide,
    std::make_unique<table_column_expression>(col)
  )};
}

column_expression operator/(const column& col, placeholder /*val*/);
column_expression operator/(placeholder /*val*/, const column& col);

template<typename Type>
column_expression operator%(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Modulo,
    std::make_unique<value_expression>(std::forward<Type>(val))
  )};
}

template<typename Type>
column_expression operator%(Type&& val, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Modulo,
    std::make_unique<table_column_expression>(col)
  )};
}

column_expression operator%(const column& col, placeholder /*val*/);
column_expression operator%(placeholder /*val*/, const column& col);
}
#endif //MATADOR_EXPRESSION_OPERATORS_HPP