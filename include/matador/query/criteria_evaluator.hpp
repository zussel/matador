#ifndef MATADOR_CRITERIA_EVALUATOR_HPP
#define MATADOR_CRITERIA_EVALUATOR_HPP

#include "matador/query/criteria/abstract_criteria.hpp"
#include "matador/query/criteria/criteria_visitor.hpp"
#include "matador/query/criteria/criteria_utils.hpp"

#include <string>

namespace matador::query {
class dialect;
struct query_context;

class criteria_evaluator final : public criteria_visitor {
public:
  criteria_evaluator(const dialect &d, query_context &query);

  std::string evaluate(const abstract_criteria &node);

  void visit(const between_criteria &node) override;
  void visit(const binary_criteria &node) override;
  void visit(const binary_column_criteria &node) override;
  void visit(const check_null_criteria &node) override;
  void visit(const collection_criteria &node) override;
  void visit(const collection_query_criteria &node) override;
  void visit(const like_criteria &node) override;
  void visit(const logical_criteria &node) override;
  void visit(const not_criteria &node) override;

private:
  void evaluate_value(const criteria_value &value);

private:
  const dialect &dialect_;
  query_context &query_;
  std::string clause_;
};
}
#endif //MATADOR_CRITERIA_EVALUATOR_HPP