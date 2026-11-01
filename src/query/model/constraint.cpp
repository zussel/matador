#include "matador/query/model/constraint.hpp"

#include "matador/query/model/table.hpp"

#include <stdexcept>
#include <utility>

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

constraint constraint::make_column_constraint(const query::table &table,
                                              const std::size_t column_index,
                                              const column_constraint kind) {
  if (column_index >= table.columns().size()) {
    throw std::out_of_range("Column index is out of range");
  }
  if (!table.columns()[column_index].constraints().has(kind)) {
    throw std::invalid_argument("Constraint kind is not declared for the referenced column");
  }
  return make_column_constraint(table.table_name(), table.find_column(column_index)->column_name(), kind);
}

constraint constraint::make_column_constraint(
  std::string name,
  std::string table_name,
  std::string column_name,
  const column_constraint kind
) {
  if (!is_valid_constraint_kind(kind)) {
    throw std::invalid_argument("Constraint kind must be a single declared column constraint");
  }

  return constraint{std::move(name), {std::move(table_name), std::move(column_name)}, kind};
}

constraint constraint::make_column_constraint(std::string table_name,
                                              std::string column_name,
                                              const column_constraint kind) {
  if (!is_valid_constraint_kind(kind)) {
    throw std::invalid_argument("Constraint kind must be a single declared column constraint");
  }

  return constraint{std::nullopt, {std::move(table_name), std::move(column_name)}, kind};
}

constraint constraint::make_fk_constraint(
  const query::table& table,
  const std::size_t column_index,
  const class table& ref_table
) {
  if (column_index >= table.columns().size()) {
    throw std::out_of_range("Column index is out of range");
  }

  if (!ref_table.primary_key_column()) {
    throw std::invalid_argument("Referenced table must have a primary key");
  }

  return make_fk_constraint(table, column_index, ref_table, ref_table.primary_key_column()->index());
}

constraint constraint::make_fk_constraint(
  const query::table& table,
  const std::size_t column_index,
  const class table& ref_table,
  const std::size_t ref_column_index
) {
  return constraint{
    std::nullopt,
    {table.table_name(), table.find_column(column_index)->column_name()},
    column_constraint::ForeignKey,
    column_ref{ref_table.table_name(), ref_table.find_column(ref_column_index)->column_name()}
  };
}

constraint constraint::make_fk_constraint(
  std::string name,
  std::string table_name,
  std::string column_name,
  std::string ref_table_name,
  std::string ref_column_name
) {
  return constraint{
    std::move(name),
    {std::move(table_name), std::move(column_name)},
    column_constraint::ForeignKey,
    column_ref{std::move(ref_table_name), std::move(ref_column_name)}
  };
}

constraint::constraint(std::optional<std::string> name, column_ref column, const column_constraint kind, std::optional<column_ref> ref_column)
: name_(std::move(name))
, column_(std::move(column))
, ref_column_(std::move(ref_column))
, kind_(kind) {
}

bool constraint::has_name() const {
  return name_.has_value();
}

std::string constraint::name() const {
  return name_.value_or("");
}

column_constraint constraint::kind() const {
  return kind_;
}

std::string constraint::table_name() const {
  return column_.table_name;
}

std::string constraint::column_name() const {
  return column_.column_name;
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

bool constraint::has_ref_table() const {
  return ref_column_.has_value();
}

std::string constraint::ref_table_name() const {
  return ref_column_.value_or({}).table_name;
}

std::string constraint::ref_column_name() const {
  return ref_column_.value_or({}).column_name;
}

void constraint::rebind_table(const class table& table) {
  // if (auto* ref = std::get_if<indexed_column_ref>(&column_)) {
    // ref->table = &table;
  // }
}
} // namespace matador::query
