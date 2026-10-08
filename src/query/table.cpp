#include "matador/query/table.hpp"

#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace matador::query {
table table::make_plain(std::string name, std::string schema_name) {
  return {std::move(schema_name), std::move(name), "", {}};
}

table table::make_primary_key_table(std::string name, std::vector<column> columns, std::string schema_name) {
  return {std::move(schema_name), std::move(name), "", std::move(columns)};
}

table table::make_relation_table(std::string name, std::vector<column> columns, std::size_t join_column_index, std::size_t inverse_join_column_index, std::string schema_name) {
  return {std::move(schema_name), std::move(name), "", std::move(columns)};
}

// table::table(const char* name)
// : table(name == nullptr ? throw std::invalid_argument("Table name must not be null") :
                           // std::string(name))
// {}

// table::table(std::string name)
// : table("", std::move(name), "", {}) {}

// table::table(std::string name, std::vector<column> columns)
// : table("", std::move(name), "", std::move(columns)) {
// }

// table::table(std::string schema_name, std::string name, std::vector<column> columns)
// : table(std::move(schema_name), std::move(name), "", std::move(columns)) {}

table::table(std::string schema_name, std::string name, std::string alias, std::vector<column> columns)
: name_(std::move(name))
, alias_(std::move(alias))
, schema_name_(std::move(schema_name))
, columns_(std::move(columns)) {
  rebind_columns();
  create_constraints();

  for (std::size_t index = 0; index < columns_.size(); ++index) {
    if (!columns_[index].is_primary_key()) {
      continue;
    }

    if (has_primary_key()) {
      throw std::invalid_argument("Table schemas cannot contain multiple primary keys");
    }

    make_primary_key_table(index);
  }
}
table::table(std::string schema_name, std::string name, std::string alias,
             std::vector<column> columns, std::size_t join_column_index,
             std::size_t inverse_join_column_index)
: name_(std::move(name))
, alias_(std::move(alias))
, schema_name_(std::move(schema_name))
, columns_(std::move(columns))
, value_(relation_table{join_column_index, inverse_join_column_index}) {
  rebind_columns();
  rebind_constraints();

}

table::table(const table &other)
: name_(other.name_)
, alias_(other.alias_)
, schema_name_(other.schema_name_)
, columns_(other.columns_)
, constraints_(other.constraints_)
, value_(other.value_) {
  rebind_columns();
  rebind_constraints();
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
, constraints_(std::move(other.constraints_))
, value_(other.value_) {
  for (std::size_t index = 0; index < columns_.size(); ++index) {
    if (auto *plain = columns_[index].plain(); plain != nullptr) {
      plain->table = this;
      plain->index = index;
    }
  }
  rebind_constraints();
  other.constraints_.clear();
  other.columns_.clear();
  other.value_ = primary_key_table{};
}

table & table::operator=(table &&other) noexcept {
  if (this == &other) {
    return *this;
  }

  name_ = std::move(other.name_);
  alias_ = std::move(other.alias_);
  schema_name_ = std::move(other.schema_name_);
  columns_ = std::move(other.columns_);
  constraints_ = std::move(other.constraints_);
  value_ = other.value_;
  for (std::size_t index = 0; index < columns_.size(); ++index) {
    if (auto *plain = columns_[index].plain(); plain != nullptr) {
      plain->table = this;
      plain->index = index;
    }
  }
  rebind_constraints();
  other.constraints_.clear();
  other.columns_.clear();
  other.value_ = primary_key_table{};
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

void table::update_name(const std::string& name) {
  name_ = name;
}

const std::vector<constraint>& table::constraints() const {
  return constraints_;
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

bool table::is_primary_key_table() const {
  return std::holds_alternative<primary_key_table>(value_);
}

bool table::is_relation_table() const {
  return std::holds_alternative<relation_table>(value_);
}

bool table::has_primary_key() const {
  const auto *data = primary_key_data();
  return data && data->pk_column_index.has_value();
}

const column* table::primary_key_column() const {
  const auto *data = primary_key_data();
  if (!data || !data->pk_column_index.has_value()) {
    return nullptr;
  }

  const auto index = *data->pk_column_index;
  if (index >= columns_.size()) {
    return nullptr;
  }

  return &columns_[index];
}

const column* table::join_column() const {
  const auto *data = relation_data();
  if (!data || data->join_column_index >= columns_.size()) {
    return nullptr;
  }

  return &columns_[data->join_column_index];
}

const column* table::inverse_join_column() const {
  const auto *data = relation_data();
  if (!data || data->inverse_join_column_index >= columns_.size()) {
    return nullptr;
  }

  return &columns_[data->inverse_join_column_index];
}

void table::validate_schema(const std::vector<column>& columns) {
  std::unordered_set<std::string> column_names;

  for (const auto& col : columns) {
    if (!col.is_plain_column()) {
      throw std::invalid_argument("table schema must contain plain columns only");
    }

    if (col.column_name().empty()) {
      throw std::invalid_argument("table schema contains a column with an empty name");
    }

    if (!column_names.insert(col.column_name()).second) {
      throw std::invalid_argument("table schema contains duplicate column '" + col.column_name() + "'");
    }
  }
}

void table::rebind_columns() {
  validate_schema(columns_);
  for (std::size_t index = 0; index < columns_.size(); ++index) {
    auto& col = columns_[index];

    auto* plain = col.plain();
    if (plain == nullptr) {
      throw std::invalid_argument("table schema must contain plain columns only");
    }

    plain->table = this;
    plain->index = index;
  }
}

void table::rebind_constraints() {
  for (auto& c : constraints_) {
    c.table_ = this;
  }
}

void table::create_constraints() {
  constexpr column_constraint constraint_kinds[] = {
    column_constraint::Index,
    column_constraint::Unique,
    column_constraint::PrimaryKey,
    column_constraint::ForeignKey,
    column_constraint::Identity,
    column_constraint::Default,
    column_constraint::NotNull
  };

  constraints_.clear();
  for (std::size_t column_index = 0; column_index < columns_.size(); ++column_index) {
    const auto column_constraints = columns_[column_index].constraints();
    for (const auto kind : constraint_kinds) {
      if (column_constraints.has(kind)) {
        constraints_.emplace_back(constraint::make_column_constraint(*this, column_index, kind));
      }
    }
  }
}

const table::primary_key_table* table::primary_key_data() const {
  return std::get_if<primary_key_table>(&value_);
}

table::primary_key_table* table::primary_key_data() {
  return std::get_if<primary_key_table>(&value_);
}

const table::relation_table* table::relation_data() const {
  return std::get_if<relation_table>(&value_);
}

table::relation_table* table::relation_data() {
  return std::get_if<relation_table>(&value_);
}

void table::make_primary_key_table(const std::optional<std::size_t> pk_column_index) {
  if (pk_column_index && *pk_column_index >= columns_.size()) {
    throw std::out_of_range("primary key column index is out of range");
  }

  value_ = primary_key_table{pk_column_index};
}

void table::make_relation_table(const std::size_t join_column_index, const std::size_t inverse_join_column_index) {
  if (join_column_index >= columns_.size()) {
    throw std::out_of_range("join column index is out of range");
  }

  if (inverse_join_column_index >= columns_.size()) {
    throw std::out_of_range("inverse join column index is out of range");
  }

  value_ = relation_table{
    join_column_index,
    inverse_join_column_index
  };
}
}
