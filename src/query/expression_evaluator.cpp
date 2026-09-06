#include "matador/query/expression_evaluator.hpp"

#include "matador/query/expression/binary_column_expression.hpp"
#include "matador/query/expression/table_column_expression.hpp"
#include "matador/query/expression/placeholder_expression.hpp"
#include "matador/query/expression/value_expression.hpp"

#include "matador/query/internal/basic_type_to_string_visitor.hpp"
#include "matador/query/internal/string_builder_utils.hpp"

#include "matador/query/dialect.hpp"
#include "matador/query/query_context.hpp"

#include "matador/utils/enum_mapper.hpp"

namespace matador::query {
namespace detail {
static const utils::enum_mapper<binary_expression_operator> BinaryExpressionOperatorEnum({
  {binary_expression_operator::Plus, "+"},
  {binary_expression_operator::Minus, "-"},
  {binary_expression_operator::Multiply, "*"},
  {binary_expression_operator::Divide, "/"},
  {binary_expression_operator::Modulo, "%"},
});
}

expression_evaluator::expression_evaluator(const dialect &d, query_context &query)
: dialect_(d)
, query_(query){
}

void expression_evaluator::visit(const binary_column_expression& node) {
  expression_.append("(");
  node.left_column()->accept(*this);
  expression_.append(" ");
  expression_.append(detail::BinaryExpressionOperatorEnum.to_string(node.operand()).value());
  expression_.append(" ");
  node.right_column()->accept(*this);
  expression_.append(")");
}

void expression_evaluator::visit(const table_column_expression& node) {
  prepare_identifier_string_append(expression_, node.col().name(), dialect_);
}

void expression_evaluator::visit(const value_expression& node) {
  attribute_string_writer writer(dialect_, std::nullopt);
  internal::basic_type_to_string_visitor v(writer, query_);
  std::visit(v, node.value());
  expression_.append(v.result);
}

void expression_evaluator::visit(const placeholder_expression& /*node*/) {
  query_.bind_vars.emplace_back(std::string("value_") + std::to_string(query_.bind_vars.size() + 1));
  expression_.append(dialect_.next_placeholder(query_.bind_vars));
}

const std::string& expression_evaluator::result() const {
  return expression_;
}
}
