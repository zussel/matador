#include "matador/query/table.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace matador::query {
table::table(const char* name)
: table(name == nullptr ? throw std::invalid_argument("Table name must not be null") :
                           std::string(name))
{}

table::table(std::string name)
: table("", std::move(name), "", {}) {}

table::table(std::string name, std::vector<column> columns)
: table("", std::move(name), "", std::move(columns)) {
}

table::table(std::string schema_name, std::string name, std::vector<column> columns)
: table(std::move(schema_name), std::move(name), "", std::move(columns)) {}

table::table(std::string schema_name, std::string name, std::string alias,
             std::vector<column> columns)
: name_(std::move(name))
, alias_(std::move(alias))
, schema_name_(std::move(schema_name))
, columns_(std::move(columns)) {
  rebind_columns();
}

table::table(const table &other)
: name_(other.name_)
, alias_(other.alias_)
, schema_name_(other.schema_name_)
, columns_(other.columns_)
, pk_column_index_(other.pk_column_index_) {
  for (auto &col : columns_) {
    col.table(this);
  }
}

table & table::operator=(const table &other) {
  if (this == &other) {
    return *this;
  }
  return *this = table(other);
}

table::table(table &&other) noexcept
: name_(std::move(other.name_))
, alias_(std::move(other.alias_))
, schema_name_(std::move(other.schema_name_))
, columns_(std::move(other.columns_))
, pk_column_index_(other.pk_column_index_) {
  for (auto &col : columns_) {
    col.table(this);
  }
  other.columns_.clear();
  other.pk_column_index_.reset();
}

table & table::operator=(table &&other) noexcept {
  name_ = std::move(other.name_);
  alias_ = std::move(other.alias_);
  schema_name_ = std::move(other.schema_name_);
  columns_ = std::move(other.columns_);
  pk_column_index_ = other.pk_column_index_;
  for (auto &col : columns_) {
    col.table(this);
  }
  other.columns_.clear();
  other.pk_column_index_.reset();
  return *this;
}

bool table::operator==(const table& x) const {
  return schema_name_ == x.schema_name_ && name_ == x.name_ && alias_ == x.alias_;
}

table table::as(const std::string &alias) const {
  return {schema_name_, name_, alias, columns_};
}

const std::string & table::table_name() const {
  return name_;
}

std::string table::name() const {
  return has_alias() ? alias_ : qualified_name();
}

const std::vector<column>& table::columns() const {
  return columns_;
}

std::vector<constraint> table::constraints() const {
  constexpr column_constraint constraint_kinds[] = {
    column_constraint::Index,
    column_constraint::Unique,
    column_constraint::PrimaryKey,
    column_constraint::ForeignKey,
    column_constraint::Identity,
    column_constraint::Default,
    column_constraint::NotNull
  };

  std::vector<constraint> result;
  for (std::size_t column_index = 0; column_index < columns_.size(); ++column_index) {
    const auto column_constraints = columns_[column_index].constraints();
    for (const auto kind : constraint_kinds) {
      if (column_constraints.has(kind)) {
        result.emplace_back(*this, column_index, kind);
      }
    }
  }
  return result;
}

bool table::has_alias() const {
  return !alias_.empty();
}

table::operator const std::vector<query::column>&() const {
  return columns_;
}

const column* table::operator[](const std::string &column_name) const {
  return find_column(column_name);
}

const column* table::find_column(const std::string_view column_name) const {
  const auto it = std::find_if(columns_.begin(), columns_.end(),
                               [column_name](const column& col) {
                                 return col.column_name().size() == column_name.size() &&
                                        col.column_name().compare(
                                          0, column_name.size(), column_name.data(),
                                          column_name.size()) == 0;
                               });
  return it == columns_.end() ? nullptr : &*it;
}

const column& table::at_column(const std::string_view column_name) const {
  const auto* column = find_column(column_name);
  if (column == nullptr) {
    throw std::invalid_argument("Unknown column: " + std::string(column_name));
  }
  return *column;
}

const column * table::column_by_name(const table &tab, const std::string &column_name) {
  return tab.find_column(column_name);
}

const column &table::column_ref_by_name(const table &tab, const std::string &column_name) {
  return tab.at_column(column_name);
}

const std::string& table::schema_name() const {
  return schema_name_;
}

std::string table::qualified_name() const {
  return schema_name_.empty() ? name_ : schema_name_ + "." + name_;
}

bool table::has_primary_key() const {
  return pk_column_index_.has_value();
}

const column* table::primary_key_column() const {
  return pk_column_index_ ? &columns_.at(*pk_column_index_) : nullptr;
}

void table::validate_schema(const std::vector<column>& columns) {
  if (std::any_of(columns.begin(), columns.end(),
                  [](const column& col) { return col.is_expression(); })) {
    throw std::invalid_argument("Table schemas cannot contain expression columns");
  }
}

void table::rebind_columns() {
  validate_schema(columns_);
  pk_column_index_.reset();
  for (std::size_t i = 0; i < columns_.size(); ++i) {
    if (columns_[i].is_primary_key()) {
      if (pk_column_index_) {
        throw std::invalid_argument("Table schemas cannot contain multiple primary keys");
      }
      pk_column_index_ = i;
    }
    columns_[i].table(this);
  }
}
}
