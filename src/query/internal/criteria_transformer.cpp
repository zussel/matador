#include "matador/query/internal/criteria_transformer.hpp"

#include "matador/query/criteria.hpp"
#include "matador/query/model/table.hpp"

namespace matador::query {
criteria_transformer::criteria_transformer(const table_map &tables_by_name)
: tables_by_name_(tables_by_name) {}

void criteria_transformer::visit(const between_criteria &node) { rebind(node); }
void criteria_transformer::visit(const binary_criteria &node) { rebind(node); }
void criteria_transformer::visit(const check_null_criteria &node) { rebind(node); }
void criteria_transformer::visit(const collection_criteria &node) { rebind(node); }
void criteria_transformer::visit(const collection_query_criteria &node) { rebind(node); }
void criteria_transformer::visit(const like_criteria &node) { rebind(node); }

void criteria_transformer::visit(const binary_column_criteria &node) {
  auto &mutable_node = const_cast<binary_column_criteria &>(node);
  rebind(mutable_node.left_column());
  rebind(mutable_node.right_column());
}

void criteria_transformer::visit(const logical_criteria &node) {
  if (node.left_clause()) {
    node.left_clause()->accept(*this);
  }
  if (node.right_clause()) {
    node.right_clause()->accept(*this);
  }
}

void criteria_transformer::visit(const not_criteria &node) {
  if (node.clause()) {
    node.clause()->accept(*this);
  }
}

void criteria_transformer::rebind(const abstract_column_criteria &node) const {
  rebind(const_cast<abstract_column_criteria &>(node).col());
}

void criteria_transformer::rebind(column &col) const {
  if (!col.is_plain_column()) {
    return;
  }
  const auto *tab = col.table();
  if (tab == nullptr) {
    return;
  }
  if (const auto it = tables_by_name_.find(tab->table_name()); it != tables_by_name_.end()) {
    col.table(it->second);
  }
}
}
