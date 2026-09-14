#include "matador/query/criteria_evaluator.hpp"

#include "matador/query/criteria/between_criteria.hpp"
#include "matador/query/criteria/binary_criteria.hpp"
#include "matador/query/criteria/check_null_criteria.hpp"
#include "matador/query/criteria/collection_criteria.hpp"
#include "matador/query/criteria/like_criteria.hpp"
#include "matador/query/criteria/logical_criteria.hpp"
#include "matador/query/criteria/not_criteria.hpp"
#include "matador/query/query_utils.hpp"

#include "matador/query/dialect.hpp"
#include "matador/query/query_context.hpp"

#include "matador/utils/enum_mapper.hpp"
#include "matador/query/column_value.hpp"

namespace matador::query {
namespace detail {
static const utils::enum_mapper<binary_operator> BinaryOperatorEnum({
  {binary_operator::Equals, "="},
  {binary_operator::NotEquals, "<>"},
  {binary_operator::GreaterThan, ">"},
  {binary_operator::GreaterThanOrEqual, ">="},
  {binary_operator::LessThan, "<"},
  {binary_operator::LessThanOrEqual, "<="},
});
}

criteria_evaluator::criteria_evaluator(const dialect &d, query_context &query)
: dialect_(d)
, query_(query) {
}

std::string criteria_evaluator::evaluate(const abstract_criteria &node) {
  clause_.clear();
  node.accept(*this);

  return clause_;
}

void criteria_evaluator::visit(const between_criteria &node) {
  query_.bind_vars.emplace_back(node.col().name());
  query_.bind_vars.emplace_back(node.col().name());
  clause_ += prepare_identifier(dialect_, node.col()) + " " + dialect_.between() + " ";
  evaluate_value(node.minimum());
  clause_ += " " + dialect_.token_at(dialect_token::And) + " ";
  evaluate_value(node.maximum());
}

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

void criteria_evaluator::visit(const binary_criteria &node) {
  if (std::holds_alternative<placeholder>(node.value())) {
    query_.bind_vars.emplace_back(node.col().name());
  }
  clause_ += prepare_criteria(dialect_, node.col()) + " " + detail::BinaryOperatorEnum.to_string(node.operand()).value() + " ";

  evaluate_value(node.value());
}

void criteria_evaluator::visit( const binary_column_criteria& node ) {
  clause_ += prepare_criteria(dialect_, node.left_column()) + " " + detail::BinaryOperatorEnum.to_string(node.operand()).value() + " " + prepare_criteria(dialect_, node.right_column());
}

void criteria_evaluator::visit(const check_null_criteria &node) {
  clause_ += prepare_identifier(dialect_, node.col()) + " ";
  if (node.operand() == check_null_operator::IsNull)
    clause_ += dialect_.is_null();
  else
    clause_ += dialect_.is_not_null();
}

void criteria_evaluator::visit(const collection_criteria &node) {
  const auto count = node.values().size();
  for (size_t i = 0; i < count; ++i) {
    query_.bind_vars.emplace_back(node.col().name());
  }

  clause_ += prepare_identifier(dialect_, node.col()) +
      (node.operand() == collection_operator::Out ? " " + dialect_.token_at(dialect_token::Not) + " " : " ") +
      dialect_.token_at(dialect_token::In) + " (";
  if (node.values().size() < 2) {
    for (const auto &val: node.values()) {
      evaluate_value(val);
    }
  } else {
    auto it = node.values().begin();
    evaluate_value(*it++);
    for (; it != node.values().end(); ++it) {
      clause_ += ", ";
      evaluate_value(*it);
    }
  }
  clause_ += ')';
}

void criteria_evaluator::visit(const collection_query_criteria &node) {
  clause_ += prepare_identifier(dialect_, node.col()) +
      (node.operand() == collection_operator::Out ? " " + dialect_.not_() + " " : " ") +
      dialect_.in() + " (" + node.query().str(dialect_) + ")";
}

void criteria_evaluator::visit(const like_criteria &node) {
  clause_ += prepare_criteria(dialect_, node.col()) + " " + dialect_.like() +
      " " + dialect_.token_at(dialect_token::BeginStringData) + node.pattern() + dialect_.token_at(
        dialect_token::EndStringData);
}

void criteria_evaluator::visit(const logical_criteria &node) {
  clause_ += '(';
  node.left_clause()->accept(*this);
  clause_ += " " + dialect_.token_at(node.operand() == logical_operator::AND
                                       ? dialect_token::And
                                       : dialect_token::Or) + " ";
  node.right_clause()->accept(*this);
  clause_ += ')';
}

void criteria_evaluator::visit(const not_criteria &node) {
  clause_ += dialect_.token_at(dialect_token::Not) + " (";
  node.clause()->accept(*this);
  clause_ += ')';
}

void criteria_evaluator::evaluate_value(const criteria_value &value) {
  std::visit(overload{
        [this](const column_value& val){ clause_ += to_query_string(val, dialect_); },
        [this](const placeholder&) { clause_ += dialect_.next_placeholder(query_.bind_vars); }
    }, value);

}
}
