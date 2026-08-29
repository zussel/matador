#include "matador/query/constraint.hpp"

#include "matador/query/table.hpp"

#include <stdexcept>

namespace matador::query {

namespace {

bool is_valid_constraint_kind(const column_constraint kind) {
  switch (kind) {
    case column_constraint::Index:
    case column_constraint::Unique:
    case column_constraint::PrimaryKey:
    case column_constraint::ForeignKey:
    case column_constraint::Identity:
    case column_constraint::Default:
    case column_constraint::NotNull:
      return true;
    case column_constraint::None:
      return false;
  }
  return false;
}

} // namespace

constraint::constraint(const class table& table, const std::size_t column_index, const column_constraint kind)
: table_(&table)
, column_index_(column_index)
, kind_(kind) {
  if (column_index >= table.columns().size()) {
    throw std::out_of_range("Constraint column index is out of range");
  }
  if (!is_valid_constraint_kind(kind)) {
    throw std::invalid_argument("Constraint kind must be a single declared column constraint");
  }
  if (!table.columns()[column_index].constraints().has(kind)) {
    throw std::invalid_argument("Constraint kind is not declared for the referenced column");
  }
}

column_constraint constraint::kind() const {
  return kind_;
}

const class table& constraint::table() const {
  return *table_;
}

const class column& constraint::column() const {
  return table_->columns().at(column_index_);
}

std::size_t constraint::column_index() const {
  return column_index_;
}

bool constraint::is_index_constraint() const {
  return kind_ == column_constraint::Index;
}

bool constraint::is_unique_constraint() const {
  return kind_ == column_constraint::Unique;
}

bool constraint::is_primary_key_constraint() const {
  return kind_ == column_constraint::PrimaryKey;
}

bool constraint::is_foreign_key_constraint() const {
  return kind_ == column_constraint::ForeignKey;
}

bool constraint::is_identity_constraint() const {
  return kind_ == column_constraint::Identity;
}

bool constraint::is_default_constraint() const {
  return kind_ == column_constraint::Default;
}

bool constraint::is_not_null_constraint() const {
  return kind_ == column_constraint::NotNull;
}

} // namespace matador::query
