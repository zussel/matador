#include "matador/query/model/constraint.hpp"

#include "matador/query/model/table.hpp"

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

constraint constraint::make_column_constraint(
  const query::table& table,
  const std::size_t column_index,
  const column_constraint kind
) {
  return constraint{indexed_column_ref{&table, column_index}, kind};
}

constraint constraint::make_column_constraint(
  std::string table_name,
  std::string column_name,
  const column_constraint kind
) {
  return constraint{
    named_column_ref{std::move(table_name), std::move(column_name)},
    kind
  };
}
constraint constraint::make_fk_constraint(
  const query::table& table,
  const std::size_t column_index,
  const class table& ref_table
) {
  return make_fk_constraint(table, column_index, ref_table, 0);
}

constraint constraint::make_fk_constraint(
  const query::table& table,
  const std::size_t column_index,
  const class table& ref_table,
  const std::size_t ref_column_index
) {
  return constraint{
    indexed_column_ref{&table, column_index},
    column_constraint::ForeignKey,
    indexed_column_ref{&ref_table, ref_column_index}
  };
}

constraint constraint::make_fk_constraint(
  std::string table_name,
  std::string column_name,
  std::string ref_table_name,
  std::string ref_column_name
) {
  return constraint{
    named_column_ref{std::move(table_name), std::move(column_name)},
    column_constraint::ForeignKey,
    named_column_ref{std::move(ref_table_name), std::move(ref_column_name)}
  };
}

constraint::constraint(column_ref column, const column_constraint kind, optional_column_ref ref_column)
: column_(std::move(column))
, ref_column_(std::move(ref_column))
, kind_(kind) {
  if (!is_valid_constraint_kind(kind)) {
    throw std::invalid_argument("Constraint kind must be a single declared column constraint");
  }

  std::visit([kind](const auto& ref) {
    using ref_type = std::decay_t<decltype(ref)>;

    if constexpr (std::is_same_v<ref_type, indexed_column_ref>) {
      validate_indexed_column_ref(ref, kind);
    } else if constexpr (std::is_same_v<ref_type, named_column_ref>) {
      validate_named_column_ref(ref);
    }
  }, column_);

  if (kind != column_constraint::ForeignKey && !std::holds_alternative<std::monostate>(ref_column_)) {
    throw std::invalid_argument("Only foreign key constraints may reference another table");
  }

  std::visit([](const auto& ref) {
    using ref_type = std::decay_t<decltype(ref)>;

    if constexpr (std::is_same_v<ref_type, indexed_column_ref>) {
      if (ref.table == nullptr) {
        throw std::invalid_argument("Referenced constraint table must not be null");
      }

      if (ref.column_index >= ref.table->columns().size()) {
        throw std::out_of_range("Referenced constraint column index is out of range");
      }
    } else if constexpr (std::is_same_v<ref_type, named_column_ref>) {
      validate_named_column_ref(ref);
    }
  }, ref_column_);
}

column_constraint constraint::kind() const {
  return kind_;
}

std::string constraint::table_name() const {
  return std::visit([](const auto& ref) -> std::string {
    using ref_type = std::decay_t<decltype(ref)>;

    if constexpr (std::is_same_v<ref_type, indexed_column_ref>) {
      return ref.table->name();
    } else {
      return ref.table_name;
    }
  }, column_);
}

std::string constraint::column_name() const {
  return std::visit([](const auto& ref) -> std::string {
    using ref_type = std::decay_t<decltype(ref)>;

    if constexpr (std::is_same_v<ref_type, indexed_column_ref>) {
      return ref.table->columns().at(ref.column_index).column_name();
    } else {
      return ref.column_name;
    }
  }, column_);
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
  return !std::holds_alternative<std::monostate>(ref_column_);
}

std::string constraint::ref_table_name() const {
  return std::visit([](const auto& ref) -> std::string {
    using ref_type = std::decay_t<decltype(ref)>;

    if constexpr (std::is_same_v<ref_type, std::monostate>) {
      return {};
    } else if constexpr (std::is_same_v<ref_type, indexed_column_ref>) {
      return ref.table->name();
    } else {
      return ref.table_name;
    }
  }, ref_column_);
}

std::string constraint::ref_column_name() const {
  return std::visit([](const auto& ref) -> std::string {
    using ref_type = std::decay_t<decltype(ref)>;

    if constexpr (std::is_same_v<ref_type, std::monostate>) {
      return {};
    } else if constexpr (std::is_same_v<ref_type, indexed_column_ref>) {
      return ref.table->columns().at(ref.column_index).column_name();
    } else {
      return ref.column_name;
    }
  }, ref_column_);
}

const constraint::indexed_column_ref& constraint::as_indexed_ref(const column_ref& ref) {
  const auto* indexed = std::get_if<indexed_column_ref>(&ref);
  if (indexed == nullptr) {
    throw std::logic_error("Constraint is not bound to a table object");
  }
  return *indexed;
}

const constraint::indexed_column_ref& constraint::as_indexed_ref(const optional_column_ref& ref) {
  const auto* indexed = std::get_if<indexed_column_ref>(&ref);
  if (indexed == nullptr) {
    throw std::logic_error("Constraint reference is not bound to a table object");
  }
  return *indexed;
}

void constraint::validate_indexed_column_ref(const indexed_column_ref& ref, const column_constraint kind) {
  if (ref.table == nullptr) {
    throw std::invalid_argument("Constraint table must not be null");
  }

  if (ref.column_index >= ref.table->columns().size()) {
    throw std::out_of_range("Constraint column index is out of range");
  }

  if (!ref.table->columns()[ref.column_index].constraints().has(kind)) {
    throw std::invalid_argument("Constraint kind is not declared for the referenced column");
  }
}

void constraint::validate_named_column_ref(const named_column_ref& ref) {
  if (ref.table_name.empty()) {
    throw std::invalid_argument("Constraint table name must not be empty");
  }

  if (ref.column_name.empty()) {
    throw std::invalid_argument("Constraint column name must not be empty");
  }
}

void constraint::rebind_table(const class table& table) {
  if (auto* ref = std::get_if<indexed_column_ref>(&column_)) {
    ref->table = &table;
  }
}
} // namespace matador::query
