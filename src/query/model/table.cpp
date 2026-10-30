#include "matador/query/model/table.hpp"

#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace matador::query {
table table::make_primary_key_table(std::string name, std::vector<column> columns, std::string schema_name) {
  return {std::move(name), std::move(columns), {
    std::move(schema_name)
  }};
}

table table::make_relation_table(std::string name, std::vector<column> columns,
                                 const std::size_t join_column_index,
                                 const std::size_t inverse_join_column_index,
                                 std::string schema_name) {
  return {std::move(name), std::move(columns), {
    std::move(schema_name),
    {},
    {},
    join_column_info{
      join_column_index,
      inverse_join_column_index
    }
  }};
}

table::table(std::string name)
  : table(std::move(name), {}, {})
{}

table::table(std::string name, std::vector<column> columns)
  : table(std::move(name), std::move(columns), {})
{}

table::table(std::string name, std::vector<column> columns, options opts)
  : name_(std::move(name))
  , alias_(std::move(opts.alias))
  , schema_name_(std::move(opts.schema_name))
  , columns_(std::move(columns))
{
  validate_schema(columns_);

  join_column_info_ = opts.join_info;
  pk_column_index_ = opts.primary_key_column_index;

  if (!is_primary_key_table()) {
    for (std::size_t index = 0; index < columns_.size(); ++index) {
      if (!columns_[index].is_primary_key()) {
        continue;
      }

      if (is_primary_key_table()) {
        throw std::invalid_argument("Table schemas cannot contain multiple primary keys");
      }

      update_primary_key_index(index);
    }
  }

  rebind_columns();
  create_constraints();
  rebind_constraints();
}

table::table(const table &other)
: name_(other.name_)
, alias_(other.alias_)
, schema_name_(other.schema_name_)
, columns_(other.columns_)
, constraints_(other.constraints_)
, pk_column_index_(other.pk_column_index_)
, join_column_info_(other.join_column_info_) {
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
, pk_column_index_(other.pk_column_index_)
, join_column_info_(other.join_column_info_) {
  for (std::size_t index = 0; index < columns_.size(); ++index) {
    if (auto *plain = columns_[index].plain(); plain != nullptr) {
      plain->table = this;
      plain->index = index;
    }
  }
  rebind_constraints();
  other.constraints_.clear();
  other.columns_.clear();
  other.pk_column_index_ = std::nullopt;
  other.join_column_info_ = {};
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
  pk_column_index_ = other.pk_column_index_;
  join_column_info_ = other.join_column_info_;
  for (std::size_t index = 0; index < columns_.size(); ++index) {
    if (auto *plain = columns_[index].plain(); plain != nullptr) {
      plain->table = this;
      plain->index = index;
    }
  }
  rebind_constraints();
  other.constraints_.clear();
  other.columns_.clear();
  other.pk_column_index_ = std::nullopt;
  other.join_column_info_ = {};
  return *this;
}

bool table::operator==(const table& x) const {
  return schema_name_ == x.schema_name_ && name_ == x.name_ && alias_ == x.alias_;
}

table table::as(const std::string &alias) const {
  return {name_, columns_, {
    schema_name_,
    alias,
    pk_column_index_,
    join_column_info_
  }};
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

void table::update_primary_key_index(size_t index) {
  pk_column_index_ = index;
}

void table::update_relation_columns(const size_t join_column_index, const size_t inverse_join_column_index) {
  join_column_info_ = {
    join_column_index,
    inverse_join_column_index
  };
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

bool table::is_primary_key_table() const {
  return pk_column_index_.has_value();
}

bool table::is_relation_table() const {
  return join_column_info_.has_value();
}

const std::string& table::schema_name() const {
  return schema_name_;
}

std::string table::qualified_name() const {
  return schema_name_.empty() ? name_ : schema_name_ + "." + name_;
}

const column* table::primary_key_column() const {
  if (!pk_column_index_.has_value()) {
    return nullptr;
  }

  const auto index = *pk_column_index_;
  if (index >= columns_.size()) {
    return nullptr;
  }

  return &columns_[index];
}

const column* table::join_column() const {
  if (!join_column_info_.has_value() || join_column_info_->index > columns_.size()) {
    return nullptr;
  }

  return &columns_[join_column_info_->index];
}

const column* table::inverse_join_column() const {
  if (!join_column_info_.has_value() || join_column_info_->inverse_index > columns_.size()) {
    return nullptr;
  }

  return &columns_[join_column_info_->inverse_index];
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
    c.rebind_table(*this);
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
}
