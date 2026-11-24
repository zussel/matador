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
    std::make_shared<table_ref>(ref_table, ref_column_index)
  };
}

constraint constraint::make_fk_constraint(
  std::string name,
  std::string table_name,
  std::string column_name,
  const std::string& ref_table_name,
  const std::string& ref_column_name
) {
  return constraint{
    std::move(name),
    {std::move(table_name), std::move(column_name)},
    column_constraint::ForeignKey,
    std::make_shared<plain_ref>(ref_table_name, ref_column_name)
  };
}

constraint::plain_ref::plain_ref(std::string tn, std::string cn)
: table_name_(std::move(tn)), column_name_(std::move(cn)) {}

std::string constraint::plain_ref::table_name() const {
  return table_name_;
}
std::string constraint::plain_ref::column_name() const {
  return column_name_;
}
constraint::table_ref::table_ref(const table &t, size_t index)
: table_(t), pk_index_(index) {}
std::string constraint::table_ref::table_name() const {
  return table_.table_name();
}
std::string constraint::table_ref::column_name() const {
  return table_.find_column(pk_index_)->column_name();
}

constraint::constraint(std::optional<std::string> name, column_ref column, const column_constraint kind, std::shared_ptr<abstract_ref> ref)
: name_(std::move(name))
, column_(std::move(column))
, ref_(std::move(ref))
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
  return ref_ != nullptr;
}

std::string constraint::ref_table_name() const {
  return ref_ != nullptr ? ref_->table_name() : "";
}

std::string constraint::ref_column_name() const {
  return ref_ != nullptr ? ref_->column_name() : "";
}

} // namespace matador::query
