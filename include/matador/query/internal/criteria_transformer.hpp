#ifndef MATADOR_CRITERIA_TRANSFORMER_HPP
#define MATADOR_CRITERIA_TRANSFORMER_HPP

#include "matador/query/criteria/criteria_visitor.hpp"

#include <string>
#include <unordered_map>

namespace matador::query {
class column;
class table;
class abstract_column_criteria;

/**
 * Rebinds all columns of a criteria tree to the aliased tables
 * which were created while building a select query.
 *
 * The tree is visited through const nodes, but it is owned by
 * the caller, who hands over a mutable tree to be transformed.
 */
class criteria_transformer final : public criteria_visitor {
public:
  using table_map = std::unordered_map<std::string, const table *>;

  explicit criteria_transformer(const table_map &tables_by_name);

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
  void rebind(column &col) const;
  void rebind(const abstract_column_criteria &node) const;

private:
  const table_map &tables_by_name_;
};
}

#endif //MATADOR_CRITERIA_TRANSFORMER_HPP
