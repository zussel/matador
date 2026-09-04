#ifndef MATADOR_EXPRESSION_OPERATORS_HPP
#define MATADOR_EXPRESSION_OPERATORS_HPP

#include "matador/query/expression/binary_column_expression.hpp"
#include "matador/query/expression/table_column_expression.hpp"
#include "matador/query/expression/value_expression.hpp"
#include "matador/query/expression/column_expression.hpp"
#include "matador/query/placeholder.hpp"

/**
 * @file expression_operators.hpp
 * @brief Global operator declarations for all derived types of abstract_column_expression.
 */

namespace matador::query {

/**
 * @brief Creates an addition expression with a column on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * column + value
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto price = "price"_col;
 * auto expr = price + 10;
 * // Represents: price + 10
 * @endcode
 *
 * @tparam Type Type of the value to add to the column.
 * @param col Column used as the left-hand operand.
 * @param val Value used as the right-hand operand.
 * @return A column expression representing the addition.
 */
template<typename Type>
column_expression operator+(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Plus,
    std::make_unique<value_expression>(std::forward<Type>(val))
)};
}

/**
 * @brief Creates an addition expression with a value on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * value + column
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto amount = "amount"_col;
 * auto expr = 5 + amount;
 * // Represents: 5 + amount
 * @endcode
 *
 * @tparam Type Type of the value to add to the column.
 * @param val Value used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the addition.
 */
template<typename Type>
column_expression operator+(Type&& val, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Plus,
    std::make_unique<table_column_expression>(col)
  )};
}

/**
 * @brief Creates an addition expression between a column and a placeholder.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto price = "price"_col;
 * auto expr = price + placeholder{};
 * // Represents: price + ?
 * @endcode
 *
 * @param col Column used as the left-hand operand.
 * @param val Placeholder used as the right-hand operand.
 * @return A column expression representing the addition.
 */
column_expression operator+(const column& col, placeholder val);

/**
 * @brief Creates an addition expression between a placeholder and a column.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto amount = "amount"_col;
 * auto expr = placeholder{} + amount;
 * // Represents: ? + amount
 * @endcode
 *
 * @param val Placeholder used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the addition.
 */
column_expression operator+(placeholder val, const column& col);

/**
 * @brief Creates a subtraction expression with a column on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * column - value
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto stock = "stock"_col;
 * auto expr = stock - 1;
 * // Represents: stock - 1
 * @endcode
 *
 * @tparam Type Type of the value to subtract from the column.
 * @param col Column used as the left-hand operand.
 * @param val Value used as the right-hand operand.
 * @return A column expression representing the subtraction.
 */
template<typename Type>
column_expression operator-(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Minus,
    std::make_unique<value_expression>(std::forward<Type>(val))
  )};
}

/**
 * @brief Creates a subtraction expression with a value on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * value - column
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto discount = "discount"_col;
 * auto expr = 100 - discount;
 * // Represents: 100 - discount
 * @endcode
 *
 * @tparam Type Type of the value used as the left-hand operand.
 * @param val Value used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the subtraction.
 */
template<typename Type>
column_expression operator-(Type&& val, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Minus,
    std::make_unique<table_column_expression>(col)
  )};
}

/**
 * @brief Creates a subtraction expression between a column and a placeholder.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto stock = "stock"_col;
 * auto expr = stock - placeholder{};
 * // Represents: stock - ?
 * @endcode
 *
 * @param col Column used as the left-hand operand.
 * @param val Placeholder used as the right-hand operand.
 * @return A column expression representing the subtraction.
 */
column_expression operator-(const column& col, placeholder val);

/**
 * @brief Creates a subtraction expression between a placeholder and a column.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto discount = "discount"_col;
 * auto expr = placeholder{} - discount;
 * // Represents: ? - discount
 * @endcode
 *
 * @param val Placeholder used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the subtraction.
 */
column_expression operator-(placeholder val, const column& col);

/**
 * @brief Creates a multiplication expression with a column on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * column * value
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto price = "price"_col;
 * auto expr = price * 2;
 * // Represents: price * 2
 * @endcode
 *
 * @tparam Type Type of the value to multiply with the column.
 * @param col Column used as the left-hand operand.
 * @param val Value used as the right-hand operand.
 * @return A column expression representing the multiplication.
 */
template<typename Type>
column_expression operator*(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Multiply,
    std::make_unique<value_expression>(std::forward<Type>(val))
  )};
}

/**
 * @brief Creates a multiplication expression with a value on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * value * column
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto quantity = "quantity"_col;
 * auto expr = 3 * quantity;
 * // Represents: 3 * quantity
 * @endcode
 *
 * @tparam Type Type of the value to multiply with the column.
 * @param val Value used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the multiplication.
 */
template<typename Type>
column_expression operator*(Type&& val, const column& col) {
  return std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Multiply,
    std::make_unique<table_column_expression>(col)
);
}

/**
 * @brief Creates a multiplication expression between a column and a placeholder.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto price = "price"_col;
 * auto expr = price * placeholder{};
 * // Represents: price * ?
 * @endcode
 *
 * @param col Column used as the left-hand operand.
 * @param val Placeholder used as the right-hand operand.
 * @return A column expression representing the multiplication.
 */
column_expression operator*(const column& col, placeholder val);

/**
 * @brief Creates a multiplication expression between a placeholder and a column.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto quantity = "quantity"_col;
 * auto expr = placeholder{} * quantity;
 * // Represents: ? * quantity
 * @endcode
 *
 * @param val Placeholder used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the multiplication.
 */
column_expression operator*(placeholder val, const column& col);

/**
 * @brief Creates a multiplication expression from two existing column expressions.
 *
 * This overload allows combining already constructed expressions.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto price = "price"_col;
 * auto tax = "tax"_col;
 *
 * auto expr = (price + 10) * (tax + 1);
 * // Represents: (price + 10) * (tax + 1)
 * @endcode
 *
 * @param left Left-hand column expression.
 * @param right Right-hand column expression.
 * @return A column expression representing the multiplication of both expressions.
 */
column_expression operator*(column_expression&& left, column_expression&& right);

/**
 * @fn column_expression operator/(const column& col, Type&& val)
 * @brief Creates a division expression with a column on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * column / value
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto total = "total"_col;
 * auto expr = total / 2;
 * // Represents: total / 2
 * @endcode
 *
 * @tparam Type Type of the divisor value.
 * @param col Column used as the left-hand operand.
 * @param val Value used as the right-hand operand.
 * @return A column expression representing the division.
 */
template<typename Type>
column_expression operator/(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Divide,
    std::make_unique<value_expression>(std::forward<Type>(val))
  )};
}

/**
 * @brief Creates a division expression with a value on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * value / column
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto divisor = "divisor"_col;
 * auto expr = 100 / divisor;
 * // Represents: 100 / divisor
 * @endcode
 *
 * @tparam Type Type of the dividend value.
 * @param val Value used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the division.
 */
template<typename Type>
column_expression operator/(Type&& val, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Divide,
    std::make_unique<table_column_expression>(col)
  )};
}

/**
 * @brief Creates a division expression between a column and a placeholder.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto total = "total"_col;
 * auto expr = total / placeholder{};
 * // Represents: total / ?
 * @endcode
 *
 * @param col Column used as the left-hand operand.
 * @param val Placeholder used as the right-hand operand.
 * @return A column expression representing the division.
 */
column_expression operator/(const column& col, placeholder val);

/**
 * @brief Creates a division expression between a placeholder and a column.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto divisor = "divisor"_col;
 * auto expr = placeholder{} / divisor;
 * // Represents: ? / divisor
 * @endcode
 *
 * @param val Placeholder used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the division.
 */
column_expression operator/(placeholder val, const column& col);

/**
 * @brief Creates a modulo expression with a column on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * column % value
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto id = "id"_col;
 * auto expr = id % 2;
 * // Represents: id % 2
 * @endcode
 *
 * @tparam Type Type of the divisor value.
 * @param col Column used as the left-hand operand.
 * @param val Value used as the right-hand operand.
 * @return A column expression representing the modulo operation.
 */
template<typename Type>
column_expression operator%(const column& col, Type&& val) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<table_column_expression>(col),
    binary_expression_operator::Modulo,
    std::make_unique<value_expression>(std::forward<Type>(val))
  )};
}

/**
 * @brief Creates a modulo expression with a value on the left-hand side.
 *
 * Builds a binary column expression representing:
 *
 * @code{.cpp}
 * value % column
 * @endcode
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto divisor = "divisor"_col;
 * auto expr = 10 % divisor;
 * // Represents: 10 % divisor
 * @endcode
 *
 * @tparam Type Type of the value used as the left-hand operand.
 * @param val Value used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the modulo operation.
 */
template<typename Type>
column_expression operator%(Type&& val, const column& col) {
  return column_expression{std::make_unique<binary_column_expression>(
    std::make_unique<value_expression>(std::forward<Type>(val)),
    binary_expression_operator::Modulo,
    std::make_unique<table_column_expression>(col)
  )};
}

/**
 * @brief Creates a modulo expression between a column and a placeholder.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto id = "id"_col;
 * auto expr = id % placeholder{};
 * // Represents: id % ?
 * @endcode
 *
 * @param col Column used as the left-hand operand.
 * @param val Placeholder used as the right-hand operand.
 * @return A column expression representing the modulo operation.
 */
column_expression operator%(const column& col, placeholder val);

/**
 * @brief Creates a modulo expression between a placeholder and a column.
 *
 * Example:
 *
 * @code{.cpp}
 * using namespace matador::query;
 *
 * auto divisor = "divisor"_col;
 * auto expr = placeholder{} % divisor;
 * // Represents: ? % divisor
 * @endcode
 *
 * @param val Placeholder used as the left-hand operand.
 * @param col Column used as the right-hand operand.
 * @return A column expression representing the modulo operation.
 */
column_expression operator%(placeholder val, const column& col);

} // namespace matador::query

#endif //MATADOR_EXPRESSION_OPERATORS_HPP